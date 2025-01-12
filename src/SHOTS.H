#ifndef SHOTS_H
#define SHOTS_H

#include "../gfx/types.h"
#include "objects.h"

#define MAX_SHOTS 70

typedef enum { S_ALL, S_AIR, S_GROUND, S_GRALL, S_GTILE, S_SUCK } HIT_TYPE;

typedef enum { S_SHOOT, S_LINE, S_BEAM } BEAM_TYPE;

typedef struct {
    DWORD lumpnum; // LUMPNUM OF PIC(S)
    BYTE* pic[10]; // POINTER TO PIC
    bool shadow; // SHADOW ON/OFF
    OBJ_TYPE type; // SHOT TYPE
    int hits; // DAMAGE SHOT WILL DO
    int speed; // START SPEED
    int maxspeed; // MAXIMUM SPEED
    int startframe; // START ANIM ON FRAME
    int numframes; // NUMBER OF FRAMES OF ANIM
    int shoot_rate; // SHOOT EVERY (N) FRAMES
    int cur_shoot; // HOLDS FRAME COUNT DOWN
    bool delayflag; // DEALAYED ANIM START
    bool smoke; // SMOKE TRAIL
    bool use_plot; // true = PLOT X,Y ( SLOWER )
    bool move_flag; // DOES SHOT NEED MOVEMENT
    HIT_TYPE ht; // SHOT HIT TYPE ( AIR/GROUND )
    bool fplrx; // FOLLOW PLAYER X POS
    bool fplry; // FOLLOW PLAYER Y POS
    bool meffect; // TRACK MOVE TO FIND HIT ?
    BEAM_TYPE beam; // BEAM WEAPON TYPE
    GFX_PIC* h;
    int hlx;
    int hly;
} SHOT_LIB;

typedef struct SHOTS_S {
    struct SHOTS_S* prev; // Link List Prev
    struct SHOTS_S* next; // Link List Next
    BYTE* pic; // pointer to cur frame pic
    int x; // cur shot center x
    int y; // cur shot center y
    MOVEOBJ move; // MOVE stuff
    int speed; // cur speed
    int curframe; // current anim frame
    bool doneflag; // shot done = true
    bool delayflag; // DEALAYED ANIM START
    int startx; // PLAYER X OFFSET;
    int starty; // PLAYER Y OFFSET;
    SHOT_LIB* lib;
    int cnt;
} SHOTS;

/***************************************************************************
SHOTS_Clear () * Clears out SHOTS Linklist
 ***************************************************************************/
void SHOTS_Clear( void );

/***************************************************************************
SHOTS_Init () - Inits SHOTS system and clears link list
 ***************************************************************************/
void SHOTS_Init( void );

/***************************************************************************
SHOTS_PlayerShoot() - Shoots the specified weapon
 ***************************************************************************/
bool SHOTS_PlayerShoot(
    OBJ_TYPE type // INPUT : OBJECT TYPE
);

/***************************************************************************
SHOTS_Think () - Does All Thinking for shot system
 ***************************************************************************/
void SHOTS_Think( void );

/***************************************************************************
SHOTS_Display () - Displays All active Shots
 ***************************************************************************/
void SHOTS_Display( void );

#endif
