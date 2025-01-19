#ifndef PUBLIC_H
#define PUBLIC_H

// commonly used includes
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../gfx/common.h"
#include "../gfx/exitapi.h"
#include "../gfx/swdapi.h"
#include "../gfx/glbapi.h"

// local includes
#include "map.h"

#define MAX_OBJS 20

typedef enum { K_OK, K_NEXTFRAME, K_SKIPALL, K_EXITDOS } KEYOPTS;

typedef enum { R_YESNO, R_QUANTITY } ASKBUY;

typedef struct {
    BYTE b1;
    BYTE b2;
    BYTE b3;
    BYTE b4;
    short px;
    short py;
    short playerpic;
    short fil;
} RECORD;

// note: this is read from save files; sizeof(PLAYEROBJ) = 17 * 4 = 68
typedef struct {
    char name[20];
    char callsign[12];
    int id_pic;
    DWORD score; // MONEY/POINTS
    int sweapon; // CURENT SPECIAL WEAPON
    int cur_game; // CURRENT GAME
    int game_wave[3]; // CURRENT WAVE
    int numobjs; // NUMBER OF OBJECTS TO FOLLOW
    int diff[4]; // DIFFICULTY LEVEL
    uint32_t trainflag; // bool
    uint32_t fintrain; // bool
} PLAYEROBJ;

#define END_DURATION     ( 20 * 3 )
#define END_EXPLODE      ( 24 )
#define END_FLYOFF       ( 20 * 2 )

#define DIFF_0           0 // TRAINING MODE
#define DIFF_1           1 // EASY MODE
#define DIFF_2           2 // NORMAL MODE
#define DIFF_3           3 // HARD MODE

#define SHIELD_COLOR_RUN 9
#define SHIELD_LOW       10

#define PLAYERWIDTH      32
#define PLAYERHEIGHT     32
#define PLAYERMINX       5
#define PLAYERMAXX       314
#define PLAYERINITX      ( 160 - ( PLAYERWIDTH / 2 ) )
#define PLAYERINITY      160

#define MINPLAYERY       0
#define MAXPLAYERY       160

#define GREEN            100
#define RED              34
#define YELLOW           50

#define MAP_ABORT        -1
#define MAP_NEXT         -2

// #define MAX_BADGUYS  300
#define MAX_ONSCREEN     30

#define MAX_SHIELD 100

/***************************************************************************
   InitMobj() - Inits an object to be moved
 ***************************************************************************/
void InitMobj(
    MOVEOBJ* cur // INPUT : pointer to MOVEOBJ
);

/***************************************************************************
   MoveMobj() - gets next postion for an Object
 ***************************************************************************/
void MoveMobj(
    MOVEOBJ* cur // INPUT : pointer to MOVEOBJ
);

/***************************************************************************
   MoveSobj() - gets next postion for an Object at speed
 ***************************************************************************/
int MoveSobj(
    MOVEOBJ* cur, // INPUT : pointer to MOVEOBJ
    int speed // INPUT : speed to plot at
);

/***************************************************************************
AskExit () - Opens Windows and Askes if USer wants 2 quit
 ***************************************************************************/
void AskExit( void );

/***************************************************************************
Do_Game () - The main game thing this is it dude
 ***************************************************************************/
bool // true=Aborted, false = timeout
    Do_Game( void );

inline int random( int max_excl ) {
    return rand() % max_excl;
}

extern PLAYEROBJ plr;
extern int choose_special[];
extern int cur_special;
extern int player_cx;
extern int player_cy;
extern int playerx;
extern int playery;
extern int playerbasepic;
extern int playerpic;
extern int numbadguys;
extern int cur_game;
extern int game_wave[4];
extern MAZELEVEL* ml;

extern BYTE* palette;
extern int o_engine[];
extern int o_gun1[];
extern int o_gun2[];
extern int o_gun3[];
extern FLATS* flatlib[];
extern bool startfadeflag;
extern BYTE* patch1;
extern BYTE* patch2;
extern bool end_wave;
extern int gl_cnt;
extern bool testflag;
extern int startendwave;
extern bool reg_flag;
extern bool lowmem_flag;
extern int curplr_diff;
extern int bday_num;
extern int demo_flag;

#endif
