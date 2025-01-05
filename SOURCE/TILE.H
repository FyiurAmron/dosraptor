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
    INT x;
    INT y;
    INT mapspot;
} TILESPOT;

typedef struct TDELAY_S {
    struct TDELAY_S* prev;
    struct TDELAY_S* next;
    INT frames;
    INT mapspot;
    DWORD item;
    TILESPOT* ts;
} TILEDELAY;

/***************************************************************************
TILE_Draw () - Draws 32 by 32 TILE Clips on y only
 ***************************************************************************/
void TILE_Put(
    BYTE* inpic, // INPUT : pointer to GFX_PIC ( norm )
    INT x, // INPUT : x position
    INT y // INPUT : y position
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
    INT damage, // INPUT : damage to tile
    INT x, // INOUT : x screen pos, out tile x
    INT y // INOUT : y screen pos, out tile y
);

/***************************************************************************
TILE_Bomb () - Checks to see if a BOMB hits an explodable tile
 ***************************************************************************/
BOOL // RETURNS : TRUE = Tile Hit
TILE_Bomb(
    INT damage, // INPUT : damage to tile
    INT x, // INOUT : x screen pos, out tile x
    INT y // INOUT : y screen pos, out tile y
);

/***************************************************************************
TILE_Explode () - Sets the Tile to show explosion tile
 ***************************************************************************/
void TILE_Explode(
    TILESPOT* ts, // INPUT : tilespot of explosion
    INT delay // INPUT : frames to delay
);

#pragma aux TILE_Draw "_*" modify[EAX EBX ECX EDX ESI EDI];
void TILE_Draw( void );

#pragma aux TILE_ClipDraw "_*" modify[EAX EBX ECX EDX ESI EDI];
void TILE_ClipDraw( void );

#pragma aux TILE_DisplayScreen "_*" modify[EAX EBX ECX EDX ESI EDI];
void TILE_DisplayScreen( void );

#pragma aux TILE_ShakeScreen "_*" modify[EAX EBX ECX EDX ESI EDI];
void TILE_ShakeScreen( void );

extern DWORD startflat[4];
extern INT tilepos;
extern INT tileyoff;
extern BOOL scroll_flag;
extern INT g_mapleft;

#endif
