/****************************************************************************
 * VMEMapi  - Virtual Memory Routines
 *---------------------------------------------------------------------------*/

#ifndef _VMEMAPI
#define _VMEMAPI

#include "types.h"

typedef struct {
    BYTE* obj;
    DWORD age;
    //	DWORD		id;
} VM_OWNER;

/*************************************************************************
 VM_InitMemory - Assign memory block to be used as virtual memory.
               - Can be called multiple times to add additional memory.
 *************************************************************************/
void VM_InitMemory(
    BYTE* memory, // INPUT : Memory to be added to the pool
    DWORD size // INPUT : Size of memory
);

/*************************************************************************
 VM_Malloc - Allocates a block of memory - swapping out other blocks if
             needed.
 *************************************************************************/
void* VM_Malloc(
    DWORD size, // INPUT : Size of object
    VM_OWNER* owner, // INPUT : Owner Structure, NULL=Locked
    bool discard // INPUT : Discard memory to satisfy request.
);

/*************************************************************************
 * VM_Touch - touch a peice of memory to keep track of most recently used.
 *************************************************************************/
void VM_Touch(
    VM_OWNER* owner // INPUT : Owner of memory to touch.
);

/*************************************************************************
 VM_Free - frees a block of memory allocated by VM_Malloc
 *************************************************************************/
void VM_Free(
    void* mem // INPUT : Memory Object to Free
);

/*************************************************************************
 VM_Lock - Locks a block of memory from being swapped out.
 *************************************************************************/
void VM_Lock(
    void* mem // INPUT : Memory Object to Free
);

/*************************************************************************
 VM_Unlock - Unlocks a block of memory allowing it to be swapped out.
 *************************************************************************/
void VM_Unlock(
    void* mem, // INPUT : Memory Object to Free
    VM_OWNER* owner // INPUT : Owner Structure, NULL=Locked
);

/*************************************************************************
 VM_GetCoreInfo - Get information on core resource
 *************************************************************************/
void VM_GetCoreInfo(
    DWORD* largest, // OUTPUT: Largest block free
    DWORD* totalfree, // OUTPUT: Total amount free
    DWORD* totallocked, // OUTPUT: Total amount locked
    DWORD* totalused, // OUTPUT: Total amount used
    DWORD* discarded // OUTPUT: Number of items discarded
);

#endif
