

#include "../gfx/imsapi.h"
#include "../gfx/kbdapi.h"

#include "public.h"

#include "fx.h"

#include "movie.h"

PRIVATE int back_patch = EMPTY;

/*************************************************************************
MOVIE_BPatch() Plays Sound FX in background for one anim
 *************************************************************************/
void MOVIE_BPatch( int soundfx ) {
    back_patch = soundfx;
    SND_Patch( back_patch, 127 );
}

/*************************************************************************
 MOVIE_ShowFrame () - Shows an animation frame
 *************************************************************************/
void MOVIE_ShowFrame(
    BYTE* inpic // INPUT : pointer to animpic
) {
    uint8_t* src = inpic;
    uint8_t* dst;
    uint16_t offset, count;

    if ( src == NULL ) {
        return;
    }

    while ( *( (uint16_t*) src ) != 0x0000 ) {
        src += 4;
        offset = *( (uint16_t*) src );
        src += 2;
        count = *( (uint16_t*) src );
        src += 2;

        dst = displaybuffer + offset;

        memcpy( dst, src, count );
        src += count;
    }

    GFX_MarkUpdate( 0, 0, 320, 200 );
}

/*************************************************************************
 MOVIE_Play () - Playes an Animation
 *************************************************************************/
int MOVIE_Play(
    FRAME* frame, // INPUT : pointer to array of frame defs
    int numplay, // INPUT : number of times to play
    BYTE* palette // INPUT : pointer to palette
) {
    bool flag = true;
    FRAME* curfld;
    BYTE* pic;
    int opt = K_OK;
    BYTE fill;
    int hold;
    int i;

    memset( displaybuffer, 0, 64000 );

    curfld = frame;

    IMS_StartAck();

    for ( ;; ) {
        pic = GLB_GetItem( curfld->item );

        if ( flag ) {
            fill = *pic;
            memset( displaybuffer, fill, 64000 );
            pic++;
        }

        switch ( curfld->startf ) {
            default:
                MOVIE_ShowFrame( pic );
                GFX_WaitUpdate( curfld->framerate );
                break;

            case M_FADEOUT:
                GFX_FadeOut( curfld->red, curfld->green, curfld->blue, curfld->startsteps );

            case M_ERASE:
                memset( displaybuffer, 0, 64000 );
                GFX_MarkUpdate( 0, 0, 320, 200 );
                GFX_DisplayUpdate();
                break;

            case M_FADEIN:
                if ( flag ) {
                    GFX_FadeOut( 0, 0, 0, 2 );
                    flag = false;
                }
                MOVIE_ShowFrame( pic );
                GFX_WaitUpdate( curfld->framerate );
                GFX_FadeIn( palette, curfld->startsteps );
                break;
        }

        if ( curfld->holdframe ) {
            for ( i = 0; i < curfld->holdframe; i++ ) {
                hold = framecount;
                while ( framecount == hold ) {
                    if ( back_patch != EMPTY ) {
                        if ( !SND_IsPatchPlaying( back_patch ) ) {
                            SND_Patch( back_patch, 127 );
                        }
                    }
                }
            }
        } else {
            if ( back_patch != EMPTY ) {
                if ( !SND_IsPatchPlaying( back_patch ) ) {
                    SND_Patch( back_patch, 127 );
                }
            }
        }

        if ( curfld->soundfx != EMPTY ) {
            SND_Patch( curfld->soundfx, curfld->fx_xpos );
        }

        switch ( curfld->endf ) {
            default:
                break;

            case M_ERASE:
                memset( displaybuffer, 0, 64000 );
                GFX_MarkUpdate( 0, 0, 320, 200 );
                GFX_DisplayUpdate();
                break;

            case M_FADEOUT:
                GFX_FadeOut( curfld->red, curfld->green, curfld->blue, curfld->endsteps );
                break;

            case M_FADEIN:
                GFX_FadeIn( palette, curfld->startsteps );
                break;

            case M_PALETTE:
                GFX_SetPalette( palette, 0 );
                break;
        }

        GLB_FreeItem( curfld->item );

        if ( curfld->numframes == 0 ) {
            numplay--;
            if ( numplay == 0 ) {
                break;
            }
            memset( displaybuffer, 0, 64000 );
            curfld = frame;
            flag = true;
            continue;
        }

        if ( IMS_CheckAck() ) {
            IMS_StartAck();
            opt = K_SKIPALL;
            break;
        }

        flag = false;

        curfld++;
    }

    if ( opt != K_OK ) {
        KBD_Clear();
        GFX_FadeOut( 0, 0, 0, 16 );
        memset( displaybuffer, 0, 64000 );
        GFX_MarkUpdate( 0, 0, 320, 200 );
        GFX_DisplayUpdate();
        GFX_SetPalette( palette, 0 );
    }

    if ( back_patch != EMPTY ) {
        SND_StopPatch( back_patch );
        back_patch = EMPTY;
    }

    return opt;
}
