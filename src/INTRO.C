

#include "../gfx/imsapi.h"
#include "../gfx/kbdapi.h"

#include "public.h"

#include "inc/file0000.inc"
#include "inc/file0001.inc"
#include "inc/file0002.inc"
#include "inc/file0003.inc"

#include "fx.h"
#include "movie.h"
#include "windows.h"

#include "intro.h"

FRAME frm[40];

/***************************************************************************
INTRO_City () - Shows City with planes flying into it
 ***************************************************************************/
bool INTRO_City( void ) {
    int i;
    int maxframes = 30;
    int framecnt = maxframes - 1;
    FRAME* cur = frm;

    for ( i = 0; i < maxframes; i++, cur++ ) {
        cur->holdframe = 0;
        cur->opt = M_ANIM;
        cur->framerate = 8;
        cur->numframes = framecnt--;
        cur->item = CHASE_AGX + i;
        cur->startf = M_NORM;
        cur->startsteps = 0;
        cur->endf = M_NORM;
        cur->endsteps = 0;
        cur->red = 0;
        cur->green = 0;
        cur->blue = 0;
        cur->songid = EMPTY;
        cur->songopt = S_PLAY;
        cur->songstep = 0;
        cur->soundfx = EMPTY;
        cur->fx_vol = 0;
        cur->fx_xpos = 127;

        if ( i == 4 ) {
            cur->soundfx = FX_FLYBY;
            cur->fx_xpos = 210;
        }

        if ( i == 9 ) {
            cur->soundfx = FX_FLYBY;
            cur->fx_xpos = 100;
        }
    }

    cur = frm;

    cur->startf = M_FADEIN;
    cur->startsteps = 128;

    if ( MOVIE_Play( frm, 1, palette ) == K_SKIPALL ) {
        return true;
    }

    SND_StopPatches();

    return false;
}

/***************************************************************************
INTRO_Side1 () - Show Side OF Player ship going thru city
 ***************************************************************************/
bool INTRO_Side1( void ) {
    int i;
    int maxframes = 20;
    int framecnt = maxframes - 1;
    FRAME* cur = frm;

    MOVIE_BPatch( FX_JETSND );

    for ( i = 0; i < maxframes; i++, cur++ ) {
        cur->holdframe = 0;
        cur->opt = M_ANIM;
        cur->framerate = 18;
        cur->numframes = framecnt--;
        cur->item = SHIPSD1_AGX + i;
        cur->startf = M_NORM;
        cur->startsteps = 0;
        cur->endf = M_NORM;
        cur->endsteps = 0;
        cur->red = 0;
        cur->green = 0;
        cur->blue = 0;
        cur->songid = EMPTY;
        cur->songopt = S_PLAY;
        cur->songstep = 0;
        cur->fx_vol = 127;
        cur->fx_xpos = 127;
        cur->soundfx = EMPTY;
    }

    return MOVIE_Play( frm, 2, palette ) == K_SKIPALL;
}

/***************************************************************************
INTRO_Pilot () - Shows Pilots Face with lights moving thru
 ***************************************************************************/
bool INTRO_Pilot( void ) {
    int i;
    int maxframes = 21;
    int framecnt = maxframes - 1;
    FRAME* cur = frm;

    MOVIE_BPatch( FX_IJETSND );

    for ( i = 0; i < maxframes; i++, cur++ ) {
        cur->holdframe = 0;
        cur->opt = M_ANIM;
        cur->framerate = 10;
        cur->numframes = framecnt--;
        cur->item = PILOT_AGX + i;
        cur->startf = M_NORM;
        cur->startsteps = 0;
        cur->endf = M_NORM;
        cur->endsteps = 0;
        cur->red = 0;
        cur->green = 0;
        cur->blue = 0;
        cur->songid = EMPTY;
        cur->songopt = S_PLAY;
        cur->songstep = 0;
        cur->soundfx = EMPTY;
        cur->fx_vol = 127;
        cur->fx_xpos = 127;
    }

    return MOVIE_Play( frm, 1, palette ) == K_SKIPALL;
}

/***************************************************************************
INTRO_Explosion () - Bad Guy Blowing UP
 ***************************************************************************/
