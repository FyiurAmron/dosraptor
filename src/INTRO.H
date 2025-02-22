
#ifndef INTRO_H
#define INTRO_H

#include "../gfx/types.h"

/***************************************************************************
INTRO_Landing () - Ship Landing on Base
 ***************************************************************************/
bool INTRO_Landing( void );

/***************************************************************************
INTRO_BaseLanding() - BaseLanding PLays all needed MOVES
 ***************************************************************************/
void INTRO_BaseLanding( void );

/***************************************************************************
INTRO_Takeoff() - Ships taking Off
 ***************************************************************************/
bool INTRO_Takeoff( void );

/***************************************************************************
INTRO_PlayMain() - Plays Main Intro
 ***************************************************************************/
bool INTRO_PlayMain( void );

/***************************************************************************
INTRO_Taiwan (
 ***************************************************************************/
void INTRO_Taiwan( void );

/***************************************************************************
INTRO_Credits() - Credits Screen
 ***************************************************************************/
bool INTRO_Credits( void );

/***************************************************************************
INTRO_EndGame() - Ends the current game anims
 ***************************************************************************/
void INTRO_EndGame( int game );

/***************************************************************************
INTRO_Death () - Death Scene
 ***************************************************************************/
bool INTRO_Death( void );

#endif
