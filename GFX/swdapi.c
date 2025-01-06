#include <ctype.h>
#include <dos.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dpmiapi.h"
#include "exitapi.h"
#include "gfxapi.h"
#include "glbapi.h"
#include "kbdapi.h"
#include "ptrapi.h"
#include "swdapi.h"

PUBLIC BOOL usekb_flag = FALSE;

PRIVATE BOOL kbactive = FALSE;
PRIVATE INT prev_window = EMPTY;
PRIVATE INT master_window = EMPTY;
PRIVATE INT active_window = EMPTY;
INT active_field = EMPTY;
PRIVATE SFIELD* lastfld = NULL;
PRIVATE BOOL highlight_flag = FALSE;
PRIVATE SWD_WIN g_wins[MAX_WINDOWS];
PRIVATE void ( *winfuncs[MAX_WINDOWS] )( SWD_DLG* );
PRIVATE void ( *fldfuncs[15] )( SWIN*, SFIELD* );
PRIVATE BYTE* movebuffer = NULL;
PRIVATE void ( *viewdraw )( void ) = (void( * )) 0;
PRIVATE ACT cur_act = S_IDLE;
PRIVATE CMD cur_cmd = C_IDLE;
PRIVATE INT obj_x;
PRIVATE INT obj_y;
PRIVATE INT obj_width;
PRIVATE INT obj_height;
PRIVATE BOOL clearscreenflag = TRUE;
PUBLIC BOOL g_button_flag = TRUE;
PRIVATE INT old_field = -99;
PRIVATE INT old_win = -99;
PRIVATE INT g_key;
PRIVATE INT g_ascii;

// == STUFF FOR TEXT SCRIPT ==============================

#define MAX_TEXT_LEN 81
#define MAX_ARGS     5

typedef enum { T_NONE, T_IMAGE, T_COLOR, T_TEXT_POS, T_RIGHT, T_DOWN, T_LASTCMD } TEXTCMD;

PRIVATE BYTE tcmds[T_LASTCMD - 1][14] = {
    "TEXT_IMAGE", // GLBNAME, X, Y
    "TEXT_COLOR", // COLOR #
    "TEXT_POS", // X, Y
    "TEXT_RIGHT", // X add
    "TEXT_DOWN" // Y add
};

PRIVATE BYTE textfill[MAX_TEXT_LEN];
PRIVATE INT textcolor;
PRIVATE BOOL textcmd_flag;
PRIVATE INT textdraw_x;
PRIVATE INT textdraw_y;
PRIVATE INT textcmd_x;
PRIVATE INT textcmd_y;
PRIVATE INT textcmd_x2;
PRIVATE INT textcmd_y2;
PRIVATE INT textcmd_line;

/*------------------------------------------------------------------------
   SWD_ShadeButton (
  ------------------------------------------------------------------------*/
PRIVATE void SWD_ShadeButton(
    BUTTON opt, // INPUT : NORMAL/UP/DOWN
    INT x, // INPUT : x position
    INT y, // INPUT : y position
    INT lx, // INPUT : x width
    INT ly // INPUT : y width
) {
    switch ( opt ) {
        case DOWN:
            GFX_HShadeLine( DARK, x, y, lx );
            GFX_VShadeLine( DARK, x + lx - 1, y + 1, ly - 1 );
            break;

        case UP:
            GFX_HShadeLine( LIGHT, x + 2, y, lx - 2 );
            GFX_VShadeLine( LIGHT, x + lx - 1, y + 1, ly - 3 );

        default:
        case NORMAL:
            GFX_HShadeLine( LIGHT, x + 1, y, lx - 1 );
            GFX_VShadeLine( LIGHT, x + lx - 1, y + 1, ly - 2 );
            GFX_HShadeLine( DARK, x, y + ly - 1, lx );
            GFX_VShadeLine( DARK, x, y, ly - 1 );
            break;
    }
}

INT SWD_GetLine( BYTE* inmem ) {
    static BYTE* text;
    BYTE temp[MAX_TEXT_LEN + 1];
    CHAR* cbrks = "\n\v\r \t,;\b";
    INT curpos = 0;
    BYTE* cmd;
    int i;
    INT x, y;
    DWORD item;
    BYTE* pic;
    GFX_PIC* h;

    if ( inmem != NULL ) {
        text = inmem;
    }

    textcmd_flag = FALSE;
    curpos = 0;

    memcpy( temp, text, MAX_TEXT_LEN );

    cmd = strtok( temp, cbrks );

    for ( i = 0; i < T_LASTCMD - 1; i++ ) {
        if ( strcmp( cmd, tcmds[i] ) == 0 ) {
            textcmd_flag = TRUE;

            while ( *( text + curpos ) > 31 ) {
                curpos++;
            }

            while ( *( text + curpos ) <= 31 ) {
                curpos++;
            }

            text += curpos;

            cmd = strtok( NULL, cbrks );

            switch ( i + 1 ) {
                default:
                    break;

                case T_IMAGE:
                    item = GLB_GetItemID( cmd );
                    if ( item == EMPTY ) {
                        break;
                    }
                    pic = GLB_GetItem( item );
                    cmd = strtok( NULL, cbrks );

                    h = (GFX_PIC*) pic;

                    if ( cmd == NULL ) {
                        x = textdraw_x;
                        y = textdraw_y;
                    } else {
                        x = atoi( cmd );
                        cmd = strtok( NULL, cbrks );
                        y = atoi( cmd );

                        x += textcmd_x;
                        y += textcmd_y;
                    }

                    if ( x > textcmd_x2 ) {
                        break;
                    }
                    if ( y > textcmd_y2 ) {
                        break;
                    }

                    textdraw_x += h->width + 1;
                    textdraw_y = y;

                    GFX_PutImage( pic, x, y, FALSE );
                    GLB_FreeItem( item );
                    break;

                case T_COLOR:
                    x = atoi( cmd );
                    if ( x >= 0 && x < 256 ) {
                        textcolor = x;
                    }
                    break;

                case T_TEXT_POS:
                    x = atoi( cmd );
                    cmd = strtok( NULL, cbrks );
                    y = atoi( cmd );

                    if ( x > textcmd_x2 ) {
                        break;
                    }
                    if ( y > textcmd_y2 ) {
                        break;
                    }

                    textdraw_x = x + textcmd_x;
                    textdraw_y = y + textcmd_y;
                    textcmd_line = 0;
                    break;

                case T_RIGHT:
                    x = atoi( cmd );
                    if ( x > textcmd_x2 || cmd == NULL ) {
                        break;
                    }
                    textdraw_x += x;
                    break;

                case T_DOWN:
                    y = atoi( cmd );
                    if ( y > textcmd_y2 || cmd == NULL ) {
                        break;
                    }
                    textdraw_y += y;
                    break;
            }
            return curpos;
        }
    }

    while ( *( text + curpos ) > 31 ) {
        curpos++;
    }

    memcpy( textfill, text, curpos );
    textfill[curpos] = '\0';

    while ( *( text + curpos ) <= 31 ) {
        curpos++;
    }

    text += curpos;

    return curpos;
}

/***************************************************************************
SWD_FillText () - Fills Text from GLB intro an AREA
 ***************************************************************************/
void SWD_FillText(
    FONT* font, // INPUT : pointer to FONT
    DWORD item, // INPUT : GLB text Item
    INT color, // INPUT : field color
    INT x, // INPUT : x position
    INT y, // INPUT : y position
    INT lx, // INPUT : width of field
    INT ly // INPUT : height of field
) {
    BYTE* text;
    INT len;
    INT sizerec;

    if ( item == EMPTY ) {
        return;
    }

    text = GLB_LockItem( item );

    if ( text == NULL ) {
        return;
    }

    textdraw_x = textcmd_x = x;
    textdraw_y = textcmd_y = y;
    textcmd_x2 = x + lx - 1;
    textcmd_y2 = y + ly - 1;

    textcmd_line = 0;

    textcolor = color;

    sizerec = GLB_ItemSize( item );

    len = SWD_GetLine( text );

    for ( ;; ) {
        if ( !textcmd_flag ) {

            GFX_Print( textdraw_x, textdraw_y, textfill, font, textcolor );
            textdraw_y += font->height + 3;
        }

        if ( len >= sizerec ) {
            break;
        }

        len += SWD_GetLine( NULL );
    }

    GLB_FreeItem( item );
}

/*------------------------------------------------------------------------
  SWD_PutField() - puts a field in displaybuffer
  ------------------------------------------------------------------------*/
