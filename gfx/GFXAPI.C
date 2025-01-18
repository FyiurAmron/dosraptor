/****************************************************************************
 * GFXapi  - Graphic Library for 320x200 256 color vga/mcga
 *----------------------------------------------------------------------------
 * Copyright (C) 1992  Scott Hostynski All Rights Reserved
 *----------------------------------------------------------------------------
 *
 * Created by:  Scott Host
 * Date:        Oct, 1992
 *
 * CONTENTS: gfxapi.c gfxapi_a.asm gfxapi.h
 *
 * EXTERN MODULES - Timer Int routines to call GFX_TimeFrameRate at about
 *                  the refresh rate of the monitor
 *
 *---------------------------------------------------------------------------*/

#include <conio.h>
#include <dos.h>
#include <graph.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#include "dpmiapi.h"
#include "exitapi.h"
#include "tsmapi.h"

#include "gfxapi.h"

#define G3D_DIST 200

#define SCREENWIDTH  320
#define SCREENHEIGHT 200 // unused here

PUBLIC volatile int framecount = 0;
PUBLIC int g_rseed = 1;
PUBLIC DWORD ylookup[SCREENHEIGHT];
PUBLIC BYTE* displaybuffer = (BYTE*) 0x0000;
PUBLIC BYTE* displayscreen = (BYTE*) 0xa0000;
PUBLIC bool update_start = false;
PUBLIC int ud_x = 0;
PUBLIC int ud_y = 0;
PUBLIC int ud_lx = 0;
PUBLIC int ud_ly = 0;
PUBLIC int o_ud_x = 0;
PUBLIC int o_ud_y = 0;
PUBLIC int o_ud_lx = 0;
PUBLIC int o_ud_ly = 0;
PUBLIC int stable[324];
PUBLIC int tablelen;
PUBLIC int fontspacing = 1;
PUBLIC BYTE* ltable;
PUBLIC BYTE* dtable;
PUBLIC BYTE* gtable;
PUBLIC void ( *framehook )( void ( * )( void ) ) = (void( * )) 0;
PUBLIC bool retraceflag = true;

PUBLIC int G3D_x = 0; // input: x position
PUBLIC int G3D_y = 0; // input: y position
PUBLIC int G3D_z = 0; // input: z position
PUBLIC int G3D_screenx = 0; // output: screen x pos
PUBLIC int G3D_screeny = 0; // output: screen y pos
PUBLIC int G3D_viewx = 159; // user view x pos
PUBLIC int G3D_viewy = 99; // user view y pos
PUBLIC int G3D_viewz = G3D_DIST; // user view z pos

PRIVATE bool gfxdebug = false;
PRIVATE BYTE tpal1[768];
PRIVATE BYTE tpal2[768];

PRIVATE DWORD tsm_id;
PRIVATE int start_lookup = 0;
PRIVATE int end_lookup = 255;

// USED TO PASS STUFF TO GFXAPI_A
PUBLIC BYTE* gfx_inmem = NULL;
PUBLIC int gfx_xp = 0; // x pos
PUBLIC int gfx_yp = 0; // y pos
PUBLIC int gfx_lx = 0; // len x
PUBLIC int gfx_ly = 0; // len y
PUBLIC int gfx_imga = 0;

/*==========================================================================
   GFX_TimeFrameRate () - Should be interrupt called at 70 fps
 ==========================================================================*/
void GFX_TimeFrameRate( void ) {
    framecount++;
}

/***************************************************************************
GFX_SetDebug () - Sets Debug mode
 ***************************************************************************/
void GFX_SetDebug( bool flag ) {
    gfxdebug = flag;
}

/*************************************************************************
   GFX_ClipLines ()
 *************************************************************************/
int // RETURN: 0 = Off, 1 == No Clip , 2 == Clipped
GFX_ClipLines(
    BYTE** image, // INOUT : pointer to image or NULL
    int* x, // INOUT : pointer to x pos
    int* y, // INOUT : pointer to y pos
    int* lx, // INOUT : pointer to width
    int* ly // INOUT : pointer to length
) {
    int rval = 1;

    if ( *x >= SCREENWIDTH ) {
        return 0;
    }
    if ( *y >= SCREENHEIGHT ) {
        return 0;
    }
    if ( *x + *lx <= 0 ) {
        return 0;
    }
    if ( *y + *ly <= 0 ) {
        return 0;
    }

    if ( *y < 0 ) {
        rval = 2;
        if ( image ) {
            *image += ( -*y * *lx );
        }
        *ly += *y;
        *y = 0;
    }

    if ( *y + *ly > SCREENHEIGHT ) {
        rval = 2;
        *ly = SCREENHEIGHT - *y;
    }

    if ( *x < 0 ) {
        rval = 2;
        if ( image ) {
            *image += -*x;
        }
        *lx += *x;
        *x = 0;
    }

    if ( *x + *lx > SCREENWIDTH ) {
        rval = 2;
        *lx = SCREENWIDTH - *x;
    }

    return rval;
}

/**************************************************************************
   GFX_SetVideoMode13() - sets 256 color 320x200 mode
 **************************************************************************/
void GFX_SetVideoMode13( void ) {
    union REGS regs;
    regs.w.ax = 0x13;
    int386( 0x10, &regs, &regs );
}

/**************************************************************************
   GFX_RestoreMode() - set text mode
 **************************************************************************/
void GFX_RestoreMode( void ) {
    union REGS regs;
    regs.w.ax = 0x03;
    int386( 0x10, &regs, &regs );
}

/**************************************************************************
GFX_SetPalette() - Sets VGA palette
 **************************************************************************/
