

#include "../gfx/kbdapi.h"

#include "public.h"

#include "inc/file0001.inc"

#include "inc/helpwin.inc"

#include "fx.h"
#include "windows.h"

#include "help.h"

PRIVATE DWORD startitem;
PRIVATE int curpage;
PRIVATE int maxpages;

/***************************************************************************
HELP_Init() - inits the help stuff
 ***************************************************************************/
void HELP_Init( void ) {
    int enditem;

    startitem = GLB_GetItemID( "STARTHELP" );
    enditem = GLB_GetItemID( "ENDHELP" );

    if ( !reg_flag ) {
        startitem += (DWORD) 2;
    }

    maxpages = enditem - startitem - 1;

    startitem++;

    curpage = 0;
}

/***************************************************************************
HELP_Win() - Displays the help window at the specified page
 ***************************************************************************/
void HELP_Win(
    char* strpage // INPUT : GLB string item
) {
    char temp[20];
    SWD_DLG dlg;
    int window;
    bool update = true;
    DWORD item;

    item = GLB_GetItemID( strpage );

    if ( item == EMPTY ) {
        EXIT_Error( "HELP() - Invalid Page" );
    }

    curpage = item - startitem;

    KBD_Clear();
    window = SWD_InitWindow( HELP_SWD );

    SND_Patch( FX_DOOR, 127 );

mainloop:

    if ( update ) {
        update = false;

        if ( curpage >= 0 ) {
            curpage = curpage % maxpages;
        } else {
            curpage = maxpages + curpage;
        }

        SWD_SetFieldItem( window, HELP_TEXT, startitem + (DWORD) curpage );

        sprintf( temp, "PAGE : %02u", curpage + 1 );
        SWD_SetFieldText( window, HELP_HEADER, temp );

        SWD_ShowAllWindows();
        GFX_DisplayUpdate();
    }

    SWD_Dialog( &dlg );

    switch ( dlg.keypress ) {
        case SC_HOME:
            update = true;
            curpage = 0;
            break;

        case SC_F1:
            update = true;
            curpage = 1;
            break;

        case SC_END:
            update = true;
            curpage = maxpages - 1;
            break;

        case SC_DOWN:
        case SC_RIGHT:
        case SC_PAGEDN:
            update = true;
            curpage++;
            break;

        case SC_UP:
        case SC_LEFT:
        case SC_PAGEUP:
            update = true;
            curpage--;
            break;
    }

    if ( KBD_IsKey( SC_ESC ) ) {
        goto func_exit;
    }

    if ( KBD_Key( SC_X ) && KBD_Key( SC_ALT ) ) {
        WIN_AskExit();
    }

    switch ( dlg.cur_act ) {
        case S_FLD_COMMAND:
            switch ( dlg.cur_cmd ) {
                case F_SELECT:
                    switch ( dlg.field ) {
                        case HELP_DONE:
                            goto func_exit;

                        case HELP_DOWN:
                            update = true;
                            curpage++;
                            break;

                        case HELP_UP:
                            update = true;
                            curpage--;
                            break;
                    }
            }
    }

    goto mainloop;

func_exit:

    SWD_DestroyWindow( window );
    SWD_ShowAllWindows();
    GFX_DisplayUpdate();
}