PRIVATE void SWD_PutField(
    SWIN* curwin, // INPUT : pointer to window data
    SFIELD* curfld // INPUT : pointer to field data
) {
    void* fld_font = GLB_GetItem( curfld->fontid );
    CHAR* fld_text = ( (CHAR*) curfld ) + curfld->txtoff;
    INT fontheight = ( (FONT*) fld_font )->height;
    INT fld_x = curfld->x + curwin->x;
    INT fld_y = curfld->y + curwin->y;
    INT curpos;
    BYTE* pic;
    INT rval;
    INT i;
    INT text_x;
    INT text_y;
    GFX_PIC* h;
    BOOL draw_style = FALSE;
    BOOL draw_text = FALSE;

    rval = GFX_StrPixelLen( fld_font, fld_text, strlen( fld_text ) );
    text_x = ( ( curfld->lx - rval ) >> 1 ) + fld_x;
    text_y = ( ( curfld->ly - ( (FONT*) fld_font )->height ) >> 1 ) + fld_y;

    if ( curfld->bstatus == DOWN && curfld->opt != FLD_DRAGBAR ) {
        if ( text_x > 0 ) {
            text_x--;
        }
        text_y++;
    }

    if ( curfld->saveflag && curfld->sptr ) {
        GFX_PutImage( curfld->sptr, fld_x, fld_y, FALSE );
    }

    if ( curfld->picflag != FILL && curfld->picflag != INVISABLE ) {
        if ( curfld->item == EMPTY ) {
            goto PutField_Exit;
        }

        if ( curfld->picflag == SEE_THRU ) {
            draw_style = TRUE;
        }

        switch ( curfld->opt ) {
            case FLD_BUTTON:
                pic = GLB_GetItem( curfld->item );

                if ( curfld->picflag == TEXTURE ) {
                    GFX_PutTexture( pic, fld_x, fld_y, curfld->lx, curfld->ly );
                    SWD_ShadeButton( curfld->bstatus, fld_x, fld_y, curfld->lx, curfld->ly );
                } else {
                    GFX_PutImage( pic, fld_x, fld_y, draw_style );
                }

                draw_text = TRUE;
                break;

            case FLD_DRAGBAR:
                pic = GLB_GetItem( curfld->item );
                if ( curfld->picflag == TEXTURE ) {
                    GFX_PutTexture( pic, fld_x, fld_y, curfld->lx, curfld->ly );
                    GFX_LightBox( UPPER_RIGHT, fld_x, fld_y, curfld->lx, curfld->ly );
                } else {
                    GFX_PutImage( pic, fld_x, fld_y, draw_style );
                }

                if ( g_wins[active_window].win != curwin ) {
                    GFX_ShadeArea( GREY, fld_x, fld_y, curfld->lx, curfld->ly );
                }

                draw_text = TRUE;
                break;

            case FLD_ICON:
                pic = GLB_GetItem( curfld->item );

                if ( pic == NULL ) {
                    break;
                }

                if ( curfld->picflag == TEXTURE ) {
                    GFX_PutTexture( pic, fld_x, fld_y, curfld->lx, curfld->ly );
                    goto PutField_Exit;
                }

                h = (GFX_PIC*) pic;
                if ( curfld->lx < h->width || curfld->ly < h->height ) {
                    GFX_ScalePic( pic, fld_x, fld_y, curfld->lx, curfld->ly, FALSE );
                } else {
                    GFX_PutImage( pic, fld_x, fld_y, draw_style );
                }
                break;

            case FLD_CLOSE:
            case FLD_MARK:
                pic = GLB_GetItem( curfld->item );
                GFX_PutImage( pic, fld_x, fld_y, draw_style );
                break;

            case FLD_TEXT:
                SWD_FillText( fld_font, curfld->item, curfld->fontbasecolor, fld_x, fld_y, curfld->lx, curfld->ly );
                break;

            default:
                break;
        }

        if ( curfld->bstatus == NORMAL ) {
            goto PutField_Exit;
        }

        goto PutField_ShadeExit;
    }

    switch ( curfld->opt ) {
        case FLD_OFF:
            break;

        case FLD_TEXT:
            if ( curfld->maxchars ) {
                GFX_Print( fld_x, fld_y, fld_text, fld_font, curfld->fontbasecolor );
            }
            break;

        case FLD_BUTTON:
            if ( curfld->picflag != INVISABLE ) {
                GFX_ColorBox( fld_x, fld_y, curfld->lx, curfld->ly, curfld->color );
                SWD_ShadeButton( curfld->bstatus, fld_x, fld_y, curfld->lx, curfld->ly );
                draw_text = TRUE;
            } else {
                GFX_Print( text_x, text_y, fld_text, fld_font, curfld->fontbasecolor );
            }
            break;

        case FLD_INPUT:
            if ( curfld->bstatus == NORMAL ) {
                GFX_ColorBox( fld_x, fld_y, curfld->lx, curfld->ly, curfld->color );
            } else {
                GFX_ColorBox( fld_x, fld_y, curfld->lx, curfld->ly, curfld->lite );
            }

            if ( curfld->maxchars ) {
                GFX_Print( fld_x + 1, text_y, fld_text, fld_font, curfld->fontbasecolor );
            }

            if ( curfld->bstatus != NORMAL ) {
                curpos = strlen( fld_text );
                rval = GFX_StrPixelLen( fld_font, fld_text, curpos );

                text_x = fld_x + 1 + rval;

                if ( rval + 2 < curfld->lx ) {
                    GFX_VLine( text_x, fld_y + 1, fontheight - 1, curfld->fontbasecolor );
                }
            }
            break;

        case FLD_MARK:
            GFX_ColorBox( fld_x, fld_y, curfld->lx, curfld->ly, curfld->color );
            GFX_LightBox( UPPER_RIGHT, fld_x, fld_y, curfld->lx, curfld->ly );
            GFX_ColorBox( fld_x + 2, fld_y + 2, curfld->lx - 4, curfld->ly - 4, 0 );
            text_x = fld_x + 3;
            text_y = fld_y + 3;
            if ( curfld->mark ) {
                GFX_ColorBox( text_x, text_y, curfld->lx - 6, curfld->ly - 6, curfld->lite );
                SWD_ShadeButton( curfld->bstatus, text_x, text_y, curfld->lx - 6, curfld->ly - 6 );
            } else {
                GFX_ColorBox( text_x, text_y, curfld->lx - 6, curfld->ly - 6, 0 );
            }
            break;

        case FLD_CLOSE:
            if ( curfld->picflag == INVISABLE ) {
                goto PutField_Exit;
            }
            GFX_ColorBox( fld_x, fld_y, curfld->lx, curfld->ly, curfld->color );
            GFX_LightBox( UPPER_RIGHT, fld_x, fld_y, curfld->lx, curfld->ly );
            GFX_ColorBox( fld_x + 2, fld_y + 2, curfld->lx - 4, curfld->ly - 4, curfld->lite );
            text_x = fld_x + 3;
            text_y = fld_y + 3;
            GFX_ColorBox( text_x, text_y, curfld->lx - 6, curfld->ly - 6, curfld->lite );
            SWD_ShadeButton( curfld->bstatus, text_x, text_y, curfld->lx - 6, curfld->ly - 6 );
            break;

        case FLD_DRAGBAR:
            if ( curfld->picflag != INVISABLE ) {
                GFX_ColorBox( fld_x, fld_y, curfld->lx, curfld->ly, curfld->color );
            }

            if ( curfld->maxchars > 1 ) {
                GFX_Print( text_x, text_y, fld_text, fld_font, curfld->fontbasecolor );
            }

            if ( curfld->picflag == INVISABLE ) {
                break;
            }

            if ( g_wins[active_window].win != curwin ) {
                GFX_ShadeArea( DARK, fld_x, fld_y, curfld->lx, curfld->ly );

                if ( curfld->color ) {
                    for ( i = 0; i < curfld->ly; i += 2 ) {
                        GFX_HShadeLine( DARK, fld_x, fld_y + i, curfld->lx );
                    }
                }
            }
            break;

        case FLD_BUMPIN:
            if ( curfld->color ) {
                GFX_ShadeArea( DARK, fld_x + 1, fld_y, curfld->lx - 1, curfld->ly - 1 );
            }
            GFX_LightBox( LOWER_LEFT, fld_x, fld_y, curfld->lx, curfld->ly );
            if ( !curfld->color ) {
                GFX_ColorBox( fld_x + 1, fld_y + 1, curfld->lx - 2, curfld->ly - 2, curfld->color );
            }
            break;

        case FLD_BUMPOUT:
            GFX_ShadeArea( LIGHT, fld_x + 1, fld_y, curfld->lx - 1, curfld->ly - 1 );
            GFX_LightBox( UPPER_RIGHT, fld_x, fld_y, curfld->lx, curfld->ly );
            if ( !curfld->color ) {
                GFX_ColorBox( fld_x + 1, fld_y + 1, curfld->lx - 2, curfld->ly - 2, curfld->color );
            }
            break;

        default:
            break;
    }

PutField_ShadeExit:

    if ( curfld->bstatus != NORMAL && curfld->opt != FLD_INPUT ) {
        if ( curfld->picflag == PICTURE ) {
            h = (GFX_PIC*) GLB_GetItem( curfld->item );
        } else {
            h = NULL;
        }

        if ( curfld->bstatus == DOWN ) {
            if ( h->type == GSPRITE && h ) {
                GFX_ShadeShape( DARK, (BYTE*) h, fld_x, fld_y );
            } else {
                GFX_ShadeArea( DARK, fld_x, fld_y, curfld->lx, curfld->ly );
            }
        } else if ( curfld->bstatus == UP ) {
            if ( h->type == GSPRITE && h ) {
                GFX_ShadeShape( LIGHT, (BYTE*) h, fld_x, fld_y );
            } else {
                GFX_ShadeArea( LIGHT, fld_x, fld_y, curfld->lx, curfld->ly );
            }
        }
    }

PutField_Exit:

    if ( draw_text && curfld->maxchars > 1 ) {
        GFX_Print( text_x, text_y, fld_text, fld_font, curfld->fontbasecolor );
    }
}

/*------------------------------------------------------------------------
  SWD_DoButton() - processes all buttons from SWD_Dialog
  ------------------------------------------------------------------------*/
