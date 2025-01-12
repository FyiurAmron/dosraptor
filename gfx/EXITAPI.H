/****************************************************************************
 * EXITapi  - Program Exiting routines
 *---------------------------------------------------------------------------*/

#ifndef EXITAPI_H
#define EXITAPI_H

#include "types.h"

#undef ASSERT

#ifdef __386__
#ifdef DEBUG
void EXIT_Assert( char*, char*, unsigned );
#define ASSERT( exp ) ( ( exp ) ? (void) 0 : EXIT_Assert( #exp, __FILE__, __LINE__ ) )
#else
#define ASSERT( __ignore ) ( (void) 0 )
#endif

#else
#ifdef DEBUG
#define ASSERT( exp ) ( ( exp ) ? (void) 0 : EXIT_Assert( #exp, __FILE__, __LINE__ ) )
#else
#define ASSERT( exp ) ( (void) 0 )
#endif
#endif

/***************************************************************************
   EXIT_Install() Sets User routine to shut down systems
 ***************************************************************************/
void EXIT_Install( void ( *SD )( int ) // INPUT : pointer to function
);

/***************************************************************************
   EXIT_Error() - Terminate Program in Error with message
 ***************************************************************************/
void EXIT_Error(
    char* instr, // INPUT : message string ot format
    ... // INPUT : args for instr
);

/***************************************************************************
   EXIT_Clean() - Terminate Program Clean
 ***************************************************************************/
void EXIT_Clean( void );

#endif
