/****************************************************************************
 * KBDapi  - keyboard routines
 *----------------------------------------------------------------------------
 * Copyright (C) 1992  Scott Hostynski All Rights Reserved
 *----------------------------------------------------------------------------
 *
 * Created by:  Scott Host
 * Date:        Oct, 1992
 *
 * CONTENTS: kbdapi.c kbdapi.h keys.h
 *
 * EXTERN MODULES - NONE
 *
 *---------------------------------------------------------------------------*/
#include <conio.h>
#include <ctype.h>
#include <dos.h>
#include <stdio.h>
#include <string.h>

#include "dpmiapi.h"
#include "kbdapi.h"

#define KEYBOARDINT 0x9

PUBLIC bool kbd_ack = false;

bool keyboard[256];
bool paused, capslock;
int lastscan;
int lastascii;

PRIVATE void( _interrupt _far* oldkeyboardisr )() = 0L;
PRIVATE void ( *keyboardhook )( void ) = (void( * )) 0;

PUBLIC int ASCIINames[] = // Unshifted ASCII for scan codes
    {
        //       0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
        0,   27,  '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 8,   9, // 0
        'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', 13,  0,   'a', 's', // 1
        'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', 39,  '`', 0,   92,  'z', 'x', 'c', 'v', // 2
        'b', 'n', 'm', ',', '.', '/', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0, // 3
        0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1', // 4
        '2', '3', '0', 127, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 5
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 6
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 // 7
};

PRIVATE int ShiftNames[] = // Shifted ASCII for scan codes
    {
        //       0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
        0,   27,  '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 8,   9, // 0
        'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', 13,  0,   'A', 'S', // 1
        'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', 34,  '~', 0,   '|', 'Z', 'X', 'C', 'V', // 2
        'B', 'N', 'M', '<', '>', '?', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0, // 3
        0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1', // 4
        '2', '3', '0', 127, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 5
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 6
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 // 7
};

/*------------------------------------------------------------------------
   KBD_ReadScan () - reads scan code, ( interrupt routine )
  ------------------------------------------------------------------------*/
void _interrupt KBD_ReadScan( void ) {
    static BYTE key;

    key = inp( 0x60 );
    outp( 0x20, 0x20 );

    if ( key & 0x80 ) {
        key &= 0x7f;
        keyboard[key] = false;

        if ( key == SC_CAPS_LOCK ) {
            capslock = false;
        }
    } else {
        switch ( key ) {
            case 0xE0:
                break;

            case 0xE1: // pause key
                paused ^= true;
                break;

            case 0x80:
                break;

            default:
                kbd_ack = true;
                lastscan = key;
                keyboard[key] = true;
                if ( lastscan && KBD_ISCAPS ) {
                    lastascii = ShiftNames[key];
                } else {
                    lastascii = ASCIINames[key];
                }
                if ( key == SC_CAPS_LOCK ) {
                    capslock = true;
                }

                *(short*) 0x41c = *(short*) 0x41a; // clear bios key buffer
                break;
        }
    }

    //   _chain_intr ( oldkeyboardisr );
}

/***************************************************************************
   KBD_Clear() - Resets all flags
 ***************************************************************************/
void KBD_Clear( void ) {
    lastscan = SC_NONE;
    memset( keyboard, 0, sizeof( keyboard ) );
}

/***************************************************************************
 KBD_SetKeyboardHook() - Sets User function to call from keyboard handler
 ***************************************************************************/
void // RETURN: none
KBD_SetKeyboardHook( void ( *hook )( void ) // INPUT : pointer to function
) {
    keyboardhook = hook;
}

/***************************************************************************
   KBD_Ascii2Scan () - converts most ASCII chars to keyboard scan code
 ***************************************************************************/
int // RETURN: scan code
KBD_Ascii2Scan(
    int ascii // INPUT : ASCII character
) {
    int i;

    ascii = tolower( ascii );

    for ( i = 0; i < 100; i++ ) {
        if ( ASCIINames[i] == ascii ) {
            return i;
        }
    }

    return 0;
}

/***************************************************************************
KBD_Wait() - Waits for Key to be released
 ***************************************************************************/
void KBD_Wait(
    int scancode // SCANCODE see keys.h
) {
    volatile bool* ky;

    ky = &keyboard[scancode];

    for ( ;; ) {
        if ( !*ky ) {
            break;
        }
    }

    lastscan = SC_NONE;
    lastascii = SC_NONE;
}

/***************************************************************************
KBD_IsKey() - Tests to see if key is down if so waits for release
 ***************************************************************************/
bool KBD_IsKey(
    int scancode // SCANCODE see keys.h
) {
    if ( KBD_Key( scancode ) ) {
        KBD_Wait( scancode );
        return true;
    }

    return false;
}

/***************************************************************************
   KBD_Install() - Sets up keyboard system
 ***************************************************************************/
void KBD_Install( void ) {
    oldkeyboardisr = _dos_getvect( KEYBOARDINT );
    memset( keyboard, 0, sizeof( keyboard ) );

    _dos_setvect( KEYBOARDINT, KBD_ReadScan );
}

/***************************************************************************
   KBD_End() - Shuts down KBD system
 ***************************************************************************/
void KBD_End( void ) {
    _dos_setvect( KEYBOARDINT, oldkeyboardisr );
}
