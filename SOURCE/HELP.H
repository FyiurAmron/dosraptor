#ifndef HELP_H
#define HELP_H

#include "../gfx/types.h"

/***************************************************************************
HELP_Init() - inits the help stuff
 ***************************************************************************/
VOID HELP_Init( VOID );

/***************************************************************************
HELP_Win() - Displays the help window at the specified page
 ***************************************************************************/
VOID HELP_Win(
    CHAR* strpage // INPUT : GLB string item
);

#endif