void GFX_SetPalette( BYTE* palette, int start_pal ) {
    volatile int num = 0;

    palette += start_pal * 3;

    if ( retraceflag ) {
    retrace1:
        num = inp( 0x3DA );
        if ( num & 8 ) {
            goto retrace1;
        }
    retrace2:
        num = inp( 0x3DA );
        if ( !( num & 8 ) ) {
            goto retrace2;
        }
    }

    outp( 0x3C8, start_pal );

    start_pal = ( 256 - start_pal ) * 3;

    while ( start_pal-- ) {
        outp( 0x3C9, *palette++ );
    }
}

/**************************************************************************
  GFX_InitSystem() - allocates buffers, makes tables, does not set vmode
 **************************************************************************/
void GFX_InitSystem( void ) {
    char* err = "GFX_Init() - DosMemAlloc";
    DWORD segment;

    if ( _dpmi_dosalloc( 4000, &segment ) ) {
        EXIT_Error( err );
    }
    displaybuffer = (BYTE*) ( segment << 4 );

    _dpmi_lockregion( displaybuffer, 64000 );
    memset( displaybuffer, 0, 64000 );

    tsm_id = TSM_NewService( GFX_TimeFrameRate, 70, 255, 0 );

    for ( int i = 0; i < SCREENHEIGHT; i++ ) {
        ylookup[i] = SCREENWIDTH * i;
    }

    if ( _dpmi_dosalloc( 32, &segment ) ) {
        EXIT_Error( err );
    }
    ltable = (BYTE*) ( segment << 4 );
    ltable = (BYTE*) ( ( (int) ltable + 255 ) & ~0xff );

    if ( _dpmi_dosalloc( 32, &segment ) ) {
        EXIT_Error( err );
    }
    dtable = (BYTE*) ( segment << 4 );
    dtable = (BYTE*) ( ( (int) dtable + 255 ) & ~0xff );

    if ( _dpmi_dosalloc( 32, &segment ) ) {
        EXIT_Error( err );
    }
    gtable = (BYTE*) ( segment << 4 );
    gtable = (BYTE*) ( ( (int) gtable + 255 ) & ~0xff );

    displayscreen = (BYTE*) 0xa0000;
}

/**************************************************************************
GFX_InitVideo() - Inits things related to Video, and sets up fade tables
 **************************************************************************/
void GFX_InitVideo( BYTE* curpal ) {
    GFX_SetVideoMode13();
    GFX_SetPalette( curpal, 0 );

    GFX_MakeLightTable( curpal, ltable, 9 );
    GFX_MakeLightTable( curpal, dtable, -9 );
    GFX_MakeGreyTable( curpal, gtable );
}

/**************************************************************************
  GFX_EndSystem() - Frees up all resources used by GFX system
 **************************************************************************/
void GFX_EndSystem( void ) {
    TSM_DelService( tsm_id );

    memset( displayscreen, 0, 64000 );

    GFX_RestoreMode();
}

/**************************************************************************
  GFX_GetPalette() - Sets 256 color palette
 **************************************************************************/
void GFX_GetPalette(
    BYTE* curpal // OUTPUT : pointer to palette data
) {
    int i;

    outp( 0x3c7, 0 );

    for ( i = 0; i < 768; i++ ) {
        *curpal++ = inp( 0x3c9 );
    }
}

/**************************************************************************
 GFX_FadeOut () - Fade Palette out to ( Red, Green , and Blue Value
 **************************************************************************/
void GFX_FadeOut(
    int red, // INPUT : red ( 0 - 63 )
    int green, // INPUT : green ( 0 - 63 )
    int blue, // INPUT : blue ( 0 - 63 )
    int steps // INPUT : steps of fade ( 0 - 255 )
) {
    int j, i;
    BYTE pal1[769];
    BYTE pal2[769];
    BYTE* optr;
    BYTE* nptr;
    int num;
    int delta;

    GFX_GetPalette( pal1 );
    memcpy( pal2, pal1, 768 );

    for ( j = 0; j < steps; j++ ) {
        optr = pal1;
        nptr = pal2;

        for ( i = 0; i < 256; i++ ) {
            num = *optr++;
            delta = red - num;
            *nptr++ = num + delta * j / steps;

            num = *optr++;
            delta = green - num;
            *nptr++ = num + delta * j / steps;

            num = *optr++;
            delta = blue - num;
            *nptr++ = num + delta * j / steps;
        }

        GFX_SetPalette( pal2, 0 );
    }

    nptr = pal2;
    for ( j = 0; j < 256; j++ ) {
        *nptr++ = red;
        *nptr++ = green;
        *nptr++ = blue;
    }

    GFX_SetPalette( pal2, 0 );
}

/**************************************************************************
 GFX_FadeIn () - Fades From current palette to new palette
 **************************************************************************/
void GFX_FadeIn(
    BYTE* palette, // INPUT : palette to fade into
    int steps // INPUT : steps of fade ( 0 - 255 )
) {
    BYTE pal1[768];
    BYTE pal2[768];

    GFX_GetPalette( pal1 );
    memcpy( pal2, pal1, 768 );

    for ( int j = 0; j < steps; j++ ) {
        for ( int i = 0; i < 768; i++ ) {
            int delta = palette[i] - pal1[i];
            pal2[i] = pal1[i] + delta * j / steps;
        }

        GFX_SetPalette( pal2, 0 );
    }

    GFX_SetPalette( palette, 0 );
}

/**************************************************************************
GFX_FadeStart () - Sets up fade for GFX_FadeFrame()
 **************************************************************************/
void GFX_FadeStart( void ) {
    GFX_GetPalette( tpal1 );
    memcpy( tpal2, tpal1, 768 );
}

