/****************************************************************************
 * TILE  - 32 by 32 Tile Drawing
 *----------------------------------------------------------------------------
 * Copyright (C) 1993  Scott Hostynski All Rights Reserved
 *----------------------------------------------------------------------------
 *
 * Created by:  Scott Host
 * Date:        July, 1993
 *
 * CONTENTS: tile.c tile_a.asm
 *
 * EXTERN MODULES - GFXAPI
 *
 *---------------------------------------------------------------------------*/

#ifndef TILE_H
#define TILE_H

#include "../gfx/types.h"

typedef enum { TILE_NONE, TILE_HIT, TILE_DESTROY, TILE_BONUS } T_HIT;

typedef struct {
    DWORD item;
    int x;
    int y;
    int mapspot;
} TILESPOT;

typedef struct TDELAY_S {
    struct TDELAY_S* prev;
    struct TDELAY_S* next;
    int frames;
    int mapspot;
    DWORD item;
    TILESPOT* ts;
} TILEDELAY;

/***************************************************************************
TILE_Draw () - Draws 32 by 32 TILE Clips on y only
 ***************************************************************************/
void TILE_Put(
    BYTE* inpic, // INPUT : pointer to GFX_PIC ( norm )
    int x, // INPUT : x position
    int y // INPUT : y position
);

/***************************************************************************
TILE_Init () - Sets Up A level for Displaying
 ***************************************************************************/
void TILE_Init( void );

/***************************************************************************
TILE_FreeLevel () - Free tile level
 ***************************************************************************/
void TILE_FreeLevel( void );

/***************************************************************************
TILE_CacheLevel () - Cache tiles in current level
 ***************************************************************************/
void TILE_CacheLevel( void );

/***************************************************************************
TILE_DamageAll () - Damages All tiles on screen
 ***************************************************************************/
void TILE_DamageAll( void );

/***************************************************************************
TILE_Think () - Does Position Calculations for tiles
 ***************************************************************************/
void TILE_Think( void );

/***************************************************************************
TILE_Display () - Displays Tiles
 ***************************************************************************/
void TILE_Display( void );

/***************************************************************************
TILE_IsHit () - Checks to see if a shot hits an explodable tile
 ***************************************************************************/
T_HIT // RETURNS : see T_HIT in tile.h
TILE_IsHit(
    int damage, // INPUT : damage to tile
    int x, // INOUT : x screen pos, out tile x
    int y // INOUT : y screen pos, out tile y
);

/***************************************************************************
TILE_Bomb () - Checks to see if a BOMB hits an explodable tile
 ***************************************************************************/
BOOL // RETURNS : TRUE = Tile Hit
TILE_Bomb(
    int damage, // INPUT : damage to tile
    int x, // INOUT : x screen pos, out tile x
    int y // INOUT : y screen pos, out tile y
);

/***************************************************************************
TILE_Explode () - Sets the Tile to show explosion tile
 ***************************************************************************/
void TILE_Explode(
    TILESPOT* ts, // INPUT : tilespot of explosion
    int delay // INPUT : frames to delay
);

extern DWORD startflat[4];
extern int tilepos;
extern int tileyoff;
extern BOOL scroll_flag;
extern int g_mapleft;

#endif