bool INTRO_Explosion( void ) {
    int i;
    int maxframes = 22;
    int framecnt = maxframes - 1;
    FRAME* cur = frm;

    MOVIE_BPatch( FX_EJETSND );

    for ( i = 0; i < maxframes; i++, cur++ ) {
        cur->holdframe = 0;
        cur->opt = M_ANIM;
        cur->framerate = 12;
        cur->numframes = framecnt--;
        cur->item = EXPLO_AGX + i;
        cur->startf = M_NORM;
        cur->startsteps = 0;
        cur->endf = M_NORM;
        cur->endsteps = 0;
        cur->red = 0;
        cur->green = 0;
        cur->blue = 0;
        cur->songid = EMPTY;
        cur->songopt = S_PLAY;
        cur->songstep = 0;
        cur->soundfx = EMPTY;
        cur->fx_xpos = 127;
        cur->fx_vol = 127;

        if ( i >= 2 && i < 10 ) {
            cur->soundfx = FX_INTROHIT;
            cur->fx_xpos = 110 + random( 40 );
        }

        if ( i >= 8 ) {
            if ( i & 1 ) {
                cur->soundfx = FX_AIREXPLO;
            }
        }
    }

    cur--;
    cur->soundfx = FX_AIREXPLO;
    cur->endf = M_FADEOUT;
    cur->red = 63;
    cur->green = 28;
    cur->blue = 3;
    cur->endsteps = 60;

    return MOVIE_Play( frm, 1, palette ) == K_SKIPALL;
}

/***************************************************************************
INTRO_Side2 () - Plaer Side flying thru city Shooting
 ***************************************************************************/
bool INTRO_Side2( void ) {
    int i;
    int maxframes = 20;
    int framecnt = maxframes - 1;
    FRAME* cur = frm;
    int opt;

    MOVIE_BPatch( FX_JETSND );

    for ( i = 0; i < maxframes; i++, cur++ ) {
        cur->holdframe = 0;
        cur->opt = M_ANIM;
        cur->framerate = 18;
        cur->numframes = framecnt--;
        cur->item = SHIPSD1_AGX + i;
        cur->startf = M_NORM;
        cur->startsteps = 0;
        cur->endf = M_NORM;
        cur->endsteps = 0;
        cur->red = 0;
        cur->green = 0;
        cur->blue = 0;
        cur->songid = EMPTY;
        cur->songopt = S_PLAY;
        cur->songstep = 0;
        cur->soundfx = EMPTY;
        cur->fx_vol = 50;
        cur->fx_xpos = 127;
        cur->soundfx = EMPTY;
    }

    opt = MOVIE_Play( frm, 1, palette );

    if ( opt == K_SKIPALL ) {
        return true;
    }

    if ( opt == K_NEXTFRAME ) {
        return false;
    }

    framecnt = maxframes - 1;
    cur = frm;

    for ( i = 0; i < maxframes; i++, cur++ ) {
        cur->holdframe = 0;
        cur->opt = M_ANIM;
        cur->framerate = 18;
        cur->numframes = framecnt--;
        cur->item = SHIPSD2_AGX + i;
        cur->startf = M_NORM;
        cur->startsteps = 0;
        cur->endf = M_NORM;
        cur->endsteps = 0;
        cur->red = 0;
        cur->green = 0;
        cur->blue = 0;
        cur->songid = EMPTY;
        cur->songopt = S_PLAY;
        cur->songstep = 0;
        cur->soundfx = EMPTY;
        cur->fx_xpos = 127;
        if ( i > 1 ) {
            cur->soundfx = FX_INTROGUN;
        }
    }

    if ( MOVIE_Play( frm, 1, palette ) == K_SKIPALL ) {
        return true;
    }

    SND_StopPatches();

    return false;
}

/***************************************************************************
INTRO_Base() - Base Landing at sunset
 ***************************************************************************/
bool INTRO_Base( void ) {
    int i;
    int maxframes = 30;
    int framecnt = maxframes - 1;
    FRAME* cur = frm;

    if ( !reg_flag ) {
        return true;
    }

    for ( i = 0; i < maxframes; i++ ) {
        cur->holdframe = 0;
        cur->opt = M_ANIM;
        cur->framerate = 10;
        cur->numframes = framecnt--;
        cur->item = BASE_AGX + i;
        cur->startf = M_NORM;
        cur->startsteps = 0;
        cur->endf = M_NORM;
        cur->endsteps = 0;
        cur->red = 0;
        cur->green = 0;
        cur->blue = 0;
        cur->songid = EMPTY;
        cur->songopt = S_PLAY;
        cur->songstep = 0;
        cur->soundfx = EMPTY;
        cur++;
    }

    cur = frm;

    cur->startf = M_FADEIN;
    cur->startsteps = 128;

    return MOVIE_Play( frm, 1, palette ) == K_SKIPALL;
}

