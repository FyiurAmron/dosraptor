/****************************************************************************
 * GLBapi  - File loading routines
 *---------------------------------------------------------------------------*/

#include <stddef.h>

#ifndef GLBAPI_H
#define GLBAPI_H

#include "types.h"

typedef struct {
    DWORD opt; // option (encode on/off)
    DWORD offset; // offset into file
    DWORD filesize; // filesize
    char name[16]; // text name ( end with null )
} KEYFILE;

typedef struct {
    uint16_t itemnum;
    uint16_t filenum;
} ITEMS;

#define MAX_GLB_FILES 0x000F

#define GLB_NORMAL    0
#define GLB_ENCODED   1

/*************************************************************************
   GLB_InitSystem() - Starts up .GLB file system
 *************************************************************************/
int GLB_InitSystem(
    char*,
    int innum, // INPUT: MAX .GLB FILES TO LOOK FOR
    char* iprefix // INPUT: FILENAME PREFIX ( NULL for "FILE" )
);

/*************************************************************************
 GLB_Load() Loads a file to a pointer from a .GLB file
 *************************************************************************/
DWORD // RETURN : size of item read
GLB_Load(
    BYTE* inmem, // INPUT: memory pointer or NULL
    int filenum, // INPUT: file number
    int itemnum // INPUT: item in file number
);

/***************************************************************************
 GLB_LockItem () - Keeps Item From being discarded.
 ***************************************************************************/
void* GLB_LockItem( DWORD handle );

/***************************************************************************
 GLB_UnlockItem () - Allows item to be discarded from memory.
 ***************************************************************************/
void GLB_UnlockItem( DWORD handle );

/***************************************************************************
 GLB_CacheItem() - Loads item into memory only if free core exists.
 ***************************************************************************/
void* GLB_CacheItem( DWORD handle );

/***************************************************************************
 GLB_GetItem() - Loads and allocates memory for a .GLB item
 ***************************************************************************/
BYTE* GLB_GetItem(
    DWORD handle // INPUT : handle of item
);

/***************************************************************************
 GLB_ItemSize() - Returns Size of Item
 ***************************************************************************/
int // RETURN: sizeof ITEM
GLB_ItemSize(
    DWORD handle // INPUT : handle of item
);

/***************************************************************************
 GLB_IsLabel () - tests to see if ID is a label or an Item
 ***************************************************************************/
bool // RETURN: true = Label
GLB_IsLabel(
    DWORD handle // INPUT : handle of item
);

/***************************************************************************
 GLB_ReadItem() - Loads Item into user memory for a .GLB item
 ***************************************************************************/
void GLB_ReadItem(
    DWORD handle, // INPUT : handle of item
    BYTE* mem // INPUT : pointer to memory
);

/***************************************************************************
   GLB_GetItemID () - Gets Item ID from the text name
 ***************************************************************************/
DWORD // RETURN:
GLB_GetItemID(
    char* in_name // INPUT : pointer to text name
);

/***************************************************************************
 GLB_GetPtr() - Returns a pointer to item ( handle )
 ***************************************************************************/
BYTE* // RETURN: pointer to item
GLB_GetPtr(
    int handle // INPUT : handle of item
);

/***************************************************************************
 GLB_FreeItem() - Frees memory for items and places items < MAX SIZE
 ***************************************************************************/
void GLB_FreeItem(
    DWORD handle // INPUT : handle of item
);

/***************************************************************************
 GLB_FreeAll() - Frees All memory used by GLB items
 ***************************************************************************/
void GLB_FreeAll( void );

/***************************************************************************
 GLB_ItemSize() - Returns Size of Item
 ***************************************************************************/
int // RETURN: sizeof ITEM
GLB_ItemSize(
    DWORD handle // INPUT : handle of item
);

/***************************************************************************
   GLB_ReadFile () reads in a normal file
 ***************************************************************************/
int // RETURN: size of record
GLB_ReadFile(
    char* name, // INPUT : filename
    void* buffer // OUTPUT: pointer to buffer or NULL
);

/***************************************************************************
   GLB_SaveFile () saves buffer to a normal file ( filename )
 ***************************************************************************/
void GLB_SaveFile(
    char* name, // INPUT : filename
    void* buffer, // INPUT : pointer to buffer
    DWORD length // INPUT : length of buffer
);

/*************************************************************************
   GLB_UseVM - Use virtual memory functions for heap managment.
 *************************************************************************/
void GLB_UseVM( void );

#endif
