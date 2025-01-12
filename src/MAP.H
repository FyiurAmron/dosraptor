#ifndef MAP_H
#define MAP_H

#include "../gfx/types.h"

#define MAX_FLIGHT    30

#define MAP_ROWS      150
#define MAP_ONSCREEN  8
#define MAP_COLS      9
#define MAP_BLOCKSIZE 32
#define MAP_BOTTOM    200 - 18
#define MAP_TOP       2
#define MAP_LEFT      16
#define MAP_RIGHT     ( 320 - 16 )
#define MAP_SIZE      ( MAP_ROWS * MAP_COLS )

#define MAX_TILES     2048
#define MAX_GUNS      24

#define ENGINE_COLOR  254
#define SHOT_COLOR    255

typedef struct {
    int x;
    int y;
    int x2;
    int y2;
    int delx;
    int dely;
    int addx;
    int addy;
    int maxloop;
    int err;
    BOOL done;
} MOVEOBJ;

// ENEMY SHIPS
// SHOOT
// MOVE
// CRASH INTO PLAYER
// ANIMATED
// TAKE DAMAGE
// SKY/GROUND

typedef enum {
    F_REPEAT,
    F_LINEAR,
    F_KAMI,
    F_GROUND,
    F_GROUNDLEFT,
    F_GROUNDRIGHT,
} FLIGHT_TYPE;

typedef struct {
    int linkflat;
    short bonus;
    short bounty;
} FLATS;

typedef struct {
    short flats;
    short fgame;
} MAZEDATA;

typedef struct {
    DWORD sizerec;
    DWORD spriteoff;
    int numsprites;
    MAZEDATA map[MAP_SIZE];
} MAZELEVEL;

typedef enum { GANIM_NORM, GANIM_SHOOT, GANIM_MULTI } ANIMTYPE;

typedef enum {
    EXP_AIRSMALL1, // 0
    EXP_AIRMED, // 1 NORMAL AIR    ( norm )
    EXP_AIRLARGE, // 2 bigger that 32 by 32
    EXP_GRDSMALL, // 3 smaller than 32 by 32
    EXP_GRDMED, // 4 NORMAL GROUND ( norm )
    EXP_GRDLARGE, // 5 bigger than 32 by 32
    EXP_BOSS, // 6 BIG BOSS SHIP ( AIR )
    EXP_PERSON, // 7 ONE LITTLE PERSON BLWING UP
    EXP_ENERGY, // 8 ENERGY SHIP BLOWING UP
    EXP_PLATOON, // 9 ONE LITTLE PERSON BLWING UP
    EXP_AIRSMALL2 // 10
} EXP_TYPE;

typedef struct {
    char iname[16]; // ITEM NAME
    DWORD item; // * GLB ITEM #
    int bonus; // BONUS # ( -1 == NONE )
    EXP_TYPE exptype; // EXPLOSION TYPE
    int shotspace; // SLOWDOWN SPEED
    BOOL ground; // IS ON GROUND
    BOOL suck; // CAN SUCK WEAPON AFFECT
    int frame_rate; // FRAME RATE
    int num_frames; // NUM FRAMES
    int countdown; // COUNT DOWN TO START ANIM
    int rewind; // FRAMES TO REWIND
    ANIMTYPE animtype; // FREE SPACE FOR LATER USE
    BOOL shadow; // USE SHADOW ( TRUE/FALSE )
    int bossflag; // SHOULD THIS BE CONSIDERED A BOSS
    int hits; // HIT POINTS
    int money; // $$ AMOUNT WHEN KILLED
    int shootstart; // SHOOT START OFFSET
    int shootcnt; // HOW MANY TO SHOOT
    int shootframe; // FRAME RATE TO SHOOT
    int movespeed; // MOVEMENT SPEED
    int numflight; // NUMBER OF FLIGHT POSITIONS
    int repos; // REPEAT TO POSITION
    FLIGHT_TYPE flighttype; // FLIGHT TYPE
    int numguns; // NUMBER OF GUNS
    int numengs; // NUMBER OF ENGINES
    int sfx; // SFX # TO PLAY
    int song; // SONG # TO PLAY
    short shoot_type[MAX_GUNS]; // ENEMY SHOOT TYPE
    short engx[MAX_GUNS]; // X POS ENGINE FLAME
    short engy[MAX_GUNS]; // Y POS ENGINE FLAME
    short englx[MAX_GUNS]; // WIDTH OF ENGINE FLAME
    short shootx[MAX_GUNS]; // X POS SHOOT FROM
    short shooty[MAX_GUNS]; // Y POS SHOOT FROM
    short flightx[MAX_FLIGHT]; // FLIGHT X POS
    short flighty[MAX_FLIGHT]; // FLIGHT Y POS
} SPRITE;

typedef struct {
    int link;
    int slib;
    int x;
    int y;
    int game;
    DWORD level;
} CSPRITE;

#endif