/***************************************************************************
INTRO_Landing () - Ship Landing on Base
 ***************************************************************************/
bool INTRO_Landing( void ) {
    int i;
    int maxframes = 33;
    int framecnt = maxframes - 1;
    FRAME* cur = frm;

    for ( i = 0; i < maxframes; i++ ) {
        cur->holdframe = 0;
        cur->opt = M_ANIM;
        cur->framerate = 10;
        cur->numframes = framecnt--;
        cur->item = LANDING_AGX + i;
        cur->startf = M_NORM;
        cur->startsteps = 0;
        cur->endf = M_NORM;
        cur->endsteps = 0;
        cur->red = 0;
        cur->green = 0;
        cur->blue = 0;
        cur->songid = EMPTY;
        cur->songopt = S_PLAY;
        cur->songstep = 0;
        cur->soundfx = EMPTY;
        cur++;
    }

    cur--;
    cur->startf = M_FADEOUT;
    cur->startsteps = 64;
    cur->endf = M_PALETTE;
    cur->endsteps = 64;

    return MOVIE_Play( frm, 1, palette ) == K_SKIPALL;
}

/***************************************************************************
INTRO_Death2 () - Ground Death Scene
 ***************************************************************************/
bool INTRO_Death2( void ) {
    int i;
    int maxframes = 6;
    int framecnt = maxframes - 1;
    FRAME* cur = frm;

    for ( i = 0; i < maxframes; i++, cur++ ) {
        cur->holdframe = 0;
        cur->opt = M_ANIM;
        cur->framerate = 3;
        cur->numframes = framecnt--;
        cur->item = SDEATH_AGX + i;
        cur->startf = M_NORM;
        cur->startsteps = 0;
        cur->endf = M_NORM;
        cur->endsteps = 0;
        cur->red = 0;
        cur->green = 0;
        cur->blue = 0;
        cur->songid = EMPTY;
        cur->songopt = S_PLAY;
        cur->songstep = 0;
        cur->soundfx = EMPTY;
    }

    if ( MOVIE_Play( frm, 8, palette ) == K_SKIPALL ) {
        return true;
    }

    GFX_FadeOut( 0, 0, 0, 100 );

    memset( displaybuffer, 0, 64000 );
    GFX_MarkUpdate( 0, 0, 320, 200 );
    GFX_DisplayUpdate();

    return false;
}

/***************************************************************************
INTRO_Death1 () - Air Death Scene
 ***************************************************************************/
bool INTRO_Death1( void ) {
    int i;
    int maxframes = 30;
    int framecnt = maxframes - 1;
    FRAME* cur = frm;

    for ( i = 0; i < maxframes; i++, cur++ ) {
        cur->holdframe = 0;
        cur->opt = M_ANIM;
        cur->framerate = 11;
        cur->numframes = framecnt--;
        cur->item = DOWN_AGX + i;
        cur->startf = M_NORM;
        cur->startsteps = 0;
        cur->endf = M_NORM;
        cur->endsteps = 0;
        cur->red = 0;
        cur->green = 0;
        cur->blue = 0;
        cur->songid = EMPTY;
        cur->songopt = S_PLAY;
        cur->songstep = 0;
        cur->soundfx = EMPTY;
    }

    return MOVIE_Play( frm, 1, palette ) == K_SKIPALL;
}

/***************************************************************************
INTRO_Death () - Death Scene
 ***************************************************************************/
bool INTRO_Death( void ) {

    if ( INTRO_Death1() ) {
        return true;
    }
    INTRO_Death2();
    return false;
}

/***************************************************************************
INTRO_Game1End () - Game 1 Victory
 ***************************************************************************/
bool INTRO_Game1End( void ) {
    int i;
    int maxframes = 5;
    int framecnt = maxframes - 1;
    FRAME* cur = frm;

    for ( i = 0; i < maxframes; i++, cur++ ) {
        cur->holdframe = 0;
        cur->opt = M_ANIM;
        cur->framerate = 4;
        cur->numframes = framecnt--;
        cur->item = GAME1END_AGX + i;
        cur->startf = M_NORM;
        cur->startsteps = 0;
        cur->endf = M_NORM;
        cur->endsteps = 0;
        cur->red = 0;
        cur->green = 0;
        cur->blue = 0;
        cur->songid = EMPTY;
        cur->songopt = S_PLAY;
        cur->songstep = 0;
        cur->soundfx = EMPTY;
    }

    if ( MOVIE_Play( frm, 8, palette ) == K_SKIPALL ) {
        return true;
    }

    GFX_FadeOut( 0, 0, 0, 120 );

    memset( displaybuffer, 0, 64000 );
    GFX_MarkUpdate( 0, 0, 320, 200 );
    GFX_DisplayUpdate();

    GFX_SetPalette( palette, 0 );

    return false;
}