/**************************************************************************
GFX_FadeFrame () - Fades Individual Frames
 **************************************************************************/
void GFX_FadeFrame(
    BYTE* palette, // INPUT : palette to fade into
    int cur_step, // INPUT : cur step position
    int steps // INPUT : total steps of fade ( 0 - 255 )
) {

    for ( int i = 0; i < 768; i++ ) {
        int delta = palette[i] - tpal1[i];
        tpal2[i] = tpal1[i] + delta * cur_step / steps;
    }

    GFX_SetPalette( tpal2, 0 );
}

/**************************************************************************
GFX_SetPalRange() - Sets start and end range for remaping stuff
 **************************************************************************/
void GFX_SetPalRange( int start, int end ) {
    if ( start < end && end < 256 && start >= 0 ) {
        start_lookup = start;
        end_lookup = end;
    }
}

/**************************************************************************
  GFX_GetRGB() - gets R,G and B values from pallete data
 **************************************************************************/
void GFX_GetRGB(
    BYTE* pal, // INPUT : pointer to palette data
    int num, // INPUT : palette entry
    int* red, // OUTPUT: red value
    int* green, // OUTPUT: green value
    int* blue // OUTPUT: blue value
) {
    *red = pal[num * 3];
    *green = pal[num * 3 + 1];
    *blue = pal[num * 3 + 2];
}

/**************************************************************************
  GFX_Remap() - Finds the closest color avalable
 **************************************************************************/
int // RETURN: new color number
GFX_Remap(
    BYTE* pal, // INPUT : pointer to palette data
    int red, // INPUT : red  ( 0 - 63 )
    int green, // INPUT : green( 0 - 63 )
    int blue // INPUT : blue ( 0 - 63 )
) {
    int pos = 0;
    int color[3];
    int diff[3];

    int low = 256 * 3 + 1;

    for ( int i = start_lookup; i < end_lookup + 1; i++ ) {

        GFX_GetRGB( pal, i, &color[0], &color[1], &color[2] );

        diff[0] = abs( color[0] - red );
        diff[1] = abs( color[1] - green );
        diff[2] = abs( color[2] - blue );

        int num = diff[0] + diff[1] + diff[2];

        if ( num <= low ) {
            low = num;
            pos = i;
        }
    }
    return pos;
}

/**************************************************************************
  GFX_MakeLightTable() - make a light/dark palette lookup table
 **************************************************************************/
void GFX_MakeLightTable(
    BYTE* palette, // INPUT : pointer to palette data
    BYTE* ltable, // OUTPUT: pointer to lookup table
    int level // INPUT : - 63 to + 63
) {
    int i;
    int red;
    int green;
    int blue;
    int n_red;
    int n_green;
    int n_blue;

    for ( i = 0; i < 256; i++ ) {
        GFX_GetRGB( palette, i, &red, &green, &blue );

        if ( red >= 0 ) {
            n_red = red + level;
        } else {
            n_red = 0;
        }

        if ( green >= 0 ) {
            n_green = green + level;
        } else {
            n_green = 0;
        }

        if ( blue >= 0 ) {
            n_blue = blue + level;
        } else {
            n_blue = 0;
        }

        if ( level >= 0 ) {
            if ( n_red > 63 ) {
                n_red = 63;
            }
            if ( n_green > 63 ) {
                n_green = 63;
            }
            if ( n_blue > 63 ) {
                n_blue = 63;
            }
        } else {
            if ( n_red < 0 ) {
                n_red = 0;
            }
            if ( n_green < 0 ) {
                n_green = 0;
            }
            if ( n_blue < 0 ) {
                n_blue = 0;
            }
        }

        ltable[i] = GFX_Remap( palette, n_red, n_green, n_blue );
    }
}

/**************************************************************************
  GFX_MakeGreyTable() - make a grey palette lookup table
 **************************************************************************/
void GFX_MakeGreyTable(
    BYTE* palette, // INPUT : pointer to palette data
    BYTE* ltable // OUTPUT: pointer to lookup table
) {
    int red, green, blue;

    for ( int i = 0; i < 256; i++ ) {
        GFX_GetRGB( palette, i, &red, &green, &blue );
        int avg = ( red + green + blue ) / 3;

        ltable[i] = GFX_Remap( palette, avg, avg, avg );
    }
}

/*************************************************************************
   GFX_GetScreen() -     Gets A block of screen memory to CPU memory
 *************************************************************************/
void GFX_GetScreen(
    BYTE* outmem, // OUTPUT: pointer to CPU mem
    int x, // INPUT : x pos
    int y, // INPUT : y pos
    int lx, // INPUT : x length
    int ly // INPUT : y length
) {

    if ( GFX_ClipLines( &outmem, &x, &y, &lx, &ly ) ) {
        BYTE* source;
        source = displaybuffer + x + ylookup[y];

        for ( int i = 0; i < ly; i++, outmem += lx, source += SCREENWIDTH ) {
            memcpy( outmem, source, (size_t) lx );
        }
    }
}

/*************************************************************************
   GFX_PutTexture() - Repeats a Picture though the area specified
 *************************************************************************/
