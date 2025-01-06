#ifndef _TYPES_H
#define _TYPES_H

typedef enum { FALSE, TRUE } BOOL;

#define PRIVATE static
#define PUBLIC
#define EMPTY       ~0
#define ASIZE( a )  ( sizeof( a ) / sizeof( ( a )[0] ) )
#define FMUL32( a ) ( ( a ) << 5 )

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef char CHAR;
typedef short SHORT;
typedef unsigned short USHORT;
typedef int INT;
typedef unsigned int UINT;

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;

int random( int );
#define random( x ) ( rand() % x )

#endif