PRIVATE void SWD_DoButton(
    SWIN* curwin, // INPUT : pointer to current window
    SFIELD* curfld // INPUT : pointer to current field
) {
    curfld = curfld; // unused, but required for signature compatibility

    if ( !g_button_flag ) {
        return;
    }

    switch ( g_key ) {
        case SC_TAB:
            if ( KBD_Key( SC_ALT ) ) {
                cur_act = S_WIN_COMMAND;
                cur_cmd = W_NEXT;
                while ( (volatile) KBD_Key( SC_TAB ) ) {
                }
            } else {
                cur_act = S_FLD_COMMAND;

                if ( KBD_ISCAPS ) {
                    cur_cmd = F_PREV;
                } else {
                    cur_cmd = F_NEXT;
                }
            }
            break;

        case SC_ENTER:
            cur_act = S_FLD_COMMAND;
            cur_cmd = F_SELECT;
            break;

        case SC_DOWN:
            if ( !curwin->arrowflag ) {
                break;
            }
            cur_act = S_FLD_COMMAND;
            cur_cmd = F_DOWN;
            break;

        case SC_UP:
            if ( !curwin->arrowflag ) {
                break;
            }
            cur_act = S_FLD_COMMAND;
            cur_cmd = F_UP;
            break;

        case SC_RIGHT:
            if ( !curwin->arrowflag ) {
                break;
            }
            cur_act = S_FLD_COMMAND;
            cur_cmd = F_RIGHT;
            break;

        case SC_LEFT:
            if ( !curwin->arrowflag ) {
                break;
            }
            cur_act = S_FLD_COMMAND;
            cur_cmd = F_LEFT;
            break;

        default:
            break;
    }
}

/*------------------------------------------------------------------------
  SWD_FieldInput() - Field Input function for SWD_Dialog
  ------------------------------------------------------------------------*/
PRIVATE void SWD_FieldInput(
    SWIN* curwin, // INPUT : pointer to current window
    SFIELD* curfld // INPUT : pointer to current field
) {
    PRIVATE INT curpos = 0;
    void* fld_font = GLB_GetItem( curfld->fontid );
    BOOL flag = FALSE;
    CHAR* wrkbuf = ( (CHAR*) curfld ) + curfld->txtoff;
    INT fontheight;
    INT len;

    fontheight = ( (FONT*) fld_font )->height;

    curpos = strlen( wrkbuf );

    switch ( g_key ) {
        case SC_TAB:
            if ( KBD_Key( SC_ALT ) ) {
                while ( (volatile) KBD_Key( SC_TAB ) ) {
                }
                cur_act = S_WIN_COMMAND;
                cur_cmd = W_NEXT;
            } else {
                cur_act = S_FLD_COMMAND;

                if ( KBD_ISCAPS ) {
                    cur_cmd = F_PREV;
                } else {
                    cur_cmd = F_NEXT;
                }
            }
            break;

        case SC_ENTER:
            cur_act = S_FLD_COMMAND;
            cur_cmd = F_SELECT;
            break;

        case SC_DOWN:
            if ( !curwin->arrowflag ) {
                break;
            }
            cur_act = S_FLD_COMMAND;
            cur_cmd = F_DOWN;
            break;

        case SC_UP:
            if ( !curwin->arrowflag ) {
                break;
            }
            cur_act = S_FLD_COMMAND;
            cur_cmd = F_UP;
            break;

        case SC_RIGHT:
            if ( !curwin->arrowflag ) {
                break;
            }
            cur_act = S_FLD_COMMAND;
            cur_cmd = F_RIGHT;
            break;

        case SC_LEFT:
            if ( !curwin->arrowflag ) {
                break;
            }
            cur_act = S_FLD_COMMAND;
            cur_cmd = F_LEFT;
            break;

        case SC_BACKSPACE:
            flag = TRUE;
            if ( curpos > 0 ) {
                curpos--;
            }
            wrkbuf[curpos] = 0;
            break;

        default:
            if ( KBD_Key( SC_Y ) && KBD_Key( SC_CTRL ) ) {
                curpos = 0;
                wrkbuf[curpos] = 0;
                flag = TRUE;
                break;
            }

            if ( KBD_Key( SC_ALT ) || KBD_Key( SC_CTRL ) ) {
                break;
            }

            if ( g_key > 0 && curpos < curfld->maxchars - 1 ) {
                if ( g_ascii > 31 && g_ascii < 127 ) {
                    switch ( curfld->input_opt ) {
                        case I_NORM:
                            wrkbuf[curpos] = (CHAR) g_ascii;
                            break;

                        case I_TOUPPER:
                            wrkbuf[curpos] = (CHAR) toupper( g_ascii );
                            break;

                        case I_NUMERIC:
                            if ( isdigit( g_ascii ) || g_ascii == '-' ) {
                                wrkbuf[curpos] = (CHAR) g_ascii;
                            } else {
                                wrkbuf[curpos] = (CHAR) 0;
                            }
                            break;
                    }
                } else {
                    wrkbuf[curpos] = (CHAR) 0;
                }

                len = GFX_StrPixelLen( fld_font, wrkbuf, (size_t) curpos + 1 );

                if ( len >= curfld->lx ) {
                    curpos--;
                }

                wrkbuf[curpos + 1] = 0;

                flag = TRUE;
            }
            break;
    }

    if ( flag ) {
        SWD_PutField( curwin, curfld );
        cur_act = S_UPDATE;
        cur_cmd = C_IDLE;
    }
}

/*------------------------------------------------------------------------
   SWD_GetObjAreaInfo () - looks for a objarea then sets obj_xx variables
  ------------------------------------------------------------------------*/
PRIVATE void SWD_GetObjAreaInfo(
    INT handle // INPUT: handle of window
) {
    SWIN* cwin = g_wins[handle].win;
    SFIELD* curfld;
    INT i;

    obj_x = 0;
    obj_y = 0;
    obj_height = 0;
    obj_width = 0;

    curfld = (SFIELD*) ( (BYTE*) cwin + cwin->fldofs );

    for ( i = 0; i < cwin->numflds; i++, curfld++ ) {
        if ( curfld->opt == FLD_OBJAREA ) {
            obj_x = curfld->x;
            obj_y = curfld->y;
            obj_width = curfld->lx;
            obj_height = curfld->ly;
            break;
        }
    }
}

/*------------------------------------------------------------------------
  SWD_GetNextWindow() - Gets the Next Active Window
  ------------------------------------------------------------------------*/
PRIVATE void SWD_GetNextWindow( void ) {
    INT i;
    INT pos = active_window - 1;

    active_window = EMPTY;

    if ( pos < 0 ) {
        pos = MAX_WINDOWS - 1;
    }

    for ( i = 0; i < MAX_WINDOWS; i++ ) {
        if ( g_wins[pos].flag && g_wins[pos].win->display ) {
            active_window = pos;
            active_field = g_wins[pos].win->firstfld;
            break;
        }
        pos--;
        if ( pos < 0 ) {
            pos = MAX_WINDOWS - 1;
        }
    }

    lastfld = NULL;

    if ( active_window == EMPTY ) {
        active_field = EMPTY;
    }
}

/*------------------------------------------------------------------------
  SWD_GetFirstField() - Gets the first selectable field
  ------------------------------------------------------------------------*/
PRIVATE INT SWD_GetFirstField( void ) {
    INT rval = EMPTY;

    rval = g_wins[active_window].win->firstfld;

    return rval;
}

/*------------------------------------------------------------------------
  SWD_GetLastField() - Gets the last selectable field
  ------------------------------------------------------------------------*/
PRIVATE INT SWD_GetLastField(
    SFIELD* firstfld, // INPUT : pointer to first field
    INT maxfields // INPUT : number of fields
) {
    INT i;
    SFIELD* curfld;
    INT rval = EMPTY;

    for ( i = maxfields - 1; i >= 0; i-- ) {
        curfld = firstfld + i;

        switch ( curfld->opt ) {
            case FLD_DRAGBAR:
            case FLD_BUMPIN:
            case FLD_BUMPOUT:
            case FLD_OFF:
            case FLD_TEXT:
            case FLD_ICON:
                break;

            default:
                if ( curfld->selectable ) {
                    rval = i;
                }
                break;
        }

        if ( rval != EMPTY ) {
            break;
        }
    }

    return rval;
}

/*------------------------------------------------------------------------
  SWD_GetNextField() - Gets the Next selectable field
  ------------------------------------------------------------------------*/
PRIVATE INT SWD_GetNextField(
    SFIELD* firstfld, // INPUT : pointer to first field
    INT maxfields // INPUT : number of fields
) {
    SFIELD* activefld = firstfld + active_field;
    SFIELD* curfld;
    INT low = 0x7fff;
    INT i;
    INT rval;
    INT del;

    rval = SWD_GetFirstField();

    for ( i = 0; i < maxfields; i++ ) {
        curfld = firstfld + i;

        if ( curfld->opt == FLD_DRAGBAR ) {
            continue;
        }

        if ( curfld->selectable && curfld->id > activefld->id ) {
            del = abs( curfld->id - activefld->id );
            if ( del < low ) {
                low = del;
                rval = i;
            }
        }
    }

    if ( rval != EMPTY ) {
        active_field = rval;
    }

    return rval;
}

/*------------------------------------------------------------------------
  SWD_GetPrevField() - Gets the Previous selectable field
  ------------------------------------------------------------------------*/