void GFX_PutTexture(
    BYTE* intxt, // INPUT : color texture
    int x, // INPUT : x pos
    int y, // INPUT : y pos
    int lx, // INPUT : x length
    int ly // INPUT : y length
) {
    GFX_PIC* h = (GFX_PIC*) intxt;
    int xpos;
    int ypos;
    int maxxloop = abs( x ) + lx;
    int maxyloop = abs( y ) + ly;

    int x2 = x + lx - 1;
    int y2 = y + ly - 1;

    if ( x2 >= SCREENWIDTH ) {
        x2 = SCREENWIDTH - 1;
    }
    if ( y2 >= SCREENHEIGHT ) {
        y2 = SCREENHEIGHT - 1;
    }

    for ( int iy = y; iy < maxyloop; iy += h->height ) {
        if ( iy > y2 ) {
            continue;
        }

        if ( iy + h->height <= 0 ) {
            continue;
        }

        ypos = ( iy < 0 ) ? 0 : iy;

        for ( int ix = x; ix < maxxloop; ix += h->width ) {
            if ( ix > x2 ) {
                continue;
            }
            if ( ix + h->width <= 0 ) {
                continue;
            }

            BYTE* buf = intxt + sizeof( GFX_PIC );

            int new_lx = h->width;
            int new_ly = h->height;

            if ( ix < 0 ) {
                buf += -ix;
                new_lx += ix;
                xpos = 0;
            } else {
                xpos = ix;
            }

            if ( xpos + new_lx - 1 >= x2 ) {
                new_lx = x2 + 1 - xpos;
            }

            if ( ypos + new_ly - 1 >= y2 ) {
                new_ly = y2 + 1 - ypos;
            }

            gfx_inmem = buf;
            gfx_xp = xpos;
            gfx_yp = ypos;
            gfx_lx = new_lx;
            gfx_ly = new_ly;
            gfx_imga = h->width - new_lx;

            GFX_PutPic();

            GFX_MarkUpdate( xpos, ypos, new_lx, new_ly );
        }
    }
}

/*************************************************************************
   GFX_ShadeArea()- lightens or darkens and area of the screen
 *************************************************************************/
void GFX_ShadeArea(
    SHADE opt, // INPUT : DARK/LIGHT or GREY
    int x, // INPUT : x position
    int y, // INPUT : y position
    int lx, // INPUT : x length
    int ly // INPUT : y length
) {
    int i;

    if ( GFX_ClipLines( NULL, &x, &y, &lx, &ly ) ) {
        BYTE* buf = displaybuffer + x + ylookup[y];

        BYTE* cur_table;
        switch ( opt ) {
            case DARK:
                cur_table = dtable;
                break;

            case LIGHT:
                cur_table = ltable;
                break;

            case GREY:
                cur_table = gtable;
                break;
        }

        GFX_MarkUpdate( x, y, lx, ly );

        for ( i = 0; i < ly; i++ ) {
            GFX_Shade( buf, lx, cur_table );
            buf += SCREENWIDTH;
        }
    }
}

/*************************************************************************
   GFX_ShadeShape()- lightens or darkens and area of the screen
 *************************************************************************/
void GFX_ShadeShape(
    SHADE opt, // INPUT : DARK/LIGHT or GREY
    BYTE* inmem, // INPUT : mask 0 = no shade ( GFX format pic )
    int x, // INPUT : x position
    int y // INPUT : y position
) {
    GFX_PIC* h = (GFX_PIC*) inmem;
    GFX_SPRITE* ah;
    BYTE* cur_table;
    BYTE* dest;
    int ox = x;
    int oy = y;
    int lx = h->width;
    int ly = h->height;

    inmem += sizeof( GFX_PIC );

    int rval = GFX_ClipLines( NULL, &ox, &oy, &lx, &ly );
    if ( !rval ) {
        return;
    }

    switch ( opt ) {
        case DARK:
            cur_table = dtable;
            break;

        case LIGHT:
            cur_table = ltable;
            break;

        case GREY:
            cur_table = gtable;
            break;
    }

    switch ( rval ) {
        case 1:
            dest = displaybuffer + ox + ylookup[oy];

            GFX_ShadeSprite( dest, inmem, cur_table );
            break;

        case 2:
            ah = (GFX_SPRITE*) inmem;

            while ( ah->offset != EMPTY ) {
                inmem += sizeof( GFX_SPRITE );

                ox = ah->x + x;
                oy = ah->y + y;

                if ( oy > SCREENHEIGHT ) {
                    break;
                }

                lx = ah->length;
                ly = 1;

                if ( GFX_ClipLines( NULL, &ox, &oy, &lx, &ly ) ) {
                    GFX_Shade( displaybuffer + ox + ylookup[oy], lx, cur_table );
                }

                inmem += ah->length;

                ah = (GFX_SPRITE*) inmem;
            }
            break;
        default:
            break;
    }
}

/*************************************************************************
   GFX_VShadeLine () - Shades a vertical line
 *************************************************************************/
void GFX_VShadeLine(
    SHADE opt, // INPUT : DARK/LIGHT or GREY
    int x, // INPUT : x position
    int y, // INPUT : y position
    int ly // INPUT : length of line
) {
    int lx = 1;

    if ( ly < 1 ) {
        return;
    }

    if ( GFX_ClipLines( NULL, &x, &y, &lx, &ly ) ) {
        BYTE* cur_table;
        switch ( opt ) {
            case DARK:
                cur_table = dtable;
                break;

            case LIGHT:
                cur_table = ltable;
                break;

            case GREY:
                cur_table = gtable;
                break;
        }

        GFX_MarkUpdate( x, y, lx, ly );

        BYTE* outbuf = displaybuffer + x + ylookup[y];

        while ( ly-- ) {
            *outbuf = *( cur_table + *outbuf );
            outbuf += SCREENWIDTH;
        }
    }
}

/*************************************************************************
   GFX_HShadeLine () Shades a Horizontal Line
 *************************************************************************/
