/****************************************************************************
 * PTRapi  - Cursor routines
 *----------------------------------------------------------------------------
 * Copyright (C) 1992  Scott Hostynski All Rights Reserved
 *----------------------------------------------------------------------------
 *
 * Created by:  Scott Host
 * Date:        Oct, 1992
 * Modifed:     Mar, 1993 ( made work with Watcom C )
 *
 * CONTENTS: ptrapi.c ptrapi_a.asm ptrapi.h
 *
 * EXTERN MODULES - GFX, TSM(timer) and MOUSE.LIB
 *
 * NOTES:
 *        PTR_UpdateCursor should be called by an intr. routine
 *        PTR_FrameHook should not be called in an intr. routine
 *
 *---------------------------------------------------------------------------*/
#include <conio.h>
#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dpmiapi.h"
#include "exitapi.h"
#include "gfxapi.h"
#include "kbdapi.h"
#include "ptrapi.h"
#include "tsmapi.h"

#define JOYADD       8
#define CURSORHEIGHT 16
#define CURSORWIDTH  16
#define CURSORSIZE   ( CURSORHEIGHT * CURSORWIDTH )
#define HOTSPOTCOLOR 255

extern int ud_x; // update x pos
extern int ud_y; // update y pos
extern int ud_lx; // update x length
extern int ud_ly; // update y length

#define MOUSE_MV 0x01 // Mouse movement occurred
#define LB_PRESS 0x02 // Left button pressed
#define LB_OFF   0x04 // Left button released
#define RB_PRESS 0x08 // Right button pressed
#define RB_OFF   0x10 // Right button released
#define CB_OFF   0x20 // Center button released
#define CB_PRESS 0x40 // Center button pressed

bool mouse_b1_ack = false;
bool mouse_b2_ack = false;
bool mouse_b3_ack = false;
BYTE* cursorstart;
BYTE* displaypic;
BYTE* cursorpic;
BYTE* cursorsave;
bool mousepresent = false;
bool joypresent = false;
bool joyactive = false;
int joy2b1 = 0;
int joy2b2 = 0;
int mouseb1 = 0;
int mouseb2 = 0;
int mouseb3 = 0;
int cursorx = 0;
int cursory = 0;
int cursorloopx = 0;
int cursorloopy = 0;
int cur_mx = 0;
int cur_my = 0;
PRIVATE int dm_x = 0;
PRIVATE int dm_y = 0;
PRIVATE int hot_mx = 0;
PRIVATE int hot_my = 0;
PRIVATE bool mouseonhold = false;
PRIVATE bool mouseaction = true;
PRIVATE bool mouse_erase = false;
PRIVATE bool not_in_update = true;
PRIVATE void ( *cursorhook )( void ) = (void( * )) 0;
PRIVATE void ( *checkbounds )( void ) = (void( * )) 0;

bool drawcursor = false;
PRIVATE bool g_paused = false;
PRIVATE bool lastclip = false;
PRIVATE DWORD tsm_id = EMPTY;
int joy_limit_xh = 10;
int joy_limit_xl = -10;
int joy_limit_yh = 10;
int joy_limit_yl = -10;
int joy_sx = 0;
int joy_sy = 0;
PRIVATE bool joy_present = false;
int joy_x = 0;
int joy_y = 0;
int joy_buttons = 0;
bool ptr_init_flag = false;
PRIVATE int g_addx = 0;
PRIVATE int g_addy = 0;

/*------------------------------------------------------------------------
PTR_IsJoyPresent() - Checks to see if joystick is present
  ------------------------------------------------------------------------*/
bool PTR_IsJoyPresent( void ) {
    int i;
    int rval = true;

    _disable();
    outp( 0x201, 1 );
    inp( 0x201 );
    for ( i = 0; i <= 10000; i++ ) {
        if ( ( inp( 0x201 ) & 0x0f ) == 0 ) {
            break;
        }
    }
    _enable();

    if ( i >= 10000 ) {
        rval = false;
    }

    return rval;
}

/*------------------------------------------------------------------------
   PTR_MouseHandler() - Mouse Handler Function
  ------------------------------------------------------------------------*/