PRIVATE INT SWD_GetPrevField(
    SFIELD* firstfld, // INPUT : pointer to first field
    INT maxfields // INPUT : number of fields
) {
    SFIELD* activefld = firstfld + active_field;
    SFIELD* curfld;
    INT low = 0x7fff;
    INT i;
    INT rval;
    INT del;

    rval = SWD_GetLastField( firstfld, maxfields );

    for ( i = 0; i < maxfields; i++ ) {
        curfld = firstfld + i;

        if ( curfld->opt == FLD_DRAGBAR ) {
            continue;
        }

        if ( curfld->selectable && curfld->id < activefld->id ) {
            del = abs( activefld->id - curfld->id );
            if ( del < low ) {
                low = del;
                rval = i;
            }
        }
    }

    if ( rval != EMPTY ) {
        active_field = rval;
    }

    return rval;
}

/*------------------------------------------------------------------------
  SWD_GetUpField() - Gets the closest right selectable field
  ------------------------------------------------------------------------*/
PRIVATE INT SWD_GetUpField(
    SFIELD* firstfld, // INPUT : pointer to first field
    INT maxfields // INPUT : number of fields
) {
    SFIELD* activefld = firstfld + active_field;
    SFIELD* curfld;
    INT i;
    INT rval = EMPTY;
    INT low = 0x7fff;
    INT del;

    rval = SWD_GetLastField( firstfld, maxfields );

    for ( i = 0; i < maxfields; i++ ) {
        curfld = firstfld + i;

        switch ( curfld->opt ) {
            case FLD_DRAGBAR:
            case FLD_BUMPIN:
            case FLD_BUMPOUT:
            case FLD_OFF:
            case FLD_TEXT:
            case FLD_ICON:
                break;

            default:
                if ( curfld->y < activefld->y ) {
                    del = abs( curfld->x - activefld->x );
                    del += abs( curfld->y - activefld->y );

                    if ( del < low ) {
                        if ( curfld->selectable ) {
                            low = del;
                            rval = i;
                        }
                    }
                }
                break;
        }
    }

    if ( rval < 0 ) {
        active_field = SWD_GetFirstField();
    } else {
        active_field = rval;
    }

    return rval;
}

/*------------------------------------------------------------------------
  SWD_GetDownField() - Gets the closest right selectable field
  ------------------------------------------------------------------------*/
PRIVATE INT SWD_GetDownField(
    SFIELD* firstfld, // INPUT : pointer to first field
    INT maxfields // INPUT : number of fields
) {
    SFIELD* activefld = firstfld + active_field;
    SFIELD* curfld;
    INT i;
    INT rval = EMPTY;
    INT low = 0x7fff;
    INT del;

    rval = SWD_GetFirstField();

    for ( i = 0; i < maxfields; i++ ) {
        curfld = firstfld + i;

        switch ( curfld->opt ) {
            case FLD_DRAGBAR:
            case FLD_BUMPIN:
            case FLD_BUMPOUT:
            case FLD_OFF:
            case FLD_TEXT:
            case FLD_ICON:
                break;

            default:
                if ( curfld->y > activefld->y ) {
                    del = abs( curfld->x - activefld->x );
                    del += abs( curfld->y - activefld->y );

                    if ( del < low ) {
                        if ( curfld->selectable ) {
                            low = del;
                            rval = i;
                        }
                    }
                }
                break;
        }
    }

    if ( rval < 0 ) {
        active_field = SWD_GetFirstField();
    } else {
        active_field = rval;
    }

    return rval;
}

/*------------------------------------------------------------------------
  SWD_ShowAllFields() - Displays all Fields in a window
  ------------------------------------------------------------------------*/
PRIVATE INT // RETURN: number of fields displayed
SWD_ShowAllFields(
    void* inptr // INPUT : pointer to window data
) {
    SWIN* header = inptr;
    SFIELD* fld = (SFIELD*) ( ( (BYTE*) inptr ) + header->fldofs );
    INT numflds = 0;
    INT i;
    INT fx;
    INT fy;
    BYTE* picdata;
    GFX_PIC* pich;

    for ( i = 0; i < header->numflds; i++, fld++, numflds++ ) {
        if ( fld->opt != FLD_OFF ) {
            fx = header->x + fld->x;
            fy = header->y + fld->y;

            if ( fld->saveflag && fld->sptr ) {
                picdata = fld->sptr;
                pich = (GFX_PIC*) picdata;
                picdata += sizeof( GFX_PIC );
                pich->width = (short) fld->lx;
                pich->height = (short) fld->ly;
                GFX_GetScreen( picdata, fx, fy, fld->lx, fld->ly );
            }

            if ( fld->shadow ) {
                if ( fld->picflag != SEE_THRU ) {
                    GFX_LightBox( UPPER_RIGHT, fx - 1, fy + 1, fld->lx, fld->ly );
                } else {
                    if ( fld->item != EMPTY ) {
                        picdata = GLB_GetItem( fld->item );
                        GFX_ShadeShape( DARK, picdata, fx - 1, fy + 1 );
                    }
                }
            }

            SWD_PutField( header, fld );
        }
    }
    return i;
}

/*------------------------------------------------------------------------
  SWD_PutWin() - Displays a single window
  ------------------------------------------------------------------------*/
PRIVATE void SWD_PutWin(
    INT handle // INPUT : number/handle of window
) {
    PRIVATE SWD_DLG wdlg;
    SWIN* cwin = g_wins[handle].win;
    INT x = cwin->x - 8;
    INT y = cwin->y + 8;
    INT y2 = cwin->y + cwin->ly;
    INT lx = cwin->lx;
    INT ly = 8;
    BYTE* pic;
    BOOL draw_style = FALSE;

    if ( cwin->display == FALSE ) {
        return;
    }

    if ( cwin->color == 0 ) {
        draw_style = TRUE;
    }

    if ( cwin->shadow ) {
        if ( cwin->picflag == SEE_THRU && cwin->item != EMPTY ) {
            pic = GLB_GetItem( cwin->item );
            GFX_ShadeShape( DARK, pic, x, y );
        } else {
            GFX_ShadeArea( DARK, x, y, 8, cwin->ly - 8 );
            GFX_ShadeArea( DARK, x, y2, lx, ly );
        }
    }

    switch ( cwin->picflag ) {
        case FILL:
            GFX_ColorBox( cwin->x, cwin->y, cwin->lx, cwin->ly, cwin->color );

            if ( cwin->lx < 320 && cwin->ly < 200 ) {
                GFX_LightBox( UPPER_RIGHT, cwin->x, cwin->y, cwin->lx, cwin->ly );
            }
            break;

        case PICTURE:
            if ( cwin->item != EMPTY ) {
                pic = GLB_GetItem( cwin->item );
                GFX_PutImage( pic, cwin->x, cwin->y, FALSE );
            }
            break;

        case SEE_THRU:
            if ( cwin->item != EMPTY ) {
                pic = GLB_GetItem( cwin->item );
                GFX_PutImage( pic, cwin->x, cwin->y, TRUE );
            }
            break;

        case TEXTURE:
            if ( cwin->item != EMPTY ) {
                pic = GLB_GetItem( cwin->item );
                GFX_PutTexture( pic, cwin->x, cwin->y, cwin->lx, cwin->ly );
                GFX_LightBox( UPPER_RIGHT, cwin->x, cwin->y, cwin->lx, cwin->ly );
            }
            break;

        case INVISABLE:
            if ( !cwin->color ) {
                GFX_ShadeArea( DARK, cwin->x, cwin->y, cwin->lx, cwin->ly );
                GFX_LightBox( UPPER_RIGHT, cwin->x, cwin->y, cwin->lx, cwin->ly );
            }
        default:
            break;
    }

    if ( cwin->numflds ) {
        SWD_ShowAllFields( cwin );
    }

    if ( winfuncs[handle] ) {
        SWD_GetObjAreaInfo( handle );
        wdlg.x = obj_x;
        wdlg.y = obj_y;
        wdlg.width = obj_width;
        wdlg.height = obj_height;
        wdlg.id = cwin->id;
        wdlg.type = cwin->type;
        wdlg.window = active_window;
        wdlg.field = active_field;
        winfuncs[handle]( &wdlg );
    }
}

/*------------------------------------------------------------------------
   SWD_IsButtonDown () - returns TRUE if any SWD Butons are down
  ------------------------------------------------------------------------*/
PRIVATE BOOL SWD_IsButtonDown( void ) {
    if ( KBD_Key( SC_ENTER ) ) {
        return TRUE;
    }

    if ( PTR_B1 ) {
        return TRUE;
    }

    return FALSE;
}

/***************************************************************************
 SWD_Install() - Initializes Window system
 ***************************************************************************/
void SWD_Install(
    BOOL moveflag // INPUT : Use Move Window feature ( 64k )
) {
    CHAR* err = "SWD_Init() - DosMemAlloc";
    DWORD segment;

    g_key = 0;
    memset( g_wins, 0, sizeof( g_wins ) );

    if ( moveflag ) {
        if ( _dpmi_dosalloc( 4000, &segment ) ) {
            EXIT_Error( err );
        }
        movebuffer = (BYTE*) ( segment << 4 );
    } else {
        movebuffer = NULL;
    }

    fldfuncs[0] = NULL;
    fldfuncs[1] = NULL;
    fldfuncs[2] = SWD_DoButton;
    fldfuncs[3] = SWD_FieldInput;
    fldfuncs[4] = SWD_DoButton;
    fldfuncs[5] = NULL;
    fldfuncs[6] = SWD_DoButton;
    fldfuncs[7] = NULL;
    fldfuncs[8] = NULL;
    fldfuncs[9] = NULL;
    fldfuncs[10] = NULL;
    fldfuncs[11] = NULL;
    fldfuncs[12] = NULL;
    fldfuncs[13] = NULL;
    fldfuncs[14] = NULL;
}