void GFX_HShadeLine(
    SHADE opt, // INPUT : DARK/LIGHT or GREY
    int x, // INPUT : x position
    int y, // INPUT : y position
    int lx // INPUT : length of line
) {
    int ly = 1;

    if ( lx < 1 ) {
        return;
    }

    if ( GFX_ClipLines( NULL, &x, &y, &lx, &ly ) ) {
        BYTE* cur_table;
        switch ( opt ) {
            case DARK:
                cur_table = dtable;
                break;

            case LIGHT:
                cur_table = ltable;
                break;

            case GREY:
                cur_table = gtable;
                break;
        }

        GFX_MarkUpdate( x, y, lx, ly );

        BYTE* outbuf = displaybuffer + x + ylookup[y];

        GFX_Shade( outbuf, lx, cur_table );
    }
}

/*************************************************************************
   GFX_LightBox()- Draws a rectangle border with light source
 *************************************************************************/
void GFX_LightBox(
    CORNER opt, // INPUT : light source
    int x, // INPUT : x position
    int y, // INPUT : y position
    int lx, // INPUT : x length
    int ly // INPUT : y length
) {
    if ( lx < 1 ) {
        return;
    }
    if ( ly < 1 ) {
        return;
    }

    switch ( opt ) {
        case UPPER_LEFT:
            GFX_HShadeLine( LIGHT, x, y, lx - 1 );
            GFX_VShadeLine( LIGHT, x, y + 1, ly - 2 );
            GFX_HShadeLine( DARK, x, y + ly - 1, lx );
            GFX_VShadeLine( DARK, x + lx - 1, y + 1, ly - 2 );
            break;

        case UPPER_RIGHT:
        default:
            GFX_HShadeLine( LIGHT, x + 1, y, lx - 1 );
            GFX_VShadeLine( LIGHT, x + lx - 1, y + 1, ly - 2 );
            GFX_HShadeLine( DARK, x, y + ly - 1, lx );
            GFX_VShadeLine( DARK, x, y, ly - 1 );
            break;

        case LOWER_LEFT:
            GFX_HShadeLine( LIGHT, x, y + ly - 1, lx - 1 );
            GFX_VShadeLine( LIGHT, x, y + 1, ly - 2 );
            GFX_HShadeLine( DARK, x, y, lx );
            GFX_VShadeLine( DARK, x + lx - 1, y + 1, ly - 1 );
            break;

        case LOWER_RIGHT:
            GFX_HShadeLine( LIGHT, x + 1, y, lx - 1 );
            GFX_VShadeLine( LIGHT, x + lx - 1, y + 1, ly - 2 );
            GFX_HShadeLine( DARK, x, y, lx );
            GFX_VShadeLine( DARK, x, y + 1, ly - 2 );
            break;
    }
}

/*************************************************************************
   GFX_ColorBox () - sets a rectangular area to color
 *************************************************************************/
void GFX_ColorBox(
    int x, // INPUT : x position
    int y, // INPUT : y position
    int lx, // INPUT : width
    int ly, // INPUT : length
    int color // INPUT : fill color ( 0 - 255 )
) {
    BYTE* outbuf;

    if ( lx < 1 ) {
        return;
    }
    if ( ly < 1 ) {
        return;
    }

    if ( GFX_ClipLines( NULL, &x, &y, &lx, &ly ) ) {
        outbuf = displaybuffer + x + ylookup[y];

        GFX_MarkUpdate( x, y, lx, ly );

        if ( color < 0 ) {
            while ( ly-- ) {
                GFX_HLine( x, y++, lx, color );
            }
        } else {
            while ( ly-- ) {
                memset( outbuf, color, (size_t) lx );
                outbuf += SCREENWIDTH;
            }
        }
    }
}

/*************************************************************************
   GFX_HLine () - plots a horizontal line in color
 *************************************************************************/
void GFX_HLine(
    int x, // INPUT : x position
    int y, // INPUT : y position
    int lx, // INPUT : width
    int color // INPUT : fill color ( 0 - 255 )
) {
    int ly = 1;

    if ( lx < 1 ) {
        return;
    }

    if ( GFX_ClipLines( NULL, &x, &y, &lx, &ly ) ) {
        BYTE* outbuf = displaybuffer + x + ylookup[y];

        GFX_MarkUpdate( x, y, lx, 1 );

        if ( color < 0 ) {
            for ( int i = 0; i < lx; i++, outbuf++ ) {
                *outbuf ^= (BYTE) ( 255 + color );
            }
        } else {
            memset( outbuf, color, (size_t) lx );
        }
    }
}

/*************************************************************************
   GFX_VLine () plots a vertical line in color
 *************************************************************************/
void GFX_VLine(
    int x, // INPUT : x position
    int y, // INPUT : y position
    int ly, // INPUT : length
    int color // INPUT : fill color ( 0 - 255 )
) {
    int lx = 1;

    if ( ly < 1 ) {
        return;
    }

    if ( GFX_ClipLines( NULL, &x, &y, &lx, &ly ) ) {
        BYTE* outbuf = displaybuffer + x + ylookup[y];

        GFX_MarkUpdate( x, y, 1, ly );

        if ( color < 0 ) {
            while ( ly-- ) {
                *outbuf ^= (BYTE) ( 255 + color );
                outbuf += SCREENWIDTH;
            }
        } else {
            while ( ly-- ) {
                *outbuf = color;
                outbuf += SCREENWIDTH;
            }
        }
    }
}

/*************************************************************************
   GFX_Line () plots a line in color ( Does no Clipping )
 *************************************************************************/
