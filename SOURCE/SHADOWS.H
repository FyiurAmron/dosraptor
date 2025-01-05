#ifndef SHADOWS_H
#define SHADOWS_H

#include "../gfx/types.h"

extern INT num_shadows;
extern INT num_gshadows;

/***************************************************************************
SHADOW_Draw () - Draws AIR shadows in 3D perspective
 ***************************************************************************/
void SHADOW_Draw(
    BYTE* pic, // INPUT : pointer to sprite data
    INT x, // INPUT : x position of sprite
    INT y // INPUT : y position of sprite
);

/***************************************************************************
SHADOW_Init() - Allocate memory and set 3D view
 ***************************************************************************/
void SHADOW_Init( void );

/***************************************************************************
SHADOW_MakeShades() - Make shade tables
 ***************************************************************************/
void SHADOW_MakeShades( void );

/***************************************************************************
SHADOW_Add() - Add a Air ship shadow
 ***************************************************************************/
void SHADOW_Add(
    DWORD item, // INPUT : GLB item
    INT x, // INPUT : x position
    INT y // INPUT : y position
);

/***************************************************************************
SHADOW_GAdd() - Adds Ground shadow
 ***************************************************************************/
void SHADOW_GAdd(
    DWORD item, // INPUT : GLB item
    INT x, // INPUT : x position
    INT y // INPUT : y position
);

/***************************************************************************
SHADOW_DisplaySky () - Display Sky Shadows
 ***************************************************************************/
void SHADOW_DisplaySky( void );

/***************************************************************************
SHADOW_DisplayGround() - Display Ground Shadows
 ***************************************************************************/
void SHADOW_DisplayGround( void );

void SHADOW_Clear( void );

#define SHADOW_Clear() ( num_shadows = num_gshadows = 0 )

#endif
