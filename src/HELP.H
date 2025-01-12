#ifndef HELP_H
#define HELP_H

#include "../gfx/types.h"

/***************************************************************************
HELP_Init() - inits the help stuff
 ***************************************************************************/
void HELP_Init( void );

/***************************************************************************
HELP_Win() - Displays the help window at the specified page
 ***************************************************************************/
void HELP_Win(
    CHAR* strpage // INPUT : GLB string item
);

#endif
