#ifndef ENEMY_H
#define ENEMY_H

#include "../gfx/types.h"

#include "map.h"

typedef enum { E_FORWARD, E_BACKWARD } EDIR;

typedef enum { KAMI_FLY, KAMI_CHASE, KAMI_END } KAMI;

typedef enum { GANIM_DONE, GANIM_OFF, GANIM_ON } GANIM;

typedef enum { MULTI_OFF, MULTI_START, MULTI_END } MULTI;

typedef struct SPRITE_SHIP_S {
    struct SPRITE_SHIP_S* prev;
    struct SPRITE_SHIP_S* next;
    DWORD item; // GLB item of current frame
    SPRITE* lib; // SPRITE LIB POS
    int sx; // START X
    int sy; // START Y
    int x; // CENTER X POS;
    int y; // Y POS
    int x2; // WIDTH
    int y2; // HEIGHT
    int width; // WIDTH
    int height; // HEIGHT
    int hlx; // WIDTH/2
    int hly; // HEIGHT/2
    int movepos; // CUR POSITION IN FLIGHT (X/Y)
    int shootagain; // * COUNT DOWN TO SHOOT
    int shootcount; // * COUNT TO SHOOT
    int shootflag; // * COUNTER TO SPACE SHOTS
    int angleflag;
    int dirflag; // * FLIGHT DIRECTION FLAG
    int hits; // *
    BOOL groundflag; // *
    BOOL doneflag;
    MOVEOBJ move;
    int countdown;
    DWORD curframe;
    int eframe;
    int num_frames;
    BOOL anim_on;
    EDIR edir;
    int kami;
    int frame_rate;
    BOOL shoot_on;
    BOOL shoot_disable;
    MULTI multi;
    int speed;
    int suckagain;
} SPRITE_SHIP;

#define E_NUM_DIFF    4

#define E_SECRET_1    0
#define E_SECRET_2    1
#define E_SECRET_3    2
#define E_EASY_LEVEL  3
#define E_MED_LEVEL   4
#define E_HARD_LEVEL  5
#define E_TRAIN_LEVEL 6

#define EB_SECRET_1   1
#define EB_SECRET_2   2
#define EB_SECRET_3   4
#define EB_EASY_LEVEL 8
#define EB_MED_LEVEL  16
#define EB_HARD_LEVEL 32
#define EB_NOT_USED   64

/***************************************************************************
ENEMY_InitSprites () - Initializes Enmy Sprites
 ***************************************************************************/
void ENEMY_InitSprites( void );

/***************************************************************************
ENEMY_FreeSprites () - Free Memory Used by Sprites use in last level
 ***************************************************************************/
void ENEMY_FreeSprites( void );

/***************************************************************************
ENEMY_LoadSprites() -
 ***************************************************************************/
void ENEMY_LoadSprites( void );

/***************************************************************************
ENEMY_LoadLib () - Loads and Locks spritelib's
 ***************************************************************************/
void ENEMY_LoadLib( void );

/***************************************************************************
ENEMY_Clear()
 ***************************************************************************/
void ENEMY_Clear( void );

/***************************************************************************
ENEMY_GetRandom () - Returns a random ship thats visable
 ***************************************************************************/
SPRITE_SHIP* ENEMY_GetRandom( void );

/***************************************************************************
ENEMY_GetRandomAir () - Returns a random ship thats visable
 ***************************************************************************/
SPRITE_SHIP* ENEMY_GetRandomAir( void );

/***************************************************************************
ENEMY_DamageAll () - Tests to see if hit occured at x/y and applys damage
 ***************************************************************************/
BOOL ENEMY_DamageAll(
    int x, // INPUT : x position
    int y, // INPUT : y position
    int damage // INPUT : damage
);

/***************************************************************************
ENEMY_DamageGround () - Tests to see if hit occured at x/y and applys damage
 ***************************************************************************/
BOOL ENEMY_DamageGround(
    int x, // INPUT : x position
    int y, // INPUT : y position
    int damage // INPUT : damage
);

/***************************************************************************
ENEMY_DamageAir () - Tests to see if hit occured at x/y and applys damage
 ***************************************************************************/
BOOL ENEMY_DamageAir(
    int x, // INPUT : x position
    int y, // INPUT : y position
    int damage // INPUT : damage
);

/***************************************************************************
ENEMY_DamageEnergy () - Tests to see if hit occured at x/y and applys damage
 ***************************************************************************/
SPRITE_SHIP* ENEMY_DamageEnergy(
    int x, // INPUT : x position
    int y, // INPUT : y position
    int damage // INPUT : damage
);

/***************************************************************************
ENEMY_Think() - Does all thinking for enemy ships ( ground/air )
 ***************************************************************************/
void ENEMY_Think( void );

/***************************************************************************
ENEMY_DisplayGround () - Displays Ground ENEMY pics
 ***************************************************************************/
void ENEMY_DisplayGround( void );

/***************************************************************************
ENEMY_DisplaySky () - Displays AIR ENEMY SHIPS
 ***************************************************************************/
void ENEMY_DisplaySky( void );

/***************************************************************************
ENEMY_GetBaseDamage() - Gets Base Ship damage
 ***************************************************************************/
int ENEMY_GetBaseDamage( void );

extern CSPRITE* csprite;
extern int numships;
extern DWORD cur_diff;

#endif