/***************************************************************************
INTRO_Game2End () - Game 1 Victory
 ***************************************************************************/
bool INTRO_Game2End( void ) {
    int i;
    int maxframes = 25;
    int framecnt = maxframes - 1;
    FRAME* cur = frm;

    for ( i = 0; i < maxframes; i++, cur++ ) {
        cur->holdframe = 0;
        cur->opt = M_ANIM;
        cur->framerate = 4;
        cur->numframes = framecnt--;
        cur->item = GAME2END_AGX + i;
        cur->startf = M_NORM;
        cur->startsteps = 0;
        cur->endf = M_NORM;
        cur->endsteps = 0;
        cur->red = 0;
        cur->green = 0;
        cur->blue = 0;
        cur->songid = EMPTY;
        cur->songopt = S_PLAY;
        cur->songstep = 0;
        cur->soundfx = EMPTY;

        if ( i == 22 ) {
            cur->soundfx = FX_AIREXPLO;
            cur->fx_xpos = 127;
        }

        if ( i == 24 ) {
            cur->soundfx = FX_AIREXPLO;
            cur->fx_xpos = 127;
        }
    }

    if ( MOVIE_Play( frm, 1, palette ) == K_SKIPALL ) {
        return true;
    }

    GFX_FadeOut( 0, 0, 0, 120 );

    memset( displaybuffer, 0, 64000 );
    GFX_MarkUpdate( 0, 0, 320, 200 );
    GFX_DisplayUpdate();

    GFX_SetPalette( palette, 0 );

    return false;
}

/***************************************************************************
INTRO_Game3End () - Game 1 Victory
 ***************************************************************************/
bool INTRO_Game3End( void ) {
    int i;
    int maxframes = 39;
    int framecnt = maxframes - 1;
    FRAME* cur = frm;

    MOVIE_BPatch( FX_JETSND );

    for ( i = 0; i < maxframes; i++, cur++ ) {
        cur->holdframe = 0;
        cur->opt = M_ANIM;
        cur->framerate = 8;
        cur->numframes = framecnt--;
        cur->item = GAME3END_AGX + i;
        cur->startf = M_NORM;
        cur->startsteps = 0;
        cur->endf = M_NORM;
        cur->endsteps = 0;
        cur->red = 0;
        cur->green = 0;
        cur->blue = 0;
        cur->songid = EMPTY;
        cur->songopt = S_PLAY;
        cur->songstep = 0;
        cur->soundfx = EMPTY;

        if ( i == 30 ) {
            cur->soundfx = FX_AIREXPLO;
            cur->fx_xpos = 100;
        }

        if ( i == 32 ) {
            cur->soundfx = FX_AIREXPLO;
            cur->fx_xpos = 100;
        }
    }

    if ( MOVIE_Play( frm, 1, palette ) == K_SKIPALL ) {
        return true;
    }

    GFX_FadeOut( 0, 0, 0, 120 );

    memset( displaybuffer, 0, 64000 );
    GFX_MarkUpdate( 0, 0, 320, 200 );
    GFX_DisplayUpdate();

    GFX_SetPalette( palette, 0 );

    return false;
}

/***************************************************************************
INTRO_EndGame() - Ends the current game anims
 ***************************************************************************/
void INTRO_EndGame( int game ) {
    IMS_StartAck();

    if ( !reg_flag && game > 0 ) {
        return;
    }

    if ( game > 2 ) {
        game = 2;
    }

    while ( IMS_IsAck() )
        ;
    IMS_StartAck();

    switch ( game ) {
        default:
        case 0:
            INTRO_Game1End();
            break;

        case 1:
            INTRO_Game2End();
            break;

        case 2:
            INTRO_Game3End();
            break;
    }

    while ( IMS_IsAck() )
        ;
    IMS_StartAck();
    INTRO_Base();

    while ( IMS_IsAck() )
        ;
    IMS_StartAck();
    INTRO_Landing();

    while ( IMS_IsAck() )
        ;
    IMS_StartAck();
    WIN_WinGame( game );

    if ( game < 1 ) {
        WIN_Order();
    }
}

