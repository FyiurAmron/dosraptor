
#include "public.h"

#include "windows.h"

#include "shadows.h"

typedef struct {
    DWORD item;
    int x;
    int y;
} SHADOW;

#define MAX_SHADOWS  50
#define MAX_GSHADOWS 25

SHADOW gshads[MAX_GSHADOWS];
SHADOW shads[MAX_SHADOWS];
int num_shadows = 0;
int num_gshadows = 0;
PRIVATE BYTE* sdtable;
PRIVATE BYTE sdtablemem[516];

#define MAXZ 1280;

/***************************************************************************
SHADOW_Draw () - Draws AIR shadows in 3D perspective
 ***************************************************************************/
void SHADOW_Draw(
    BYTE* pic, // INPUT : pointer to sprite data
    int x, // INPUT : x position of sprite
    int y // INPUT : y position of sprite
) {

    GFX_PIC* h = (GFX_PIC*) pic;
    int lx;
    int ly;
    GFX_SPRITE* ah;
    int sx;
    int sy;
    int ox;
    int oy;
    int x2;
    int y2;
    int oldy;
    int oldsy;
    bool drawflag;

    x -= 10;
    y += 20;

    oldy = oldsy = EMPTY;

    G3D_x = x;
    G3D_y = y;
    G3D_z = MAXZ;
    GFX_3DPoint();
    ox = G3D_screenx;
    oy = G3D_screeny;

    G3D_x = ox + h->width - 1;
    G3D_y = oy + h->height - 1;
    G3D_z = MAXZ;
    GFX_3DPoint();
    lx = G3D_screenx - ox + 1;
    ly = G3D_screeny - oy + 1;

    if ( !GFX_ClipLines( NULL, &ox, &oy, &lx, &ly ) ) {
        return;
    }

    pic += sizeof( GFX_PIC );

    ah = (GFX_SPRITE*) pic;

    while ( ah->offset != EMPTY ) {
        pic += sizeof( GFX_SPRITE );

        ox = ah->x + x;
        oy = ah->y + y;

        x2 = ox + ah->length - 1;
        y2 = oy + 1;

        G3D_x = ox;
        G3D_y = oy;
        G3D_z = MAXZ;
        GFX_3DPoint();
        sx = G3D_screenx;
        sy = G3D_screeny;

        G3D_x = x2;
        G3D_y = y2;
        G3D_z = MAXZ;
        GFX_3DPoint();
        lx = G3D_screenx - sx + 1;

        if ( sy > SCREENHEIGHT ) {
            break;
        }

        drawflag = true;

        if ( ah->y != oldy && oldsy == sy ) {
            drawflag = false;
        }

        if ( drawflag ) {
            ly = 1;

            if ( GFX_ClipLines( NULL, &sx, &sy, &lx, &ly ) ) {
                GFX_Shade( displaybuffer + sx + ylookup[sy], lx, sdtable );
            }

            oldy = ah->y;
        }

        oldsy = sy;

        pic += ah->length;

        ah = (GFX_SPRITE*) pic;
    }
}

/***************************************************************************
SHADOW_Init() - Allocate memory and set 3D view
 ***************************************************************************/
void SHADOW_Init( void ) {
    sdtable = sdtablemem;
    sdtable = (BYTE*) ( (int) sdtable + 255 & ~0xff );

    GFX_3D_SetView( 160, 100, 1000 );
}

/***************************************************************************
SHADOW_MakeShades() - Make shade tables
 ***************************************************************************/
void SHADOW_MakeShades( void ) {
    GFX_MakeLightTable( (BYTE*) palette, sdtable, -6 );
}

/***************************************************************************
SHADOW_Add() - Add a Air ship shadow
 ***************************************************************************/
void SHADOW_Add(
    DWORD item, // INPUT : GLB item
    int x, // INPUT : x position
    int y // INPUT : y position
) {
    SHADOW* cur = &shads[num_shadows];

    if ( num_shadows >= MAX_SHADOWS ) {
        return;
    }

    cur->item = item;
    cur->x = x;
    cur->y = y;

    num_shadows++;
}

/***************************************************************************
SHADOW_GAdd() - Adds Ground shadow
 ***************************************************************************/
void SHADOW_GAdd(
    DWORD item, // INPUT : GLB item
    int x, // INPUT : x position
    int y // INPUT : y position
) {
    SHADOW* cur = &gshads[num_gshadows];

    if ( num_gshadows >= MAX_GSHADOWS ) {
        return;
    }

    cur->item = item;
    cur->x = x - 3;
    cur->y = y + 4;

    num_gshadows++;
}

/***************************************************************************
SHADOW_DisplaySky () - Display Sky Shadows
 ***************************************************************************/
void SHADOW_DisplaySky( void ) {
    SHADOW* cur = shads;
    BYTE* pic;

    if ( opt_detail < 1 ) {
        return;
    }

    while ( num_shadows-- ) {
        pic = GLB_GetItem( cur->item );
        SHADOW_Draw( pic, cur->x, cur->y );
        cur++;
    }

    num_shadows = 0;
}

/***************************************************************************
SHADOW_DisplayGround() - Display Ground Shadows
 ***************************************************************************/
void SHADOW_DisplayGround( void ) {
    SHADOW* cur = gshads;
    BYTE* pic;

    if ( opt_detail < 1 ) {
        return;
    }

    while ( num_gshadows-- ) {
        pic = GLB_GetItem( cur->item );
        GFX_ShadeShape( DARK, pic, cur->x, cur->y );
        cur++;
    }

    num_gshadows = 0;
}
