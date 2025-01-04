#include <string.h>

#include "raptor.h"

#define MAX_DERBIS 15

typedef struct DERB_S {
    struct DERB_S* prev; // LINK LIST PREV
    struct DERB_S* next; // LINK LIST NEXT
    INT dir; // DIRECTION 0 - 7
    INT x;
    INT y;
} DERBS;

PRIVATE ESHOT derbs[MAX_DERBIS];

PRIVATE ESHOT first_derb;
PRIVATE ESHOT last_derb;
PRIVATE ESHOT* free_derb;

/***************************************************************************
DERB_Clear () - Clears out ESHOT Linklist
 ***************************************************************************/
VOID DERB_Clear( VOID ) {
    INT loop;

    eshotnum = 0;

    first_derb.prev = NUL;
    first_derb.next = &last_derb;

    last_derb.prev = &first_derb;
    last_derb.next = NUL;

    free_derb = eshots;

    memset( eshots, 0, sizeof( eshots ) );

    for ( loop = 0; loop < MAX_ESHOT - 1; loop++ ) {
        eshots[loop].next = &eshots[loop + 1];
    }
}

/*-------------------------------------------------------------------------*
DERB_Get () - gets a Free ESHOT OBJECT from linklist
 *-------------------------------------------------------------------------*/
PRIVATE ESHOT* DERB_Get( VOID ) {
    ESHOT* pNew;

    if ( !free_derb ) {
        return NUL;
    }

    eshotnum++;
    if ( eshothigh < eshotnum ) {
        eshothigh = eshotnum;
    }

    pNew = free_derb;
    free_derb = free_derb->next;

    memset( pNew, 0, sizeof( ESHOT ) );

    pNew->next = &last_derb;
    pNew->prev = last_derb.prev;
    last_derb.prev = pNew;
    pNew->prev->next = pNew;

    return pNew;
}

/*-------------------------------------------------------------------------*
DERB_Remove () - Removes SHOT OBJECT from linklist
 *-------------------------------------------------------------------------*/
PRIVATE ESHOT* DERB_Remove( ESHOT* sh ) {
    ESHOT* next;

    eshotnum--;

    next = sh->prev;

    sh->next->prev = sh->prev;
    sh->prev->next = sh->next;

    memset( sh, 0, sizeof( ESHOT ) );

    sh->next = free_derb;

    free_derb = sh;

    return next;
}