void _loadds far PTR_MouseHandler( int m_bx, int m_cx, int m_dx ) {
#pragma aux PTR_MouseHandler parm[EBX][ECX][EDX]

    if ( not_in_update ) {
        cur_mx = m_cx & 0xffff;
        cur_mx = cur_mx >> 1;
        cur_my = m_dx;
        mouseb1 = m_bx & 1;
        mouseb2 = m_bx & 2;
        mouseb3 = m_bx & 4;
        mouseaction = true;
        if ( mouseb1 ) {
            mouse_b1_ack = true;
        }
        if ( mouseb2 ) {
            mouse_b2_ack = true;
        }
        if ( mouseb3 ) {
            mouse_b3_ack = true;
        }
    }
}

void PTR_JoyReset( void ) {
    if ( joyactive ) {
        g_addx = 0;
        g_addy = 0;
    }
}

/*------------------------------------------------------------------------
  PTR_JoyHandler () - Handles Joystick Input
  ------------------------------------------------------------------------*/
void PTR_JoyHandler( void ) {
    int xm;
    int ym;

    if ( !joyactive ) {
        return;
    }

    _disable();
    PTR_ReadJoyStick();
    _enable();

    mouseb1 = joy_buttons & 1;
    mouseb2 = joy_buttons & 2;
    joy2b1 = joy_buttons & 4;
    joy2b2 = joy_buttons & 8;

    if ( mouseb1 ) {
        mouse_b1_ack = true;
    }

    if ( mouseb2 ) {
        mouse_b2_ack = true;
    }

    xm = joy_x - joy_sx;
    ym = joy_y - joy_sy;

    if ( xm < joy_limit_xl || xm > joy_limit_xh ) {
        if ( xm < 0 ) {
            if ( g_addx > 0 ) {
                g_addx = 0;
            } else {
                g_addx--;
            }

            if ( -g_addx > JOYADD ) {
                g_addx = -JOYADD;
            }
        } else {
            if ( g_addx < 0 ) {
                g_addx = 0;
            } else {
                g_addx++;
            }
            if ( g_addx > JOYADD ) {
                g_addx = JOYADD;
            }
        }
    } else {
        g_addx = 0;
    }

    if ( ym < joy_limit_yl || ym > joy_limit_yh ) {
        if ( ym < 0 ) {
            if ( g_addy > 0 ) {
                g_addy = 0;
            } else {
                g_addy--;
            }
            if ( -g_addy > JOYADD ) {
                g_addy = -JOYADD;
            }
        } else {
            if ( g_addy < 0 ) {
                g_addy = 0;
            } else {
                g_addy++;
            }
            if ( g_addy > JOYADD ) {
                g_addy = JOYADD;
            }
        }
    } else {
        g_addy = 0;
    }

    cur_mx += g_addx;
    cur_my += g_addy;

    if ( cur_mx < 0 ) {
        cur_mx = 0;
    } else if ( cur_mx > 319 ) {
        cur_mx = 319;
    }

    if ( cur_my < 0 ) {
        cur_my = 0;
    } else if ( cur_my > 199 ) {
        cur_my = 199;
    }

    mouseaction = true;
}

/*------------------------------------------------------------------------
   PTR_ClipCursor () Clips cursor from screen
  ------------------------------------------------------------------------*/
PRIVATE void PTR_ClipCursor( void ) {
    lastclip = false;

    displaypic = cursorpic;

    if ( dm_x + CURSORWIDTH > SCREENWIDTH ) {
        cursorloopx = SCREENWIDTH - dm_x;
        lastclip = true;
    } else {
        if ( dm_x < 0 ) {
            displaypic -= dm_x;
            cursorloopx = CURSORWIDTH + dm_x;
            dm_x = 0;
            lastclip = true;
        } else {
            cursorloopx = CURSORWIDTH;
        }
    }

    if ( dm_y + CURSORHEIGHT > SCREENHEIGHT ) {
        cursorloopy = SCREENHEIGHT - dm_y;
        lastclip = true;
    } else {
        if ( dm_y < 0 ) {
            displaypic += -dm_y * CURSORWIDTH;
            cursorloopy = CURSORHEIGHT + dm_y;
            dm_y = 0;
            lastclip = true;
        } else {
            cursorloopy = CURSORHEIGHT;
        }
    }
}