/***************************************************************************
   SWD_End () Frees up resources used by SWD System
 ***************************************************************************/
void SWD_End( void ) {
    memset( g_wins, 0, sizeof( g_wins ) );

    fldfuncs[0] = 0;
    fldfuncs[1] = 0;
    fldfuncs[2] = 0;
    fldfuncs[3] = 0;
    fldfuncs[4] = 0;
    fldfuncs[5] = 0;
    fldfuncs[6] = 0;
    fldfuncs[7] = 0;
    fldfuncs[8] = 0;
    fldfuncs[9] = 0;
    fldfuncs[10] = 0;
    fldfuncs[11] = 0;
    fldfuncs[12] = 0;
    fldfuncs[13] = 0;
    fldfuncs[14] = 0;
}

/***************************************************************************
 SWD_InitWindow() - Adds window to list and initailizes
 ***************************************************************************/
INT // RETURN: handle to window
SWD_InitWindow(
    DWORD handle // INPUT : GLB Item Number
) {
    SWIN* header;
    SFIELD* curfld;
    INT pic_size;
    INT rec_num;
    INT i;

    PTR_JoyReset();

    old_win = EMPTY;
    old_field = EMPTY;
    kbactive = FALSE;
    highlight_flag = FALSE;

    if ( lastfld ) {
        lastfld->bstatus = NORMAL;
        lastfld = NULL;
    }

    header = (SWIN*) GLB_LockItem( handle );

    curfld = (SFIELD*) ( (BYTE*) header + header->fldofs );

    for ( rec_num = 0; rec_num < MAX_WINDOWS; rec_num++ ) {
        if ( g_wins[rec_num].flag == FALSE ) {
            prev_window = active_window;
            active_window = rec_num;
            g_wins[rec_num].win = header;
            g_wins[rec_num].flag = TRUE;
            g_wins[rec_num].gitem = handle;

            header->display = TRUE;

            active_field = g_wins[rec_num].win->firstfld;

            if ( !( curfld + active_field )->selectable ) {
                active_field = SWD_GetFirstField();
            }

            if ( header->picflag != FILL ) {
                header->item = GLB_GetItemID( header->item_name );
                GLB_LockItem( header->item );
            }

            for ( i = 0; i < header->numflds; i++, curfld++ ) {
                if ( curfld->opt != FLD_OFF ) {
                    if ( curfld->opt == FLD_VIEWAREA ) {
                        g_wins[active_window].viewflag = TRUE;
                    }

                    switch ( curfld->opt ) {
                        default:
                            curfld->kbflag = FALSE;
                            break;

                        case FLD_INPUT:
                            curfld->kbflag = TRUE;
                            break;

                        case FLD_BUTTON:
                        case FLD_MARK:
                        case FLD_CLOSE:
                        case FLD_DRAGBAR:
                            if ( usekb_flag && curfld->selectable ) {
                                curfld->kbflag = TRUE;
                            } else {
                                curfld->kbflag = FALSE;
                            }
                            break;
                    }

                    curfld->bstatus = NORMAL;
                    curfld->item = EMPTY;

                    curfld->fontid = GLB_GetItemID( curfld->font_name );
                    if ( curfld->fontid != EMPTY ) {
                        GLB_LockItem( curfld->fontid );
                    }

                    if ( curfld->picflag == FILL ) {
                        curfld->item = EMPTY;
                    } else {
                        curfld->item = GLB_GetItemID( curfld->item_name );
                    }

                    if ( curfld->item != EMPTY ) {
                        GLB_LockItem( curfld->item );
                    }

                    curfld->sptr = NULL;
                    if ( curfld->saveflag ) {
                        pic_size = curfld->lx * curfld->ly + sizeof( GFX_PIC );
                        if ( pic_size < 0 || pic_size > 64000 ) {
                            EXIT_Error( "SWD Error: pic save to big..." );
                        }

                        curfld->sptr = (BYTE*) malloc( pic_size );
                        if ( !curfld->sptr ) {
                            EXIT_Error( "SWD Error: out of memory" );
                        }
                    }
                }
            }
            goto SWD_InitWindow_Exit;
        }
    }

    rec_num = EMPTY;

SWD_InitWindow_Exit:

    return rec_num;
}

/***************************************************************************
   SWD_InitMasterWindow () - Inits the Master Window ( must be full screen )
 ***************************************************************************/
INT SWD_InitMasterWindow(
    DWORD handle // INPUT : GLB Item Number
) {
    master_window = SWD_InitWindow( handle );

    return master_window;
}

/***************************************************************************
   SWD_SetViewDrawHook () Sets Function to draw after the master window
 ***************************************************************************/
void SWD_SetViewDrawHook(
    void ( *func )( void ) // INPUT : pointer to function
) {
    viewdraw = func;
}

/***************************************************************************
   SWD_SetWinDrawFunc () - Function called after window is drawn
 ***************************************************************************/
void SWD_SetWinDrawFunc(
    INT handle, // INPUT :handle of window
    void ( *infunc )( SWD_DLG* ) // INPUT :pointer to function
) {
    if ( infunc && g_wins[handle].flag ) {
        winfuncs[handle] = infunc;
    }
}

/***************************************************************************
SWD_SetClearFlag() - Turns ON/OFF memsetting of display buffer in showallwins
 ***************************************************************************/
void SWD_SetClearFlag( BOOL inflag ) {
    clearscreenflag = inflag;
}

/***************************************************************************
 SWD_ShowAllWindows()- Diplays all windows.. puts active window on top
 ***************************************************************************/
BOOL // RETURN : TRUE = OK, FALSE = Error
SWD_ShowAllWindows( void ) {
    INT i;

    if ( active_window < 0 ) {
        return FALSE;
    }

    if ( clearscreenflag ) {
        if ( master_window == EMPTY || !viewdraw ) {
            memset( displaybuffer, 0, 64000 );
        }
    }

    if ( master_window != EMPTY && g_wins[master_window].flag ) {
        SWD_PutWin( master_window );
    }

    if ( viewdraw ) {
        viewdraw();
    }

    for ( i = 0; i < MAX_WINDOWS; i++ ) {
        if ( g_wins[i].flag && i != active_window && i != master_window ) {
            SWD_PutWin( i );
        }
    }

    if ( movebuffer ) {
        memcpy( movebuffer, displaybuffer, 64000 );
    }

    if ( active_window != EMPTY && active_window != master_window && g_wins[active_window].flag ) {
        SWD_PutWin( active_window );
    }

    return TRUE;
}

/***************************************************************************
SWD_SetWindowPtr() - Sets Pointer to center of active field
 ***************************************************************************/
void SWD_SetWindowPtr(
    INT handle // INPUT : number/handle of window
) {
    SWIN* curwin = g_wins[handle].win;
    SFIELD* curfld;

    if ( !ptr_init_flag ) {
        return;
    }
    if ( handle == EMPTY ) {
        return;
    }

    if ( active_field == EMPTY || curwin->numflds == 0 ) {
        if ( g_wins[handle].flag && curwin ) {
            PTR_SetPos( curwin->x + ( curwin->lx >> 1 ), curwin->y + ( curwin->ly >> 1 ) );
        }
        return;
    }

    curfld = (SFIELD*) ( (BYTE*) curwin + curwin->fldofs );
    curfld += active_field;

    PTR_SetPos( curfld->x + ( curfld->lx >> 1 ), curfld->y + ( curfld->ly >> 1 ) );
}

/***************************************************************************
SWD_SetFieldPtr () - Sets Pointer on a field
 ***************************************************************************/
void SWD_SetFieldPtr(
    INT handle, // INPUT : number/handle of window
    INT field // INPUT : field
) {
    SWIN* curwin = g_wins[handle].win;
    SFIELD* curfld;

    if ( !ptr_init_flag ) {
        return;
    }
    if ( handle == EMPTY ) {
        return;
    }

    if ( field == EMPTY || curwin->numflds == 0 ) {
        if ( g_wins[handle].flag && curwin ) {
            PTR_SetPos( curwin->x + ( curwin->lx >> 1 ), curwin->y + ( curwin->ly >> 1 ) );
        }
        return;
    }

    curfld = (SFIELD*) ( (BYTE*) curwin + curwin->fldofs );
    curfld += field;

    PTR_SetPos( curfld->x + ( curfld->lx >> 1 ), curfld->y + ( curfld->ly >> 1 ) );
}

/***************************************************************************
 SWD_SetActiveWindow() - Sets the current working window
 ***************************************************************************/
void SWD_SetActiveWindow(
    INT handle // INPUT : number/handle of window
) {
    if ( g_wins[handle].flag == FALSE ) {
        EXIT_Error( "SWD: SetActiveWindow #%u", handle );
    }

    active_window = handle;
}

/***************************************************************************
 SWD_SetActiveField() - Sets the current working field
 ***************************************************************************/
