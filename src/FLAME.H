#ifndef FLAME_H
#define FLAME_H

#include "../gfx/types.h"

/***************************************************************************
FLAME_Init () - Inits Flame Tables and stuff
 ***************************************************************************/
void FLAME_Init( void );

/***************************************************************************
FLAME_InitShades () - Inits shading stuff
 ***************************************************************************/
void FLAME_InitShades( void );

/***************************************************************************
FLAME_Up () - Shows Flame shooting upward
 ***************************************************************************/
void FLAME_Up(
    INT ix, // INPUT : x position
    INT iy, // INPUT : y position
    INT width, // INPUT : width of shade
    INT frame // INPUT : frame
);

/***************************************************************************
FLAME_Down () - Shows Flame shooting downward
 ***************************************************************************/
void FLAME_Down(
    INT ix, // INPUT : x position
    INT iy, // INPUT : y position
    INT width, // INPUT : width of shade
    INT frame // INPUT : frame
);

#endif
