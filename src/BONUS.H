#ifndef BONUS_H
#define BONUS_H

#include "../gfx/types.h"
#include "objects.h"

#define BONUS_WIDTH  16
#define BONUS_HEIGHT 16

#define MAX_BONUS    12
#define MAX_MONEY    ( MAX_BONUS - 3 )

typedef struct BONUS_S {
    struct BONUS_S* prev; // LINK LIST PREV
    struct BONUS_S* next; // LINK LIST NEXT
    DWORD item; // GLB item
    int curframe; // CURRENT ANIM FRAME
    int curglow; // CURRENT GLOW FRAME
    int x; // X POSITION
    int y; // Y POSITION
    int bx; // BONUS X POSITION
    int by; // BONUS Y POSITION
    int gx; // GLOW X POSITION
    int gy; // GLOW Y POSITION
    int pos; // POS IN CIRCLE
    BOOL dflag; // DONEFLAG
    int countdown; // COUNTDOWN TO REMOVE
    OBJ_TYPE type; // OBJECT TYPE
    OBJ_LIB* lib; // POINTER TO OBJECT LIBRARY
} BONUS;

/***************************************************************************
BONUS_Clear () - Clears out All bonuses
 ***************************************************************************/
void BONUS_Clear( void );

/***************************************************************************
BONUS_Init () - Sets up Bonus stuff
 ***************************************************************************/
void BONUS_Init( void );

/***************************************************************************
BONUS_Add () - Adds A BONUS to Game so player can Try to pick it up
 ***************************************************************************/
void BONUS_Add(
    OBJ_TYPE type, // INPUT : OBJECT TYPE
    int x, // INPUT : X POSITION
    int y // INPUT : Y POSITION
);

/***************************************************************************
BONUS_Think () - Does all BONUS Thinking
 ***************************************************************************/
void BONUS_Think( void );

/***************************************************************************
BONUS_Display () - Displays Active Bonuses in game
 ***************************************************************************/
void BONUS_Display( void );

#endif