void SWD_SetActiveField(
    INT handle, // INPUT : handle of window
    INT field_id // INPUT : number/handle of field
) {
    SWIN* curwin = g_wins[handle].win;
    SFIELD* curfld;

    if ( active_field != EMPTY ) {
        curfld = (SFIELD*) ( (BYTE*) curwin + curwin->fldofs );
        curfld += active_field;
        lastfld = curfld;
    }

    curfld = (SFIELD*) ( (BYTE*) curwin + curwin->fldofs );
    curfld += field_id;

    if ( curfld->kbflag ) {
        kbactive = TRUE;
    } else {
        kbactive = FALSE;
    }

    if ( kbactive ) {
        highlight_flag = TRUE;
    }

    active_field = field_id;
}

/***************************************************************************
 SWD_DestroyWindow() - removes a window from SWD system
 ***************************************************************************/
void SWD_DestroyWindow(
    INT handle // INPUT : handle of window
) {
    BYTE* windat = (BYTE*) g_wins[handle].win;
    SWIN* curwin = (SWIN*) windat;
    SFIELD* curfld;
    INT i;
    INT hold;

    PTR_JoyReset();

    if ( g_wins[handle].flag == FALSE ) {
        EXIT_Error( "SWD: DestroyWindow %d", handle );
    }

    curfld = (SFIELD*) ( windat + curwin->fldofs );

    for ( i = 0; i < curwin->numflds; i++, curfld++ ) {
        if ( curfld->item != EMPTY ) {
            GLB_FreeItem( curfld->item );
        }

        if ( curfld->fontid != EMPTY ) {
            GLB_FreeItem( curfld->fontid );
        }

        if ( curfld->saveflag && curfld->sptr != NULL ) {
            free( curfld->sptr );
        }
    }

    if ( curwin->item != EMPTY ) {
        GLB_FreeItem( curwin->item );
    }

    GLB_FreeItem( g_wins[handle].gitem );

    g_wins[handle].flag = FALSE;
    winfuncs[handle] = NULL;

    if ( handle == master_window ) {
        master_window = EMPTY;
    }

    kbactive = FALSE;
    highlight_flag = FALSE;
    lastfld = NULL;

    SWD_GetNextWindow();

    windat = (BYTE*) g_wins[active_window].win;
    curwin = (SWIN*) windat;

    //   if ( windat )
    {
        curfld = (SFIELD*) ( windat + curwin->fldofs );

        if ( active_field != EMPTY ) {
            curfld += active_field;

            if ( curfld->kbflag ) {
                kbactive = TRUE;
            }
        }
    }

    if ( g_wins[prev_window].flag ) {
        hold = active_window;
        active_window = prev_window;
        prev_window = hold;
        active_field = g_wins[active_window].win->firstfld;
    }

    if ( active_window != EMPTY ) {
        SWD_ShowAllWindows();
    }
}

/*------------------------------------------------------------------------
   SWD_CheckMouse () does mouse stuff and returns SWD_XXX code
  ------------------------------------------------------------------------*/
PRIVATE BOOL SWD_CheckMouse(
    SWIN* curwin, // INPUT : pointer to current window
    SFIELD* firstfld // INPUT : pointer to current field
) {
    INT i;
    SFIELD* curfld = firstfld;
    BOOL flag = TRUE;
    INT px = PTR_X;
    INT py = PTR_Y;
    INT x1;
    INT y1;
    INT x2;
    INT y2;
    BOOL mflag = FALSE;

    for ( i = 0; i < curwin->numflds; i++, curfld++ ) {
        x1 = curfld->x + curwin->x;
        y1 = curfld->y + curwin->y;
        x2 = x1 + curfld->lx + 1;
        y2 = y1 + curfld->ly + 1;

        if ( px >= x1 && px <= x2 && py >= y1 && py <= y2 ) {
            flag = TRUE;
            switch ( curfld->opt ) {
                default:
                case FLD_OFF:
                case FLD_TEXT:
                case FLD_ICON:
                    flag = FALSE;
                    break;

                case FLD_DRAGBAR:
                    if ( curfld->selectable ) {
                        mflag = FALSE;
                        cur_act = S_WIN_COMMAND;
                        cur_cmd = W_MOVE;
                        active_field = i;
                    } else {
                        flag = FALSE;
                    }
                    break;

                case FLD_CLOSE:
                case FLD_INPUT:
                case FLD_BUTTON:
                case FLD_MARK:
                    mflag = FALSE;
                    cur_act = S_FLD_COMMAND;
                    cur_cmd = F_SELECT;
                    active_field = i;
                    break;

                case FLD_OBJAREA:
                    mflag = FALSE;
                    cur_act = S_FLD_COMMAND;
                    cur_cmd = F_OBJ_AREA;
                    break;

                case FLD_VIEWAREA:
                    mflag = FALSE;
                    cur_act = S_FLD_COMMAND;
                    cur_cmd = F_VIEW_AREA;
                    break;
            }
            if ( flag ) {
                break;
            }
        }
    }

    if ( mflag ) {
        while ( PTR_B1 ) {
        }
    }

    return flag;
}

/*------------------------------------------------------------------------
   SWD_CheckViewArea ()
  ------------------------------------------------------------------------*/
PRIVATE BOOL SWD_CheckViewArea(
    SWD_DLG* dlg, // INPUT : pointer to DLG window messages
    SWIN* curwin, // INPUT : pointer to current window
    SFIELD* curfld // INPUT : pointer to current field
) {
    INT i;
    BOOL flag = FALSE;
    INT px = PTR_X;
    INT py = PTR_Y;
    INT x1;
    INT y1;
    INT x2;
    INT y2;

    for ( i = 0; i < curwin->numflds; i++, curfld++ ) {
        x1 = curfld->x + curwin->x;
        y1 = curfld->y + curwin->y;
        x2 = x1 + curfld->lx + 1;
        y2 = y1 + curfld->ly + 1;

        if ( px >= x1 && px <= x2 && py >= y1 && py <= y2 ) {
            switch ( curfld->opt ) {
                default:
                    break;

                case FLD_VIEWAREA:
                    flag = TRUE;
                    dlg->viewactive = TRUE;
                    dlg->sx = curfld->x;
                    dlg->sy = curfld->y;
                    dlg->height = curfld->lx;
                    dlg->width = curfld->ly;
                    dlg->sfield = i;
                    break;
            }
            if ( flag ) {
                break;
            }
        }
    }

    return flag;
}

/*------------------------------------------------------------------------
   SWD_ClearAllButtons () Clears all buttons in all windows to NORMAL
  ------------------------------------------------------------------------*/
PRIVATE void SWD_ClearAllButtons( void ) {
    INT j;
    INT i;
    SFIELD* curfld;
    SWIN* curwin;

    for ( j = 0; j < MAX_WINDOWS; j++ ) {
        if ( g_wins[j].flag ) {
            curwin = g_wins[j].win;
            curfld = (SFIELD*) ( (BYTE*) curwin + curwin->fldofs );

            for ( i = 0; i < curwin->numflds; i++, curfld++ ) {
                curfld->bstatus = NORMAL;
            }
        }
    }
}

/***************************************************************************
   SWD_Dialog () - performs all window in/out/diaplay/move stuff
 ***************************************************************************/