/*========================================================================
  PTR_UpdateCursor() - Updates Mouse Cursor - should be called by intterupt
  ========================================================================*/
void PTR_UpdateCursor( void ) {
    if ( mouseonhold ) {
        return;
    }

    if ( joyactive ) {
        PTR_JoyHandler();
    }

    if ( mouseaction ) {
        not_in_update = false;
        mouseaction = false;

        if ( mouse_erase ) {
            if ( lastclip ) {
                PTR_ClipErase();
            } else {
                PTR_Erase();
            }

            mouse_erase = false;
        }

        if ( checkbounds ) {
            checkbounds();
        }

        dm_x = cur_mx;
        dm_y = cur_my;

        dm_x -= hot_mx;
        dm_y -= hot_my;

        if ( drawcursor ) {
            PTR_ClipCursor();

            cursorstart = displayscreen + dm_x + ylookup[dm_y];

            PTR_Save( CURSORHEIGHT );
            PTR_Draw();

            mouse_erase = true;
        }

        if ( cursorhook ) {
            cursorhook();
        }

        cursorx = dm_x;
        cursory = dm_y;
        not_in_update = true;
    }
}

/*==========================================================================
  PTR_FrameHook() - Mouse framehook Function
 ==========================================================================*/
void PTR_FrameHook(
    void ( *update )( void ) // INPUT : pointer to function
) {
    int ck_x1;
    int ck_y1;
    int ck_x2;
    int ck_y2;

    if ( !drawcursor ) {
        update();
        return;
    }

    while ( !(volatile) not_in_update ) {
    }
    not_in_update = false;
    mouseonhold = true;

    if ( joyactive ) {
        PTR_JoyHandler();
    }

    if ( checkbounds ) {
        checkbounds();
    }

    dm_x = cur_mx - hot_mx;
    dm_y = cur_my - hot_my;
    not_in_update = true;

    ck_x1 = ud_x - CURSORWIDTH;
    ck_y1 = ud_y - CURSORHEIGHT;
    ck_x2 = ud_x + ud_lx;
    ck_y2 = ud_y + ud_ly;

    if ( dm_x >= ck_x1 && dm_x <= ck_x2 && dm_y >= ck_y1 && dm_y <= ck_y2 ) {
        if ( mouse_erase ) {
            if ( cursorx >= ck_x1 && cursorx <= ck_x2 && cursory >= ck_y1 && cursory <= ck_y2 ) {
                GFX_MarkUpdate( cursorx, cursory, cursorloopx, cursorloopy );
            } else {
                if ( lastclip ) {
                    PTR_ClipErase();
                } else {
                    PTR_Erase();
                }

                mouse_erase = false;
            }
        }

        PTR_ClipCursor();

        GFX_MarkUpdate( dm_x, dm_y, cursorloopx, cursorloopy );

        cursorstart = displaybuffer + dm_x + ylookup[dm_y];

        PTR_Save( ( cursorloopy < CURSORHEIGHT ) ? cursorloopy : CURSORHEIGHT );

        PTR_Draw();

        update();

        if ( lastclip ) {
            PTR_ClipErase();
        } else {
            PTR_Erase();
        }

        cursorstart = displayscreen + dm_x + ylookup[dm_y];
        mouse_erase = true;
    } else {
        if ( mouseaction ) {
            if ( mouse_erase ) {
                if ( lastclip ) {
                    PTR_ClipErase();
                } else {
                    PTR_Erase();
                }

                mouse_erase = false;
            }

            PTR_ClipCursor();

            cursorstart = displayscreen + dm_x + ylookup[dm_y];

            PTR_Save( CURSORHEIGHT );
            PTR_Draw();

            mouse_erase = true;
        }
        update();
    }

    if ( cursorhook ) {
        cursorhook();
    }

    cursorx = dm_x;
    cursory = dm_y;
    mouseonhold = false;
}

/***************************************************************************
PTR_CalJoy() - Calibrate Joystick
 ***************************************************************************/
void PTR_CalJoy( void ) {
    _disable();
    PTR_ReadJoyStick();
    _enable();

    joy_sx = joy_x;
    joy_sy = joy_y;
}

