/****************************************************************************
 * PTRapi  - Cursor routines
 *---------------------------------------------------------------------------*/

#ifndef PTRAPI_H
#define PTRAPI_H

#include "types.h"

typedef enum { P_AUTO, P_MOUSE, P_JOYSTICK } PTRTYPE;
// TODO inline those below, dropping `volatile` in the process
#define JOY_X       ( (volatile) cur_mx )
#define JOY_Y       ( (volatile) cur_my )
#define PTR_X       ( (volatile) cur_mx )
#define PTR_Y       ( (volatile) cur_my )
#define PTR_B1      ( (volatile) mouseb1 )
#define PTR_B2      ( (volatile) mouseb2 )
#define PTR_B3      ( (volatile) mouseb3 )
#define PTR_TM1     ( (volatile) joy2b1 )
#define PTR_TM2     ( (volatile) joy2b2 )
#define PTR_PRESENT ( mousepresent + joypresent )

extern BOOL mouse_b1_ack;
extern BOOL mouse_b2_ack;
extern BOOL mouse_b3_ack;
extern BOOL drawcursor;
extern BOOL joypresent;
extern BOOL mousepresent;
extern INT joy2b1;
extern INT joy2b2;
extern INT mouseb1;
extern INT mouseb2;
extern INT mouseb3;
extern INT cursorx;
extern INT cursory;
extern INT cur_mx;
extern INT cur_my;
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
    INT x, // INPUT : x position
    INT y // INPUT : y position
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
