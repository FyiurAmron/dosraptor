#ifndef _TYPES_H
#define _TYPES_H

#include <stdint.h>
#include <stdbool.h>

typedef enum { FALSE, TRUE } BOOL; // TODO remove this after src migration is complete

#define PRIVATE static
#define PUBLIC

typedef uint8_t  BYTE;
typedef uint16_t WORD;
typedef uint32_t DWORD;

#endif