/***************************************************************************
   PTR_DrawCursor () - Turns Cursor Drawing ON/OFF
 ***************************************************************************/
void PTR_DrawCursor( bool flag ) {
    if ( ptr_init_flag ) {
        if ( !flag && mouse_erase ) {
            if ( lastclip ) {
                PTR_ClipErase();
            } else {
                PTR_Erase();
            }
            mouse_erase = false;
        }

        if ( flag ) {
            mouseaction = true;
        }

        drawcursor = flag;
    } else {
        drawcursor = false;
    }
}

/***************************************************************************
   PTR_SetPic () - Sets up a new cursor picture with hotspot
 ***************************************************************************/
void PTR_SetPic(
    BYTE* newp // INPUT : pointer to new Cursor picture
) {
    BYTE* pic;
    int i;

    hot_mx = 0;
    hot_my = 0;

    if ( !ptr_init_flag ) {
        return;
    }

    newp += sizeof( GFX_PIC );

    pic = cursorpic;

    for ( i = 0; i < CURSORSIZE; i++, newp++, pic++ ) {
        *pic = *newp;
        if ( *newp == (BYTE) HOTSPOTCOLOR ) {
            hot_mx = i % CURSORWIDTH;
            hot_my = i / CURSORWIDTH;
            *pic = *( newp + 1 );
        }
    }

    if ( hot_mx > 16 ) {
        hot_mx = 0;
    }

    if ( hot_my > 16 ) {
        hot_my = 0;
    }

    mouseaction = true;
}

/***************************************************************************
 PTR_SetBoundsHook() - Sets User function to OK or change mouse x,y values
 ***************************************************************************/
void // RETURN: none
PTR_SetBoundsHook(
    void ( *func )( void ) // INPUT : pointer to function
) {
    checkbounds = func;
    mouseaction = true;
}

/***************************************************************************
 PTR_SetCursorHook() - Sets User function to call from mouse handler
 ***************************************************************************/
void // RETURN: none
PTR_SetCursorHook(
    void ( *hook )( void ) // INPUT : pointer to function
) {
    cursorhook = hook;
    mouseaction = true;
}

/***************************************************************************
   PTR_SetUpdateFlag () - Sets cursor to be update next cycle
 ***************************************************************************/
void PTR_SetUpdateFlag( void ) {
    mouseaction = true;
}

/***************************************************************************
 PTR_SetPos() - Sets Cursor Position
 ***************************************************************************/
void // RETURN: none
PTR_SetPos(
    int x, // INPUT : x position
    int y // INPUT : y position
) {
    union REGS regs;

    if ( mousepresent ) {
        // Set Mouse Position ================
        regs.x.eax = 4;
        regs.x.ecx = x << 1;
        regs.x.edx = y;
        int386( 0x33, &regs, &regs );
    }

    cur_mx = x;
    cur_my = y;

    if ( ptr_init_flag ) {
        mouseaction = true;
    }
}

/***************************************************************************
PTR_Pause() - Pauses/ Starts PTR routines after already initing
 ***************************************************************************/
void PTR_Pause( bool flag ) {
    if ( !ptr_init_flag ) {
        return;
    }

    if ( flag == g_paused ) {
        return;
    }

    if ( flag ) {
        PTR_DrawCursor( false );
        TSM_PauseService( tsm_id );
    } else {
        drawcursor = false;
        TSM_ResumeService( tsm_id );
        PTR_SetPos( 160, 100 );
        PTR_DrawCursor( false );
    }

    g_paused = flag;
}

/***************************************************************************
 PTR_Init() - Inits Mouse Driver and sets mouse handler function
 ***************************************************************************/