void GFX_Line(
    int x, // INPUT : x start point
    int y, // INPUT : y start point
    int x2, // INPUT : x2 end point
    int y2, // INPUT : y2 end point
    int color // INPUT : color ( 0 - 255 )
) {
    int addx = 1;
    int addy = 1;
    int delx;
    int dely;
    int maxloop;
    int err;

    delx = x2 - x;
    dely = y2 - y;

    if ( delx < 0 ) {
        delx = -delx;
        addx = -addx;
    }
    if ( dely < 0 ) {
        dely = -dely;
        addy = -addy;
    }

    if ( delx >= dely ) {
        err = -( dely >> 1 );
        maxloop = delx + 1;
    } else {
        err = ( delx >> 1 );
        maxloop = dely + 1;
    }

    if ( delx >= dely ) {
        while ( maxloop ) {
            if ( x >= 0 && x < 320 && y >= 0 && y < 200 ) {
                *( displaybuffer + x + ylookup[y] ) = (BYTE) color;
            }
            maxloop--;
            err += dely;
            x += addx;

            if ( err > 0 ) {
                y += addy;
                err -= delx;
            }
        }
    } else {
        while ( maxloop ) {
            if ( x >= 0 && x < 320 && y >= 0 && y < 200 ) {
                *( displaybuffer + x + ylookup[y] ) = (BYTE) color;
            }
            maxloop--;
            err += delx;
            y += addy;
            if ( err > 0 ) {
                x += addx;
                err -= dely;
            }
        }
    }
}

/*************************************************************************
   GFX_Rectangle () - sets a rectangular border to color
 *************************************************************************/
void GFX_Rectangle(
    int x, // INPUT : x position
    int y, // INPUT : y position
    int lx, // INPUT : width
    int ly, // INPUT : length
    int color // INPUT : fill color ( 0 - 255 )
) {
    if ( ly < 1 ) {
        return;
    }
    if ( lx < 1 ) {
        return;
    }

    GFX_HLine( x, y, lx, color );
    GFX_HLine( x, y + ly - 1, lx, color );
    GFX_VLine( x, y + 1, ly - 2, color );
    GFX_VLine( x + lx - 1, y + 1, ly - 2, color );
}

/*************************************************************************
   GFX_MarkUpdate () Marks an area to be draw with GFX_DrawScreen()
 *************************************************************************/
void GFX_MarkUpdate(
    int x, // INPUT : x position
    int y, // INPUT : y position
    int lx, // INPUT : x length
    int ly // INPUT : y length
) {
    int x2 = x + lx - 1;
    int y2 = y + ly - 1;
    int ud_x2 = ud_x + ud_lx - 1;
    int ud_y2 = ud_y + ud_ly - 1;

    if ( update_start ) {
        if ( x < ud_x ) {
            ud_x = x;
        }

        if ( y < ud_y ) {
            ud_y = y;
        }

        if ( x2 > ud_x2 ) {
            ud_x2 = x2;
        }
        if ( y2 > ud_y2 ) {
            ud_y2 = y2;
        }
    } else {
        ud_x = x;
        ud_y = y;
        ud_x2 = x + lx - 1;
        ud_y2 = y + ly - 1;
        if ( ( lx + ly ) > 0 ) {
            update_start = true;
        }
    }

    if ( ud_x & 3 ) {
        ud_x -= ( ud_x & 3 );
    }

    if ( ud_x < 0 ) {
        ud_x = 0;
    }

    if ( ud_y < 0 ) {
        ud_y = 0;
    }

    ud_lx = ud_x2 - ud_x + 1;
    ud_ly = ud_y2 - ud_y + 1;

    if ( ud_lx & 3 ) {
        ud_lx += ( 4 - ( ud_lx & 3 ) );
    }

    if ( ud_lx + ud_x > SCREENWIDTH ) {
        ud_lx = SCREENWIDTH - ud_x;
    }
    if ( ud_ly + ud_y > SCREENHEIGHT ) {
        ud_ly = SCREENHEIGHT - ud_y;
    }
}

/*************************************************************************
   GFX_ForceUpdate () Marks an area to be draw with GFX_DrawScreen()
 *************************************************************************/
void GFX_ForceUpdate(
    int x, // INPUT : x position
    int y, // INPUT : y position
    int lx, // INPUT : x length
    int ly // INPUT : y length
) {
    ud_x = o_ud_x = x;
    ud_y = o_ud_y = y;
    ud_lx = o_ud_lx = lx;
    ud_ly = o_ud_ly = ly;
}

/***************************************************************************
   GFX_SetFrameHook () sets function to call before every screen update
 ***************************************************************************/
void GFX_SetFrameHook( void ( *func )( void ( * )( void ) ) // INPUT : pointer to function
) {
    framehook = func;
}

/***************************************************************************
 GFX_Delay () - Delay for ( count ) of screen frames ( sec/70 )
 ***************************************************************************/
void GFX_Delay(
    int count // INPUT : wait # of frame ticks
) {
    static int hold;
    int i;

    for ( i = 0; i < count; i++ ) {
        hold = framecount;
        while ( framecount == hold && !gfxdebug )
            ;
    }
}

/***************************************************************************
   GFX_WaitUpdate () - Updates screen at specified frame rate
 ***************************************************************************/
void GFX_WaitUpdate(
    int count // INPUT : frame rate ( MAX = 70 )
) {
    static int hold = 0;
    int i;

    if ( count > 70 ) {
        count = 70;
    } else if ( count < 1 ) {
        count = 1;
    }

    count = 70 / count;

    GFX_MarkUpdate( o_ud_x, o_ud_y, o_ud_lx, o_ud_ly );

    for ( i = 0; i < count; i++ ) {
        while ( framecount == hold && !gfxdebug )
            ;
        hold = framecount;
    }

    if ( update_start ) {
        if ( framehook ) {
            framehook( (void*) GFX_DisplayScreen );
        } else {
            GFX_DisplayScreen();
        }
    }

    o_ud_x = ud_x;
    o_ud_y = ud_y;
    o_ud_lx = ud_lx;
    o_ud_ly = ud_ly;
}

