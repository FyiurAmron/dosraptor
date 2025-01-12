/****************************************************************************
 * PTRapi  - Cursor routines
 *---------------------------------------------------------------------------*/

#ifndef PTRAPI_H
#define PTRAPI_H

#include "types.h"
#include "common.h"

typedef enum { P_AUTO, P_MOUSE, P_JOYSTICK } PTRTYPE;

extern BOOL mouse_b1_ack;
extern BOOL mouse_b2_ack;
extern BOOL mouse_b3_ack;
extern BOOL drawcursor;
extern BOOL joypresent;
extern BOOL mousepresent;
extern int joy2b1;
extern int joy2b2;
extern int mouseb1;
extern int mouseb2;
extern int mouseb3;
extern int cursorx;
extern int cursory;
extern int cur_mx;
extern int cur_my;
extern BOOL ptr_init_flag;

/*========================================================================
  PTR_UpdateCursor() - Updates Mouse Cursor - should be called by intterupt
  ========================================================================*/
void PTR_UpdateCursor( void );

/*==========================================================================
  PTR_FrameHook() - Mouse framehook Function
 ==========================================================================*/
void PTR_FrameHook( void ( *update )( void ) // INPUT : pointer to function
);

/*************************************************************************
PTR_JoyReset() -
 *************************************************************************/
void PTR_JoyReset( void );

/*************************************************************************
  PTR_JoyHandler () - Handles Joystick Input
 *************************************************************************/
void PTR_JoyHandler( void );

/***************************************************************************
   PTR_SetPic () - Sets up a new cursor picture with hotspot
 ***************************************************************************/
void PTR_SetPic(
    BYTE* newp // INPUT : pointer to new Cursor picture
);

/***************************************************************************
 PTR_SetBoundsHook() - Sets User function to OK or change mouse x,y values
 ***************************************************************************/
void // RETURN: none
    PTR_SetBoundsHook( void ( *func )( void ) // INPUT : pointer to function
    );

/***************************************************************************
 PTR_SetCursorHook() - Sets User function to call from mouse handler
 ***************************************************************************/
void // RETURN: none
    PTR_SetCursorHook( void ( *hook )( void ) // INPUT : pointer to function
    );

/***************************************************************************
   PTR_SetUpdateFlag () - Sets cursor to be update next cycle
 ***************************************************************************/
void PTR_SetUpdateFlag( void );

/***************************************************************************
   PTR_DrawCursor () - Turns Cursor Drawing to ON/OFF ( TRUE/FALSE )
 ***************************************************************************/
void PTR_DrawCursor(
    BOOL flag // INPUT: TRUE/FALSE
);

/***************************************************************************
 PTR_SetPos() - Sets Cursor Position
 ***************************************************************************/
void // RETURN: none
PTR_SetPos(
    int x, // INPUT : x position
    int y // INPUT : y position
);

/***************************************************************************
PTR_Pause() - Pauses/ Starts PTR routines after already initing
 ***************************************************************************/
void PTR_Pause(
    BOOL flag // INPUT : TRUE / FALSE
);

/***************************************************************************
 PTR_Init() - Inits Mouse Driver and sets mouse handler function
 ***************************************************************************/
BOOL // RETURN true = Installed, false  = No mouse
PTR_Init(
    PTRTYPE type // INPUT : Pointer Type to Use
);

/***************************************************************************
 PTR_End() - End Cursor system
 ***************************************************************************/
void PTR_End( void );

// ptrapi_a

void PTR_ReadJoyStick( void );
void PTR_Save( int max_y );
void PTR_Erase( void );
void PTR_ClipErase( void );
void PTR_Draw( void );

#endif