/***************************************************************************
INTRO_Taiwan (
 ***************************************************************************/
void INTRO_Taiwan( void ) {
    int i;
    BYTE* pal1;
    BYTE* pic1;
    int local_cnt = framecount;

    framecount = 0;

    pic1 = GLB_GetItem( TAIWARN_PIC );
    pal1 = GLB_GetItem( TAIPAL_DAT );

    GFX_FadeOut( 0, 0, 0, 5 );

    GFX_PutImage( pic1, 0, 0, false );
    GFX_DisplayUpdate();

    GFX_FadeIn( pal1, 64 );

    for ( i = 0; i < 45; i++ ) {
        local_cnt = framecount;
        if ( IMS_IsAck() ) {
            break;
        }
        while ( framecount - local_cnt < 4 )
            ;
    }

    GFX_FadeOut( 0, 0, 0, 63 );

    GLB_FreeItem( TAIPAL_DAT );
    GLB_FreeItem( TAIWARN_PIC );
}

/***************************************************************************
INTRO_Credits() - Credits Screen
 ***************************************************************************/
bool INTRO_Credits( void ) {
    int i;
    BYTE* pal1;
    BYTE* pal2;
    BYTE* pic1;
    BYTE* pic2;
    int local_cnt = framecount;

    framecount = 0;

    pic1 = GLB_GetItem( APOGEE_PIC );
    pal1 = GLB_GetItem( POGPAL_DAT );

    GFX_FadeOut( 0, 0, 0, 5 );

    GFX_PutImage( pic1, 0, 0, false );
    GFX_DisplayUpdate();

    GFX_FadeIn( pal1, 64 );

    if ( bday_num != EMPTY && dig_flag ) {
        SND_Patch( FX_THEME, 127 );
    } else {
        SND_PlaySong( APOGEE_MUS, false, false );
    }

    for ( i = 0; i < 30; i++ ) {
        local_cnt = framecount;
        if ( IMS_IsAck() ) {
            break;
        }
        while ( framecount - local_cnt < 4 )
            ;
    }

    if ( bday_num != EMPTY && dig_flag ) {
        while ( SND_IsPatchPlaying( FX_THEME ) )
            ;
    } else {
        while ( SND_IsSongPlaying() ) {
            if ( IMS_IsAck() ) {
                break;
            }
        }
    }

    GFX_FadeOut( 0, 0, 0, 63 );

    GLB_FreeItem( POGPAL_DAT );
    GLB_FreeItem( APOGEE_PIC );

    SND_PlaySong( EMPTY, false, true );

    memset( displayscreen, 0, 64000 );
    memset( displaybuffer, 0, 64000 );

    pic2 = GLB_GetItem( CYGNUS_PIC );
    pal2 = GLB_GetItem( CYGPAL_DAT );

    GFX_PutImage( pic2, 0, 0, false );
    GFX_DisplayUpdate();

    GFX_FadeIn( pal1, 64 );

    GLB_CacheItem( RINTRO_MUS );

    for ( i = 0; i < 65; i++ ) {
        if ( IMS_IsAck() && i > 0 ) {
            break;
        }

        if ( i == 1 || i == 40 ) {
            SND_Patch( FX_BOSS1, 127 );
        }

        if ( i == 45 ) {
            SND_PlaySong( RINTRO_MUS, true, true );
        }

        if ( KBD_Key( SC_ESC ) ) {
            break;
        }
        local_cnt = framecount;
        while ( framecount - local_cnt < 3 )
            ;
    }

    GFX_FadeOut( 0, 0, 0, 63 );

    memset( displaybuffer, 0, 64000 );
    GFX_MarkUpdate( 0, 0, 320, 200 );
    GFX_DisplayUpdate();

    GFX_SetPalette( palette, 0 );

    GLB_FreeItem( CYGPAL_DAT );
    GLB_FreeItem( CYGNUS_PIC );

    IMS_StartAck();

    return false;
}

/***************************************************************************
INTRO_BaseLanding() - BaseLanding PLays all needed MOVES
 ***************************************************************************/
void INTRO_BaseLanding( void ) {
    if ( !reg_flag || INTRO_Base() ) {
        return;
    }
    INTRO_Landing();
}

/***************************************************************************
INTRO_PlayMain() - Plays Main Intro
 ***************************************************************************/
bool INTRO_PlayMain( void ) {
    return INTRO_City() //
        || INTRO_Side1() //
        || INTRO_Pilot() //
        || INTRO_Side2() //
        || INTRO_Explosion();
}
