
#include "gfxapi_a.h"
#include "gfxapi.h"

#include <string.h>

#define SCREENWIDTH  320
#define SCREENHEIGHT 200 // unused here

extern uint8_t* gfx_inmem;

extern int ud_x, ud_y, ud_lx, ud_ly;
extern int gfx_xp, gfx_yp, gfx_lx, gfx_ly;
extern int gfx_imga;

extern BOOL update_start;

void GFX_Shade( uint8_t* dst, int maxlen, const uint8_t* shading_table ) {
    for ( int i = 0; i < maxlen; i++ ) {
        *dst = shading_table[*dst];
        dst++;
    }
}

void GFX_ShadeSprite( uint8_t* dst, const uint8_t* src, const uint8_t* shading_table ) {
    int offset;
    while ( ( offset = ( (GFX_SPRITE*) src )->offset ) != -1 ) {
        const int length = ( (GFX_SPRITE*) src )->length;
        src += sizeof( GFX_SPRITE );

        for ( int i = 0; i < length; i++ ) {
            dst[offset] = shading_table[dst[offset]];
            offset++;
        }
        src += length;
    }
}

void GFX_DrawSprite( uint8_t* dst, const uint8_t* src ) {
    int offset;
    while ( ( offset = ( (GFX_SPRITE*) src )->offset ) != -1 ) {
        const int length = ( (GFX_SPRITE*) src )->length;
        src += sizeof( GFX_SPRITE );
        memcpy( dst + offset, src, length );
        src += length;
    }
}

void GFX_DrawChar( uint8_t* dst, const uint8_t* src, int lx, int ly, int add_x_src, uint8_t color ) {
    int add_x_dst = SCREENWIDTH - lx;

    for ( int y = 0; y < ly; y++ ) {
        for ( int x = 0; x < lx; x++ ) {
            uint8_t pixel = *src++;
            if ( pixel != 0 ) {
                *dst = pixel + color;
            }
            dst++;
        }
        src += add_x_src;
        dst += add_x_dst;
    }
}

inline uint8_t* GFX_GetDst() {
    return displaybuffer + gfx_xp + gfx_yp * SCREENWIDTH;
}

void GFX_PutPic( void ) {
    uint8_t* src = gfx_inmem;
    uint8_t* dst = GFX_GetDst();

    if ( gfx_lx < 1 ) {
        return;
    }

    for ( int y = 0; y < gfx_ly; y++ ) {
        memcpy( dst, src, gfx_lx );
        dst += SCREENWIDTH;
        src += gfx_lx + gfx_imga; // ca. h->width
    }
}

void GFX_PutMaskPic( void ) {
    uint8_t* src = gfx_inmem;
    uint8_t* dst = GFX_GetDst();

    if ( gfx_lx < 1 ) {
        return;
    }

    for ( int y = 0; y < gfx_ly; y++ ) {
        for ( int x = 0; x < gfx_lx; x++ ) {
            int al = src[x];
            if ( al != 0 ) {
                dst[x] = al;
            }
        }

        dst += SCREENWIDTH;
        src += gfx_imga; // no gfx_lx added here?!
    }
}

void GFX_DisplayScreen( void ) {
    int offset = ud_x + ud_y * SCREENWIDTH;
    uint8_t* src = displaybuffer + offset;
    uint8_t* dst = displayscreen + offset;

    if ( ud_lx < 1 ) {
        return;
    }

    if ( ud_lx == SCREENWIDTH ) {
        memcpy( dst, src, SCREENWIDTH * ud_ly );
    } else {
        for ( int y = 0; y < ud_ly; y++ ) {
            memcpy( dst, src, ud_lx );
            dst += SCREENWIDTH;
            src += SCREENWIDTH;
        }
    }

    update_start = FALSE;
}
