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

#ifndef _TYPES_H
#include <types.h>
#endif

#ifndef _IMSAPI
#define _IMSAPI

/***************************************************************************
IMS_StartAck () - Starts up checking for a happening
 ***************************************************************************/
void IMS_StartAck( void );

/***************************************************************************
IMS_CheckAck () - Tells if somthing has happend since last IMS_StartAck
 ***************************************************************************/
BOOL IMS_CheckAck( void );

/***************************************************************************
IMS_IsAck() - Returns TRUE if ptr button or key pressed
 ***************************************************************************/
BOOL IMS_IsAck( void );

/***************************************************************************
IMS_WaitAck() - Waits for a pointer button or key press
 ***************************************************************************/
void IMS_WaitAck( void );

/***************************************************************************
IMS_WaitTimed() - Wait for aprox secs
 ***************************************************************************/
INT // RETURN: keypress (lastscan)
IMS_WaitTimed(
    INT secs // INPUT : seconds to wait
);

#endif
