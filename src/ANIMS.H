#ifndef ANIM_H
#define ANIM_H

#include "../gfx/types.h"
#include "enemy.h"

typedef enum { A_NORM, A_MOVEUP, A_MOVEDOWN } ANIM_DIR;

#define A_LARGE_GROUND_EXPLO1 0
#define A_SMALL_GROUND_EXPLO  1
#define A_PERSON              2
#define A_PLATOON             3

#define A_LARGE_AIR_EXPLO     4
#define A_MED_AIR_EXPLO       5
#define A_SMALL_AIR_EXPLO     6
#define A_MED_AIR_EXPLO2      7
#define A_ENERGY_AIR_EXPLO    8
#define A_LASER_BLAST         9

#define A_SMALL_SMOKE         10
#define A_SMALL_SMOKE_DOWN    11
#define A_SMALL_SMOKE_UP      12
#define A_ENERGY_GRAB_HIT     13
#define A_BLUE_SPARK          14
#define A_ORANGE_SPARK        15
#define A_PLAYER_SHOOT        16
#define A_GROUND_FLARE        17
#define A_GROUND_SPARKLE      18
#define A_ENERGY_GRAB         19
#define A_SUPER_SHIELD        20

/***************************************************************************
ANIMS_Clear () - Clears out All ANIM Objects
 ***************************************************************************/
void ANIMS_Clear( void );

/***************************************************************************
ANIMS_Init () Initializes ANIM STuff
 ***************************************************************************/
void ANIMS_Init( void );

/***************************************************************************
ANIMS_Register () - Register a ANIM for USE with this stuff
 ***************************************************************************/
int ANIMS_Register(
    DWORD item, // INPUT : lumpnum of first frame
    int numframes, // INPUT : number of frames
    BOOL groundflag, // INPUT : on the ground = TRUE
    BOOL playerflag, // INPUT : follow player movements
    BOOL transparent, // INPUT : Transparent ( LIGHT )
    ANIM_DIR adir // INPUT : Anim Direction
);

/***************************************************************************
ANIMS_CachePics() - Cache registered anim pics
 ***************************************************************************/
void ANIMS_CachePics( void );

/***************************************************************************
ANIMS_FreePics() - Free Up Anims Used
 ***************************************************************************/
void ANIMS_FreePics( void );

/***************************************************************************
ANIMS_StartAnim () - Start An ANIM Playing
 ***************************************************************************/
void ANIMS_StartAnim(
    int handle, // INPUT : ANIM handle
    int x, // INPUT : x position
    int y // INPUT : y position
);

/***************************************************************************
ANIMS_StartGAnim () - Start An ANIM Playing with groundflag == TRUE
 ***************************************************************************/
void ANIMS_StartGAnim(
    int handle, // INPUT : ANIM handle
    int x, // INPUT : x position
    int y // INPUT : y position
);

/***************************************************************************
ANIMS_Think () - Does all thinking for ANIMS
 ***************************************************************************/
void ANIMS_Think( void );

/***************************************************************************
ANIMS_DisplayGround () - Displays All Active ANIMS on the Ground
 ***************************************************************************/
void ANIMS_DisplayGround( void );

/***************************************************************************
ANIMS_DisplaySky () - Displays All Active ANIMS in SKY
 ***************************************************************************/
void ANIMS_DisplaySky( void );

/***************************************************************************
ANIMS_DisplayHigh () - Displays All Active ANIMS in ABOVE PLAYER
 ***************************************************************************/
void ANIMS_DisplayHigh( void );

/***************************************************************************
ANIMS_StartAAnim () - Start An ANIM Playing with groundflag == GROUND
 ***************************************************************************/
void ANIMS_StartAAnim(
    int handle, // INPUT : ANIM handle
    int x, // INPUT : x position
    int y // INPUT : y position
);

/***************************************************************************
ANIMS_StartEAnim () - Start An ANIM Playing locked onto ENEMY
 ***************************************************************************/
void ANIMS_StartEAnim(
    SPRITE_SHIP* en, // INPUT : pointer to ENEMY
    int handle, // INPUT : ANIM handle
    int x, // INPUT : x position
    int y // INPUT : y position
);

#endif
