
#include <string.h>

#include "../gfx/types.h"

#include "tile_a.h"

extern uint8_t* displaybuffer;
extern uint8_t* displayscreen;

extern uint8_t* tilepic;
extern uint8_t* tilestart;

extern int32_t g_mapleft;

void TILE_Draw( int max_y ) {
    uint8_t* src = tilepic;
    uint8_t* dst = tilestart;

    int i;
    for ( i = 0; i < max_y; i++ ) {
        memcpy( dst, src, TILEWIDTH );
        src += TILEWIDTH;
        dst += SCREENWIDTH;
    }
}

void TILE_DisplayScreen( int srcOffset, int dstOffset, int cols  ) {
    uint8_t* src = displaybuffer + srcOffset;
    uint8_t* dst = displayscreen + dstOffset;

    int i;
    for ( i = 0; i < SCREENHEIGHT; i++ ) {
        memcpy( dst, src, cols );
        dst += SCREENWIDTH;
        src += SCREENWIDTH;
    }
}