/***************************************************************************
   GFX_DisplayUpdate () - Copys Marked areas to display
 ***************************************************************************/
void GFX_DisplayUpdate( void ) {
    static int hold = 0;

    while ( framecount == hold && !gfxdebug )
        ;

    GFX_MarkUpdate( o_ud_x, o_ud_y, o_ud_lx, o_ud_ly );

    if ( update_start ) {
        if ( framehook ) {
            framehook( (void*) GFX_DisplayScreen );
        } else {
            GFX_DisplayScreen();
        }
    }

    o_ud_x = ud_x;
    o_ud_y = ud_y;
    o_ud_lx = ud_lx;
    o_ud_ly = ud_ly;

    hold = framecount;
}

/***************************************************************************
   GFX_PutImage() - places image in displaybuffer and performs cliping
 ***************************************************************************/
void GFX_PutImage(
    BYTE* image, // INPUT : image data
    int x, // INPUT : x position
    int y, // INPUT : y position
    bool see_thru // INPUT : true = masked, false = put block
) {
    GFX_PIC* h = (GFX_PIC*) image;

    gfx_lx = h->width;
    gfx_ly = h->height;

    if ( h->type == GSPRITE ) {
        GFX_PutSprite( image, x, y );
    } else {
        image += sizeof( GFX_PIC );

        if ( GFX_ClipLines( &image, &x, &y, &gfx_lx, &gfx_ly ) ) {
            GFX_MarkUpdate( x, y, gfx_lx, gfx_ly );

            gfx_xp = x;
            gfx_yp = y;

            gfx_inmem = image;
            gfx_imga = h->width;

            if ( !see_thru ) {
                gfx_imga -= gfx_lx;
                GFX_PutPic();
            } else {
                GFX_PutMaskPic();
            }
        }
    }
}

/***************************************************************************
   GFX_PutSprite () -Puts a Sprite into display buffer
 ***************************************************************************/
void GFX_PutSprite(
    BYTE* inmem, // INPUT : inmem
    int x, // INPUT : x pos
    int y // INPUT : y pos
) {
    GFX_PIC* h = (GFX_PIC*) inmem;
    GFX_SPRITE* ah;
    BYTE* dest;
    BYTE* outline;
    int rval;
    int ox = x;
    int oy = y;
    int lx = h->width;
    int ly = h->height;

    rval = GFX_ClipLines( NULL, &ox, &oy, &lx, &ly );
    if ( rval == 0 ) {
        return;
    }

    inmem += sizeof( GFX_PIC );

    switch ( rval ) {

        case 1:
            dest = displaybuffer + ox + ylookup[oy];
            GFX_DrawSprite( dest, inmem );
            break;

        case 2:
            ah = (GFX_SPRITE*) inmem;

            while ( ah->offset != EMPTY ) {
                inmem += sizeof( GFX_SPRITE );

                ox = ah->x + x;
                oy = ah->y + y;

                if ( oy > SCREENHEIGHT ) {
                    break;
                }

                lx = ah->length;
                ly = 1;

                outline = inmem;

                if ( GFX_ClipLines( &outline, &ox, &oy, &lx, &ly ) ) {
                    memcpy( displaybuffer + ox + ylookup[oy], outline, lx );
                }

                inmem += ah->length;

                ah = (GFX_SPRITE*) inmem;
            }
            break;
        default: // never happens
            break;
    }
}

/***************************************************************************
   GFX_OverlayImage() - places image in displaybuffer and performs cliping
 ***************************************************************************/
void GFX_OverlayImage(
    BYTE* baseimage, // INPUT : base image data
    BYTE* overimage, // INPUT : overlay image data
    int x, // INPUT : x position
    int y // INPUT : y position
) {
    GFX_PIC* bh = (GFX_PIC*) baseimage;
    GFX_PIC* oh = (GFX_PIC*) overimage;
    int x2 = x + oh->width - 1;
    int y2 = y + oh->height - 1;

    if ( x >= 0 && y >= 0 && x2 < bh->width && y2 < bh->height ) {
        baseimage += sizeof( GFX_PIC );
        baseimage += ( x + ( y * bh->width ) );

        overimage += sizeof( GFX_PIC );

        int addnum = bh->width - oh->width;

        for ( int j = 0; j < oh->height; j++ ) {
            for ( int i = 0; i < oh->width; i++, baseimage++, overimage++ ) {
                if ( i != 255 ) {
                    *baseimage = *overimage;
                }
            }
            baseimage += addnum;
        }
    }
}

/***************************************************************************
   GFX_StrPixelLen() - Calculates the length of a GFX string
 ***************************************************************************/
int // RETURNS : pixel length
GFX_StrPixelLen(
    void* infont, // INPUT : pointer to current font
    unsigned char* instr, // INPUT : pointer to string
    size_t maxloop // INPUT : length of string
) {
    int outlen = 0;

    for ( int i = 0; i < maxloop; i++ ) {
        outlen += ( (FONT*) infont )->width[instr[i]];
        outlen += fontspacing;
    }

    return outlen;
}

/*--------------------------------------------------------------------------
   GFX_PutChar () - Draws charater to displaybuffer and clips
 --------------------------------------------------------------------------*/