void SWD_Dialog(
    SWD_DLG* swd_dlg // OUTPUT: pointer to info structure
) {
    SWIN* curwin;
    SFIELD* firstfld;
    SFIELD* curfld;
    SFIELD* testfld;
    INT x;
    INT y;
    INT sx;
    INT sy;
    INT i;
    BOOL update;

    _disable();
    update = FALSE;
    g_key = KBD_LASTSCAN;
    g_ascii = KBD_LASTASCII;
    KBD_LASTSCAN = SC_NONE;
    KBD_LASTASCII = SC_NONE;
    _enable();

    if ( active_window == EMPTY ) {
        return;
    }

    curwin = g_wins[active_window].win;
    firstfld = (SFIELD*) ( (BYTE*) curwin + curwin->fldofs );
    curfld = firstfld + active_field;

    cur_act = S_IDLE;
    cur_cmd = C_IDLE;

    if ( highlight_flag ) {
        highlight_flag = FALSE;
        if ( lastfld ) {
            lastfld->bstatus = NORMAL;
            SWD_PutField( curwin, lastfld );
            lastfld = NULL;
            update = TRUE;
        }

        if ( kbactive ) {
            curfld->bstatus = UP;
            SWD_PutField( curwin, curfld );
            lastfld = curfld;
            update = TRUE;
        }
    }

    if ( old_win != active_window ) {
        SWD_ClearAllButtons();
        lastfld = NULL;
        highlight_flag = TRUE;
        cur_act = S_WIN_COMMAND;
        cur_cmd = C_IDLE;
        if ( curfld->kbflag ) {
            kbactive = TRUE;
        } else {
            kbactive = FALSE;
        }
    }

    old_win = active_window;

    swd_dlg->viewactive = FALSE;
    if ( g_wins[active_window].viewflag ) {
        SWD_CheckViewArea( swd_dlg, curwin, firstfld );
    }

    if ( active_field == EMPTY ) {
        return;
    }

    if ( PTR_B1 && cur_act == S_IDLE ) {
        old_field = active_field;
        if ( SWD_CheckMouse( curwin, firstfld ) ) {
            if ( old_win != active_window ) {
                SWD_ClearAllButtons();
                lastfld = 0;
                curwin = g_wins[active_window].win;
                firstfld = (SFIELD*) ( (BYTE*) curwin + curwin->fldofs );
                active_field = curwin->firstfld;
                curfld = firstfld + active_field;
                SWD_ShowAllWindows();
                GFX_DisplayUpdate();
            } else if ( old_field != active_field ) {
                highlight_flag = TRUE;
                curfld = firstfld + active_field;
            }

            if ( curfld->kbflag ) {
                highlight_flag = TRUE;
                kbactive = TRUE;
            } else {
                highlight_flag = TRUE;
                kbactive = FALSE;
            }
        }
    } else {
        if ( fldfuncs[curfld->opt] && cur_act == S_IDLE ) {
            fldfuncs[curfld->opt]( curwin, curfld );

            testfld = firstfld;
            for ( i = 0; i < curwin->numflds; i++, testfld++ ) {
                if ( testfld->hotkey != SC_NONE ) {
                    if ( testfld->hotkey == (unsigned int) g_key ) {
                        if ( !usekb_flag ) {
                            kbactive = FALSE;
                        }
                        active_field = i;
                        curfld = firstfld + active_field;
                        if ( lastfld ) {
                            lastfld->bstatus = NORMAL;
                            SWD_PutField( curwin, lastfld );
                            lastfld = NULL;
                            update = TRUE;
                        }
                        highlight_flag = TRUE;
                        lastfld = curfld;
                        cur_act = S_FLD_COMMAND;
                        cur_cmd = F_SELECT;
                        break;
                    }
                }
            }

            if ( cur_act != S_IDLE && cur_cmd != F_SELECT ) {
                if ( kbactive == FALSE ) {
                    highlight_flag = TRUE;
                    cur_act = S_FLD_COMMAND;
                    cur_cmd = F_FIRST;
                    kbactive = TRUE;
                }
            }
        }
    }

    old_field = active_field;

    swd_dlg->window = active_window;
    swd_dlg->field = active_field;
    swd_dlg->id = curwin->id;
    swd_dlg->type = curwin->type;
    swd_dlg->cur_act = cur_act;
    swd_dlg->cur_cmd = cur_cmd;
    swd_dlg->keypress = g_key;

    switch ( cur_act ) {
        case S_IDLE:
            break;

        case S_FLD_COMMAND:
            swd_dlg->x = curfld->x;
            swd_dlg->y = curfld->y;
            swd_dlg->width = curfld->lx;
            swd_dlg->height = curfld->ly;

            switch ( cur_cmd ) {
                case C_IDLE:
                    break;

                case F_DOWN:
                    SWD_GetDownField( firstfld, curwin->numflds );
                    break;

                case F_UP:
                    SWD_GetUpField( firstfld, curwin->numflds );
                    break;

                case F_RIGHT:
                case F_NEXT:
                    SWD_GetNextField( firstfld, curwin->numflds );
                    break;

                case F_LEFT:
                case F_PREV:
                    SWD_GetPrevField( firstfld, curwin->numflds );
                    break;

                case F_TOP:
                    active_field = SWD_GetFirstField();
                    break;

                case F_BOTTOM:
                    active_field = SWD_GetLastField( firstfld, curwin->numflds );
                    break;

                case F_FIRST:
                    active_field = curwin->firstfld;
                    break;

                case F_SELECT:
                    curfld->bstatus = DOWN;
                    SWD_PutField( curwin, curfld );
                    curfld->mark ^= TRUE;
                    if ( lastfld && lastfld != curfld ) {
                        lastfld->bstatus = NORMAL;
                        SWD_PutField( curwin, lastfld );
                        lastfld = 0;
                    }
                    GFX_DisplayUpdate();
                    while ( SWD_IsButtonDown() )
                        ;
                    if ( kbactive || curfld->kbflag ) {
                        curfld->bstatus = UP;
                    } else {
                        curfld->bstatus = NORMAL;
                    }

                    SWD_PutField( curwin, curfld );
                    update = TRUE;
                    break;
            }
            break;

        case S_WIN_COMMAND:
            swd_dlg->x = curwin->x;
            swd_dlg->y = curwin->y;
            swd_dlg->width = curwin->lx;
            swd_dlg->height = curwin->ly;
            switch ( cur_cmd ) {
                case C_IDLE:
                    break;

                case W_NEW:
                    update = TRUE;
                    break;

                case W_NEXT:
                    if ( !curwin->lock ) {
                        SWD_GetNextWindow();
                        if ( active_window == master_window ) {
                            SWD_GetNextWindow();
                        }
                        curwin = g_wins[active_window].win;
                        firstfld = (SFIELD*) ( (BYTE*) curwin + curwin->fldofs );
                        active_field = SWD_GetFirstField();
                        if ( lastfld ) {
                            lastfld->bstatus = NORMAL;
                        }
                        SWD_ShowAllWindows();
                        update = TRUE;
                    }
                    break;

                case W_MOVE:
                    if ( movebuffer == NULL ) {
                        break;
                    }
                    curfld->bstatus = DOWN;
                    SWD_PutField( curwin, curfld );
                    if ( lastfld && lastfld != curfld ) {
                        lastfld->bstatus = NORMAL;
                        SWD_PutField( curwin, lastfld );
                        lastfld = 0;
                    }
                    GFX_DisplayUpdate();
                    sx = PTR_X - curwin->x;
                    sy = PTR_Y - curwin->y;

                    KBD_Key( SC_ENTER ) = FALSE;
                    KBD_LASTSCAN = SC_NONE;
                    SWD_ShowAllWindows();
                    while ( PTR_B1 ) {
                        x = PTR_X - sx;
                        y = PTR_Y - sy;
                        curwin->x = x;
                        curwin->y = y;
                        GFX_MarkUpdate( 0, 0, 320, 200 );
                        memcpy( displaybuffer, movebuffer, 64000 );
                        SWD_PutWin( active_window );
                        GFX_DisplayUpdate();
                    }
                    curfld->bstatus = NORMAL;
                    SWD_PutField( curwin, curfld );
                    update = TRUE;
                    break;

                case W_CLOSE:
                    SWD_ClearAllButtons();
                    lastfld = 0;
                    kbactive = FALSE;
                    break;

                case W_CLOSE_ALL:
                    SWD_ClearAllButtons();
                    lastfld = 0;
                    kbactive = FALSE;
                    break;
            }
            break;

        case S_END:
            break;

        case S_UPDATE:
            update = TRUE;
            break;

        case S_REDRAW:
            SWD_ShowAllWindows();
            update = TRUE;
            break;
    }

    if ( old_field != active_field && active_field >= 0 && kbactive ) {
        highlight_flag = TRUE;
    }

    if ( update ) {
        GFX_DisplayUpdate();
    }
}

/***************************************************************************
   SWD_SetWindowLock() - Locks Window so no others can be selected
 ***************************************************************************/
void SWD_SetWindowLock(
    INT handle, // INPUT : handle to window
    BOOL lock // INPUT : TRUE/FALSE
) {
    if ( g_wins[handle].flag == TRUE ) {
        g_wins[handle].win->lock = lock;
    }
}

/***************************************************************************
 SWD_SetWindowXY() - Sets the window x,y position
 ***************************************************************************/
INT // RETURN: window opt flag
SWD_SetWindowXY(
    INT handle, // INPUT : handle to window
    INT xpos, // INPUT : x position
    INT ypos // INPUT : y position
) {
    SWIN* curwin = g_wins[handle].win;

    curwin->x = xpos;
    curwin->x = ypos;

    return curwin->opt;
}

/***************************************************************************
 SWD_GetWindowXYL () - gets the window x,y ,x length, y length
 ***************************************************************************/
INT // RETURN: window opt flag
SWD_GetWindowXYL(
    INT handle, // INPUT : handle to window
    INT* xpos, // OUTPUT: x position
    INT* ypos, // OUTPUT: y position
    INT* lx, // OUTPUT: x length
    INT* ly // OUTPUT: y length
) {
    SWIN* curwin = g_wins[handle].win;

    if ( xpos ) {
        *xpos = curwin->x;
    }
    if ( ypos ) {
        *ypos = curwin->y;
    }
    if ( lx ) {
        *lx = curwin->lx;
    }
    if ( ly ) {
        *ly = curwin->ly;
    }

    return curwin->opt;
}

/***************************************************************************
 SWD_GetFieldText() - Gets the field text
 ***************************************************************************/
INT // RETURN: text max length
SWD_GetFieldText(
    INT handle, // INPUT : window handle
    INT field_id, // INPUT : field handle
    CHAR* out_text // OUTPUT: text
) {
    SWIN* curwin = g_wins[handle].win;
    SFIELD* curfld;
    BYTE* text;

    curfld = (SFIELD*) ( (BYTE*) curwin + curwin->fldofs );
    curfld += field_id;

    text = (BYTE*) curfld + curfld->txtoff;

    memcpy( out_text, text, curfld->maxchars );

    return curfld->maxchars;
}

/***************************************************************************
 SWD_SetFieldText() - Sets The default field text
 ***************************************************************************/
INT // RETURN: text max length
SWD_SetFieldText(
    INT handle, // INPUT : window handle
    INT field_id, // INPUT : field handle
    CHAR* in_text // OUTPUT: pointer to string
) {
    SWIN* curwin = g_wins[handle].win;
    SFIELD* curfld;
    BYTE* text;

    curfld = (SFIELD*) ( (BYTE*) curwin + curwin->fldofs );
    curfld += field_id;

    text = (BYTE*) curfld + curfld->txtoff;

    if ( in_text ) {
        text[curfld->maxchars - 1] = '\0';
        memcpy( text, in_text, curfld->maxchars - 1 );
    } else {
        *text = '\0';
    }

    return curfld->maxchars;
}

