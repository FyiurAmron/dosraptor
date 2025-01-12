/****************************************************************************
 * IMSapi  - Input Managenment System
 *----------------------------------------------------------------------------
 * Copyright (C) 1992  Scott Hostynski All Rights Reserved
 *----------------------------------------------------------------------------
 *
 * Created by:  Scott Host
 * Date:        Oct, 1992
 *
 * CONTENTS: imsapi.c imsapi.h keys.h
 *
 * EXTERN MODULES - ptrapi, kbdapi, tsmapi
 *
 *---------------------------------------------------------------------------*/
#include <conio.h>
#include <dos.h>
#include <string.h>

#include "gfxapi.h"
#include "kbdapi.h"
#include "ptrapi.h"
#include "tsmapi.h"

/***************************************************************************
IMS_StartAck () - Starts up checking for a happening
 ***************************************************************************/
void IMS_StartAck( void ) {
    KBD_Clear();

    mouse_b1_ack = FALSE;
    mouse_b2_ack = FALSE;
    mouse_b3_ack = FALSE;
    kbd_ack = FALSE;
}

/***************************************************************************
IMS_CheckAck () - Tells if somthing has happend since last IMS_StartAck
 ***************************************************************************/
BOOL IMS_CheckAck( void ) {
    int rval = FALSE;

    if ( mouse_b1_ack ) {
        rval = TRUE;
    }

    if ( mouse_b2_ack ) {
        rval = TRUE;
    }

    if ( kbd_ack ) {
        rval = TRUE;
    }

    return rval;
}

/***************************************************************************
IMS_IsAck() - Returns TRUE if ptr button or key pressed
 ***************************************************************************/
BOOL IMS_IsAck( void ) {
    if ( KBD_LASTSCAN ) {
        KBD_LASTSCAN = FALSE;
        return TRUE;
    }

    return ( mouseb1 || mouseb2 || mouseb3 ) ? TRUE : FALSE;
}

/***************************************************************************
IMS_WaitAck() - Waits for a pointer button or key press
 ***************************************************************************/
void IMS_WaitAck( void ) {
    IMS_StartAck();

    while ( !IMS_CheckAck() ) {
    }

    IMS_StartAck();
}

/***************************************************************************
IMS_WaitTimed() - Wait for aprox secs
 ***************************************************************************/
int // RETURN: keypress (lastscan)
IMS_WaitTimed(
    int secs // INPUT : seconds to wait
) {
    volatile int hold;
    int i;
    int rval;

    KBD_LASTSCAN = SC_NONE;
    rval = KBD_LASTSCAN;

    IMS_StartAck();

    while ( secs > 0 ) {
        for ( i = 0; i < 55; i++ ) {
            hold = framecount;
            while ( framecount == hold )
                ;

            if ( IMS_CheckAck() ) {
                rval = 1;
                goto end_func;
            }
        }
        secs--;
    }

end_func:
    i = 100;
    while ( IMS_IsAck() ) {
        i--;
    }

    IMS_StartAck();

    return rval;
}
