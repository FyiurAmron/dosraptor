/****************************************************************************
 * KBDapi  - keyboard routines
 *---------------------------------------------------------------------------*/

#ifndef KBDAPI_H
#define KBDAPI_H

#include "types.h"
#include "keys.h"

/***************************************************************************
   KBD_Clear() - Resets all flags
 ***************************************************************************/
void KBD_Clear( void );

/***************************************************************************
 KBD_SetKeyboardHook() - Sets User function to call from keyboard handler
 ***************************************************************************/
void // RETURN: none
    KBD_SetKeyboardHook( void ( *hook )( void ) // INPUT : pointer to function
    );

/***************************************************************************
   KBD_Ascii2Scan () - converts most ASCII chars to keyboard scan code
 ***************************************************************************/
int // RETURN: scan code
KBD_Ascii2Scan(
    int ascii // INPUT : ASCII character
);

/***************************************************************************
KBD_Wait() - Waits for Key to be released
 ***************************************************************************/
void KBD_Wait(
    int scancode // SCANCODE see keys.h
);

/***************************************************************************
KBD_IsKey() - Tests to see if key is down if so waits for release
 ***************************************************************************/
BOOL KBD_IsKey(
    int scancode // SCANCODE see keys.h
);

/***************************************************************************
   KBD_Install() - Sets up keyboard system
 ***************************************************************************/
void KBD_Install( void );

/***************************************************************************
   KBD_End() - Shuts down KBD system
 ***************************************************************************/
void KBD_End( void );

/***************************************************************************
   KBD_Key() - returns true/false for scancode
 ***************************************************************************/
// implemented as a macro
/*
void KBD_Key(
    int scancode // INPUT : scan code to read
);
*/

extern BOOL paused, capslock, kbd_ack;
extern BOOL keyboard[256];
extern int lastscan;
extern int lastascii;

#define KBD_Key( c )  keyboard[c]
#define KBD_ISPAUSED  paused
#define KBD_ISCAPS    ( capslock + keyboard[SC_LEFT_SHIFT] + keyboard[SC_RIGHT_SHIFT] )
#define KBD_LASTSCAN  lastscan
#define KBD_LASTASCII lastascii

#endif
