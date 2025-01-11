#ifndef _TYPES_H
#define _TYPES_H

#include <stdlib.h>

typedef enum { FALSE, TRUE } BOOL;

#define PRIVATE static
#define PUBLIC
#define EMPTY       ~0
#define SIZE( a )  ( sizeof( a ) / sizeof( ( a )[0] ) )

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef char CHAR;
typedef short SHORT;
typedef int INT;

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;

#endif
