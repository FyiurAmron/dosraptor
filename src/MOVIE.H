#ifndef MOVIE_H
#define MOVIE_H

#include "../gfx/types.h"

typedef enum { M_ANIM, M_PIC, M_SEE_THRU } FRAMETYPE;

typedef enum { M_NORM, M_FADEIN, M_FADEOUT, M_PALETTE, M_ERASE } FRAMEOPT;

typedef enum { S_PLAY, S_FADEIN, S_FADEOUT, S_STOP } SONGOPTS;

typedef struct {
    FRAMETYPE opt; // TYPE OF DRAWING REQUIRED
    int framerate; // FRAME RATE OF UPDATE
    int numframes; // NUMBER OF FRAMES LEFT
    DWORD item; // ITEM # OF PICTURE
    FRAMEOPT startf; // START FRAME OPTS
    int startsteps; // # OF STEPS IF FADEIN
    FRAMEOPT endf; // END FRAME OPTS
    int endsteps; // # OF STEPS IN FADEOUT
    int red; // RED VAL FOR FADEOUT
    int green; // GREEN VAL FOR FADEOUT
    int blue; // BLUE VAL FOR FADEOUT
    int holdframe; // NUMBER OF TICS TO HOLD FRAME
    int songid; // SONG ID TO PLAY
    SONGOPTS songopt; // SONG OPTS
    int songstep; // SONG STEPS FOR FADES
    int soundfx; // SOUND FX START
    int fx_vol; // SOUND FX VOLUME
    int fx_xpos; // SOUND FX XPOS
} FRAME;

/*************************************************************************
MOVIE_BPatch() Plays Sound FX in background for one anim
 *************************************************************************/
void MOVIE_BPatch( int soundfx );

/*************************************************************************
   MOVIE_ShowFrame () - Shows an animation frame
 *************************************************************************/
void MOVIE_ShowFrame(
    BYTE* inpic // INPUT : pointer to animpic
);

/*************************************************************************
MOVIE_Play () - Playes an Animation
 *************************************************************************/
int MOVIE_Play(
    FRAME* frame, // INPUT : pointer to array of frame defs
    int numplay, // INPUT : number of times to play
    BYTE* palette // INPUT : pointer to palette
);

#endif