PRIVATE int GFX_PutChar(
    int x, // INPUT : x position
    int y, // INPUT : y position
    BYTE inchar, // INPUT : char to print
    FONT* font, // INPUT : pointer to font
    int basecolor // INPUT : font base color
) {
    BYTE* source = (BYTE*) font + sizeof( FONT );
    int lx = font->width[inchar];
    int ly = font->height;
    BYTE* cdata = source + font->charofs[inchar];
    int addx = lx;

    if ( GFX_ClipLines( &cdata, &x, &y, &lx, &ly ) ) {
        BYTE* dest = displaybuffer + x + ylookup[y];

        GFX_MarkUpdate( x, y, lx, ly );

        addx = addx - lx;

        GFX_DrawChar( dest, cdata, lx, ly, addx, basecolor );

        return lx;
    }

    return 0;
}

/***************************************************************************
   GFX_Print () - prints a string using specified font with basecolor
 ***************************************************************************/
int // RETURN: length of print
GFX_Print(
    int x, // INPUT : x position
    int y, // INPUT : y position
    char* str, // INPUT : string to print
    void* infont, // INPUT : pointer to font
    int basecolor // INPUT : basecolor of font
) {
    FONT* font = infont;
    int lx = 0;
    BYTE ch;

    basecolor--;

    if ( strlen( str ) ) {
        while ( ( ch = *str++ ) != 0 ) {
            if ( font->charofs[ch] == (short) EMPTY ) {
                continue;
            }
            int cwidth = GFX_PutChar( x, y, ch, font, basecolor );
            lx += cwidth + fontspacing;
            x += font->width[ch] + fontspacing;
        }
    }

    return lx;
}

/***************************************************************************
   GFX_3D_SetView() Sets user view in 3d space
 ***************************************************************************/
void GFX_3D_SetView(
    int x, // INPUT : x position
    int y, // INPUT : y position
    int z // INPUT : z position
) {
    G3D_viewx = x;
    G3D_viewy = y;
    G3D_viewz = z;
}

/*--------------------------------------------------------------------------
   GFX_3DPoint () plots a points in 3D space
 --------------------------------------------------------------------------*/
void GFX_3DPoint( void ) {
    G3D_x -= G3D_viewx;
    G3D_y -= G3D_viewy;
    G3D_z -= G3D_viewz;

    G3D_screenx = ( ( G3D_DIST << 11 ) * G3D_x / G3D_z ) >> 11;
    G3D_screeny = ( ( G3D_DIST << 11 ) * G3D_y / G3D_z ) >> 11;

    G3D_screenx += G3D_viewx;
    G3D_screeny += G3D_viewy;
}

// gfxapi_a

void GFX_Shade( uint8_t* dst, int maxlen, const uint8_t* shading_table ) {
    for ( int i = 0; i < maxlen; i++ ) {
        *dst = shading_table[*dst];
        dst++;
    }
}

void GFX_ShadeSprite( uint8_t* dst, const uint8_t* src, const uint8_t* shading_table ) {
    int offset;
    while ( ( offset = ( (GFX_SPRITE*) src )->offset ) != -1 ) {
        const int length = ( (GFX_SPRITE*) src )->length;
        src += sizeof( GFX_SPRITE );

        for ( int i = 0; i < length; i++ ) {
            dst[offset] = shading_table[dst[offset]];
            offset++;
        }
        src += length;
    }
}

void GFX_DrawSprite( uint8_t* dst, const uint8_t* src ) {
    int offset;
    while ( ( offset = ( (GFX_SPRITE*) src )->offset ) != -1 ) {
        const int length = ( (GFX_SPRITE*) src )->length;
        src += sizeof( GFX_SPRITE );
        memcpy( dst + offset, src, length );
        src += length;
    }
}

void GFX_DrawChar( uint8_t* dst, const uint8_t* src, int lx, int ly, int add_x_src, uint8_t color ) {
    int add_x_dst = SCREENWIDTH - lx;

    for ( int y = 0; y < ly; y++ ) {
        for ( int x = 0; x < lx; x++ ) {
            uint8_t pixel = *src++;
            if ( pixel != 0 ) {
                *dst = pixel + color;
            }
            dst++;
        }
        src += add_x_src;
        dst += add_x_dst;
    }
}

inline uint8_t* GFX_GetDst() {
    return displaybuffer + gfx_xp + gfx_yp * SCREENWIDTH;
}

void GFX_PutPic( void ) {
    uint8_t* src = gfx_inmem;
    uint8_t* dst = GFX_GetDst();

    if ( gfx_lx < 1 ) {
        return;
    }

    for ( int y = 0; y < gfx_ly; y++ ) {
        memcpy( dst, src, gfx_lx );
        dst += SCREENWIDTH;
        src += gfx_lx + gfx_imga; // ca. h->width
    }
}

void GFX_PutMaskPic( void ) {
    uint8_t* src = gfx_inmem;
    uint8_t* dst = GFX_GetDst();

    if ( gfx_lx < 1 ) {
        return;
    }

    for ( int y = 0; y < gfx_ly; y++ ) {
        for ( int x = 0; x < gfx_lx; x++ ) {
            int al = src[x];
            if ( al != 0 ) {
                dst[x] = al;
            }
        }

        dst += SCREENWIDTH;
        src += gfx_imga; // no gfx_lx added here?!
    }
}

void GFX_DisplayScreen( void ) {
    int offset = ud_x + ud_y * SCREENWIDTH;
    uint8_t* src = displaybuffer + offset;
    uint8_t* dst = displayscreen + offset;

    if ( ud_lx < 1 ) {
        return;
    }

    if ( ud_lx == SCREENWIDTH ) {
        memcpy( dst, src, SCREENWIDTH * ud_ly );
    } else {
        for ( int y = 0; y < ud_ly; y++ ) {
            memcpy( dst, src, ud_lx );
            dst += SCREENWIDTH;
            src += SCREENWIDTH;
        }
    }

    update_start = false;
}