bool // RETURN true = Installed, false  = No mouse
PTR_Init(
    PTRTYPE type // INPUT : Pointer Type to Use
) {
    char* err = "PTR_Init() - DosMemAlloc";
    struct SREGS sregs;
    union REGS regs;
    void( far * function_ptr )();
    DWORD segment;

    drawcursor = false;

    if ( _dpmi_dosalloc( 16, &segment ) ) {
        EXIT_Error( err );
    }
    cursorsave = (BYTE*) ( segment << 4 );

    if ( _dpmi_dosalloc( 16, &segment ) ) {
        EXIT_Error( err );
    }
    cursorpic = (BYTE*) ( segment << 4 );

    joyactive = false;
    mousepresent = false;

    joy_present = false;

    if ( type == P_JOYSTICK ) {
        joy_present = true;
    }

    segread( &sregs );

    if ( type == P_AUTO || type == P_MOUSE ) {
        // Reset Mouse Driver ===================
        regs.w.ax = 0;

        if ( int386( 0x33, &regs, &regs ) ) {
            mousepresent = true;

            // Hide Mouse ========================
            regs.w.ax = 2;
            int386( 0x33, &regs, &regs );

            // Install Mouse Handler==============
            function_ptr = PTR_MouseHandler;
            regs.w.ax = 0x0C;
            regs.w.cx = LB_OFF | LB_PRESS | RB_OFF | RB_PRESS | MOUSE_MV;
            regs.x.edx = FP_OFF( function_ptr );
            sregs.es = FP_SEG( function_ptr );
            int386x( 0x33, &regs, &regs, &sregs );
        }
    }

    if ( type == P_JOYSTICK ) {
        if ( joy_present ) {
            joyactive = true;
        }
    }

    if ( mousepresent || joyactive ) {
        ptr_init_flag = true;
        tsm_id = TSM_NewService( PTR_UpdateCursor, 15, 254, 0 );
        GFX_SetFrameHook( PTR_FrameHook );
    } else {
        tsm_id = -1;
    }

    if ( joy_present ) {
        PTR_CalJoy();
    }

    PTR_SetPos( 160, 100 );

    return mousepresent || joyactive;
}

/***************************************************************************
 PTR_End() - End Cursor system
 ***************************************************************************/
void PTR_End( void ) {
    union REGS regs;

    if ( tsm_id != EMPTY ) {
        TSM_DelService( tsm_id );
    }

    // reset to remove mouse handler ========
    regs.x.eax = 0x0;
    int386( 0x33, &regs, &regs );
}

// ptrapi_a

void PTR_ReadJoyStick( void ) {
    __asm {
        pushad
        mov edx, 0x201
        xor edi, edi
        xor esi, esi
        xor ebx, ebx
        xor eax, eax
        mov ecx, 0x3FF

        out dx, al
        in al, dx
        in al, dx
        not al
        shr eax, 4
        mov joy_buttons, eax
        xor eax, eax

    LR_1:
        in al, dx
        and al, 0xF
        jz LR_2

        rcr al, 1
        adc edi, ebx
        rcr al, 1
        adc esi, ebx
        loop LR_1

    LR_2:
        mov joy_y, esi
        mov joy_x, edi

        popad
    }
}

void PTR_Copy( const uint8_t* src, uint8_t* dst, int max_y ) {
    for ( int y = 0; y < max_y; y++ ) {
        memcpy( dst, src, CURSORWIDTH );
        dst += CURSORWIDTH;
        src += SCREENWIDTH;
    }
}

void PTR_Save( int max_y ) {
    PTR_Copy( cursorstart, cursorsave, max_y );
}

void PTR_Erase( void ) {
    uint8_t* src = cursorsave;
    uint8_t* dst = cursorstart;
    for ( int y = 0; y < CURSORHEIGHT; y++ ) {
        memcpy( dst, src, CURSORWIDTH );
        dst += SCREENWIDTH;
        src += CURSORWIDTH;
    }
}

void PTR_ClipErase( void ) {
    uint8_t* src = cursorsave;
    uint8_t* dst = cursorstart;

    for ( int y = 0; y < cursorloopy; y++ ) {
        memcpy( dst, src, cursorloopx );
        dst += SCREENWIDTH;
        src += CURSORWIDTH;
    }
}

void PTR_Draw( void ) {
    uint8_t* src = displaypic;
    uint8_t* dst = cursorstart;

    for ( int y = 0; y < cursorloopy; y++ ) {
        for ( int x = 0; x < cursorloopx; x++ ) {
            uint8_t pixel = *src++;
            if ( pixel != 0 ) {
                *dst = pixel;
            }
            dst++;
        }
        src += CURSORWIDTH - cursorloopx;
        dst += SCREENWIDTH - cursorloopx;
    }
}
