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

    mouse_b1_ack = false;
    mouse_b2_ack = false;
    mouse_b3_ack = false;
    kbd_ack = false;
}

/***************************************************************************
IMS_CheckAck () - Tells if somthing has happend since last IMS_StartAck
 ***************************************************************************/
bool IMS_CheckAck( void ) {
    return mouse_b1_ack|| mouse_b2_ack || kbd_ack;
}

/***************************************************************************
IMS_IsAck() - Returns true if ptr button or key pressed
 ***************************************************************************/
bool IMS_IsAck( void ) {
    if ( KBD_LASTSCAN ) {
        KBD_LASTSCAN = false;
        return true;
    }

    return mouseb1 || mouseb2 || mouseb3;
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
