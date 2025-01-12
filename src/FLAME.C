
#include "public.h"

#include "windows.h"

#include "flame.h"

#define MAX_SHADES 8

PRIVATE BYTE* stable[MAX_SHADES];
PRIVATE BYTE stmem[MAX_SHADES][512];

/***************************************************************************
FLAME_Init () - Inits Flame Tables and stuff
 ***************************************************************************/
void FLAME_Init( void ) {
    int i;

    for ( i = 0; i < MAX_SHADES; i++ ) {
        stable[i] = stmem[i];

        stable[i] = (BYTE*) ( (int) stable[i] + 255 & ~0xff );

        GFX_MakeLightTable( palette, stable[i], ( MAX_SHADES - i ) * 2 );
    }
}

/***************************************************************************
FLAME_InitShades () - Inits shading stuff
 ***************************************************************************/
void FLAME_InitShades( void ) {
    int i;

    for ( i = 0; i < MAX_SHADES; i++ ) {
        GFX_MakeLightTable( palette, stable[i], ( MAX_SHADES - i ) * 2 );
    }
}

/***************************************************************************
FLAME_Up () - Shows Flame shooting upward
 ***************************************************************************/
void FLAME_Up(
    int ix, // INPUT : x position
    int iy, // INPUT : y position
    int width, // INPUT : width of shade
    int frame // INPUT : frame
) {
    int height[2] = { 5, 10 };
    BYTE* outbuf;
    int y;
    int i;
    DWORD curs;
    DWORD addx;
    int num;

    if ( opt_detail < 1 ) {
        return;
    }

    frame = frame % 2;

    iy -= height[frame] - 1;

    if ( GFX_ClipLines( NULL, &ix, &iy, &width, &height[frame] ) ) {
        y = iy;

        addx = ( MAX_SHADES << 16 ) / height[frame];
        curs = addx * ( height[frame] - 1 );

        for ( i = 0; i < height[frame]; i++ ) {
            outbuf = displaybuffer + ix + ylookup[y];

            y++;

            num = curs >> 16;

            if ( num >= 8 ) {
                EXIT_Error( "flame > 8 %u", curs >> 16 );
            }

            if ( num < 0 ) {
                EXIT_Error( "flame < 0" );
            }

            GFX_Shade( outbuf, width, stable[num] );

            curs -= addx;
        }
    }
}

/***************************************************************************
FLAME_Down () - Shows Flame shooting downward
 ***************************************************************************/
void FLAME_Down(
    int ix, // INPUT : x position
    int iy, // INPUT : y position
    int width, // INPUT : width of shade
    int frame // INPUT : frame
) {
    int height[2] = { 8, 12 };
    BYTE* outbuf;
    int y;
    int i;
    DWORD curs;
    DWORD addx;

    frame = frame % 2;

    if ( GFX_ClipLines( NULL, &ix, &iy, &width, &height[frame] ) ) {
        y = iy;

        curs = 0;
        addx = ( MAX_SHADES << 16 ) / height[frame];

        for ( i = 0; i < height[frame]; i++ ) {
            outbuf = displaybuffer + ix + ylookup[y];

            y++;

            GFX_Shade( outbuf, width, stable[( curs >> 16 )] );

            curs += addx;
        }
    }
}