/***************************************************************************
   SWD_GetFieldValue () Returns INT value of field text string
 ***************************************************************************/
INT SWD_GetFieldValue(
    INT handle, // INPUT : window handle
    INT field_id // INPUT : field handle
) {
    SWIN* curwin = g_wins[handle].win;
    SFIELD* curfld;
    BYTE* text;

    curfld = (SFIELD*) ( (BYTE*) curwin + curwin->fldofs );
    curfld += field_id;

    text = (BYTE*) curfld + curfld->txtoff;

    return atoi( (CHAR*) text );
}

/***************************************************************************
   SWD_SetFieldValue () Sets Numeric (INT) Value into Field Text
 ***************************************************************************/
INT SWD_SetFieldValue(
    INT handle, // INPUT : window handle
    INT field_id, // INPUT : field handle
    INT num // INPUT : number to set in fld text
) {
    SWIN* curwin = g_wins[handle].win;
    SFIELD* curfld;
    BYTE* text;

    curfld = (SFIELD*) ( (BYTE*) curwin + curwin->fldofs );
    curfld += field_id;

    text = (BYTE*) curfld + curfld->txtoff;

    sprintf( (CHAR*) text, "%d", num );

    return atoi( (CHAR*) text );
}

/***************************************************************************
SWD_SetFieldSelect() - Sets Field Selectable status
 ***************************************************************************/
void SWD_SetFieldSelect(
    INT handle, // INPUT : window handle
    INT field_id, // INPUT : field handle
    BOOL opt // INPUT : TRUE, FALSE
) {
    SWIN* curwin = g_wins[handle].win;
    SFIELD* curfld;

    curfld = (SFIELD*) ( (BYTE*) curwin + curwin->fldofs );
    curfld += field_id;

    curfld->selectable = opt;
}

/***************************************************************************
 SWD_GetFieldMark() - Gets the field mark status ( TRUE or FALSE )
 ***************************************************************************/
BOOL // RETURN: mark status ( TRUE, FALSE )
SWD_GetFieldMark(
    INT handle, // INPUT : window handle
    INT field_id // INPUT : field handle
) {
    SWIN* curwin = g_wins[handle].win;
    SFIELD* curfld;

    curfld = (SFIELD*) ( (BYTE*) curwin + curwin->fldofs );
    curfld += field_id;

    return curfld->mark;
}

/***************************************************************************
 SWD_SetFieldMark() - Sets the Field Mark ( button )
 ***************************************************************************/
void SWD_SetFieldMark(
    INT handle, // INPUT : window handle
    INT field_id, // INPUT : field handle
    BOOL opt // INPUT : TRUE, FALSE
) {
    SWIN* curwin = g_wins[handle].win;
    SFIELD* curfld;

    curfld = (SFIELD*) ( (BYTE*) curwin + curwin->fldofs );
    curfld += field_id;

    curfld->mark = opt;
}

/***************************************************************************
 SWD_GetFieldInputOpt() - Gets the field InputOpt status
 ***************************************************************************/
INPUTOPT // RETURN: InputOpt status
SWD_GetFieldInputOpt(
    INT handle, // INPUT : window handle
    INT field_id // INPUT : field handle
) {
    SWIN* curwin = g_wins[handle].win;
    SFIELD* curfld;

    curfld = (SFIELD*) ( (BYTE*) curwin + curwin->fldofs );
    curfld += field_id;

    return curfld->input_opt;
}

/***************************************************************************
 SWD_SetFieldInputOpt() - Sets the Field InputOpt ( button )
 ***************************************************************************/
INPUTOPT // RETURN: InputOpt status
SWD_SetFieldInputOpt(
    INT handle, // INPUT : window handle
    INT field_id, // INPUT : field handle
    INPUTOPT opt // INPUT : input option
) {
    SWIN* curwin = g_wins[handle].win;
    SFIELD* curfld;
    INPUTOPT old_opt;

    curfld = (SFIELD*) ( (BYTE*) curwin + curwin->fldofs );
    curfld += field_id;

    old_opt = curfld->input_opt;
    curfld->input_opt = opt;

    return old_opt;
}

/***************************************************************************
   SWD_SetFieldItem () - Sets field Item ID ( picture )
 ***************************************************************************/
void SWD_SetFieldItem(
    INT handle, // INPUT : window handle
    INT field_id, // INPUT : field handle
    DWORD item // INPUT : GLB item id
) {
    SWIN* curwin = g_wins[handle].win;
    SFIELD* curfld;

    curfld = (SFIELD*) ( (BYTE*) curwin + curwin->fldofs );
    curfld += field_id;

    if ( item != EMPTY ) {
        if ( curfld->item != EMPTY ) {
            GLB_FreeItem( curfld->item );
        }

        curfld->item = item;

        GLB_LockItem( item );
    } else {
        curfld->item = EMPTY;
    }
}

/***************************************************************************
   SWD_GetFieldItem () - Returns Field Item number
 ***************************************************************************/
DWORD // RETURN: Item GLB ID
SWD_GetFieldItem(
    INT handle, // INPUT : window handle
    INT field_id // INPUT : field handle
) {
    SWIN* curwin = g_wins[handle].win;
    SFIELD* curfld;

    curfld = (SFIELD*) ( (BYTE*) curwin + curwin->fldofs );
    curfld += field_id;

    return curfld->item;
}

/***************************************************************************
   SWD_SetFieldItemName () - Sets Field Item Name and Loads it in
 ***************************************************************************/
void SWD_SetFieldItemName(
    INT handle, // INPUT : window handle
    INT field_id, // INPUT : field handle
    CHAR* item_name // INPUT : pointer to Item Name
) {
    SWIN* curwin = g_wins[handle].win;
    SFIELD* curfld;
    DWORD item;

    curfld = (SFIELD*) ( (BYTE*) curwin + curwin->fldofs );
    curfld += field_id;

    item = GLB_GetItemID( item_name );

    if ( item != EMPTY ) {
        if ( curfld->item != EMPTY ) {
            GLB_FreeItem( curfld->item );
        }

        memcpy( curfld->item_name, item_name, 16 );
        curfld->item = item;
        GLB_LockItem( curfld->item );
    }
}

/***************************************************************************
   SWD_GetFieldItemName () - Gets Field Item Name
 ***************************************************************************/
void SWD_GetFieldItemName(
    INT handle, // INPUT : window handle
    INT field_id, // INPUT : field handle
    CHAR* item_name // OUTPUT: pointer to Item Name
) {
    SWIN* curwin = g_wins[handle].win;
    SFIELD* curfld;

    curfld = (SFIELD*) ( (BYTE*) curwin + curwin->fldofs );
    curfld += field_id;

    memcpy( item_name, curfld->item_name, 16 );
}

/***************************************************************************
   SWD_SetWindowID () - Sets Window ID number
 ***************************************************************************/
INT // RETURN: old Window ID
SWD_SetWindowID(
    INT handle, // INPUT : window handle
    INT id // INPUT : NEW window ID
) {
    SWIN* curwin = g_wins[handle].win;
    INT old_id = curwin->id;

    curwin->id = id;

    return old_id;
}

/***************************************************************************
   SWD_GetWindowID () - Returns Window ID number
 ***************************************************************************/
INT SWD_GetWindowID(
    INT handle // INPUT : window handle
) {
    SWIN* curwin = g_wins[handle].win;

    return curwin->id;
}

/***************************************************************************
SWD_SetWindowFlag () - Sets A window to be turned on/off
 ***************************************************************************/
INT SWD_SetWindowFlag(
    INT handle, // INPUT : window handle
    BOOL flag // INPUT : TRUE/FALSE
) {
    SWIN* curwin = g_wins[handle].win;

    curwin->display = flag;

    SWD_GetNextWindow();

    return curwin->id;
}

/***************************************************************************
   SWD_SetWindowType () Sets Window TYPE number
 ***************************************************************************/
INT // RETURN: old Window TYPE
SWD_SetWindowType(
    INT handle, // INPUT : window handle
    INT type // INPUT : NEW window TYPE
) {
    SWIN* curwin = g_wins[handle].win;
    INT old_type = curwin->type;

    curwin->type = type;

    return old_type;
}

/***************************************************************************
   SWD_GetWindowType () - Returns Window TYPE number
 ***************************************************************************/
INT // RETURN: window TYPE
SWD_GetWindowType(
    INT handle // INPUT : window handle
) {
    SWIN* curwin = g_wins[handle].win;

    return curwin->type;
}

/***************************************************************************
   SWD_GetFieldXYL () Gets Field X,Y, WIDTH, HEIGHT
 ***************************************************************************/
INT SWD_GetFieldXYL(
    INT handle, // INPUT : window handle
    INT field_id, // INPUT : field handle
    INT* x, // OUTPUT: x
    INT* y, // OUTPUT: y
    INT* lx, // OUTPUT: width
    INT* ly // OUTPUT: height
) {
    SWIN* curwin = g_wins[handle].win;
    SFIELD* curfld;

    curfld = (SFIELD*) ( (BYTE*) curwin + curwin->fldofs );
    curfld += field_id;

    if ( x ) {
        *x = curwin->x + curfld->x;
    }

    if ( y ) {
        *y = curwin->y + curfld->y;
    }

    if ( lx ) {
        *lx = curfld->lx;
    }

    if ( ly ) {
        *ly = curfld->ly;
    }

    return curfld->lx;
}
