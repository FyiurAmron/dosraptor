/****************************************************************************
 * GFXapi  - Graphic Library for 320x200 256 color vga/mcga
 *----------------------------------------------------------------------------
 * Copyright (C) 1992  Scott Hostynski All Rights Reserved
 *----------------------------------------------------------------------------
 *
 * Created by:  Scott Host
 * Date:        Oct, 1992
 *
 * CONTENTS: gfxapi.c gfxapi_a.asm gfxapi.h
 *
 * EXTERN MODULES - NONE
 *
 *---------------------------------------------------------------------------*/

#include <stddef.h>

#ifndef _TYPES_H
#include <types.h>
#endif

#ifndef _GFXAPI
#define _GFXAPI

#define GFX_DARK     0
#define GFX_LITE     1

#define SCREENWIDTH  320
#define SCREENHEIGHT 200

typedef enum { UPPER_LEFT, UPPER_RIGHT, LOWER_LEFT, LOWER_RIGHT } CORNER;
typedef enum { DARK, LIGHT, GREY } SHADE;
typedef enum { GSPRITE, GPIC } GFX_TYPE;

#define CLIP_XRIGHT  2
#define CLIP_XLEFT   4
#define CLIP_YTOP    8
#define CLIP_YBOTTOM 16

typedef struct {
    GFX_TYPE type; // type of picture
    INT opt1; // option 1
    INT opt2; // option 2
    INT width; // width of pic
    INT height; // heigth of pic
} GFX_PIC;

typedef struct {
    INT x; // X POS OF SEG
    INT y; // Y POS OF SEG
    INT offset; // OFFSET FROM X, Y
    INT length; // LENGTH OF LINE
} GFX_SPRITE;

typedef struct {
    WORD opt;
    WORD fill;
    WORD offset;
    WORD length;
} ANIMLINE;

typedef struct {
    INT height;
    SHORT charofs[256];
    CHAR width[256];
} FONT;

/*==========================================================================
   GFX_TimeFrameRate () - Should be interrupt called at 70 fps
 ==========================================================================*/
INT GFX_TimeFrameRate( void );

/***************************************************************************
GFX_SetDebug () - Sets Debug mode
 ***************************************************************************/
void GFX_SetDebug(
    BOOL flag // INPUT : TRUE/FALSE
);

/*************************************************************************
   GFX_ClipLines ()
 *************************************************************************/
INT // RETURN: 0 = Off, 1 == No Clip , 2 == Cliped
GFX_ClipLines(
    BYTE** image, // INOUT : pointer to image or NULL
    INT* x, // INOUT : pointer to x pos
    INT* y, // INOUT : pointer to y pos
    INT* lx, // INOUT : pointer to width
    INT* ly // INOUT : pointer to length
);

/**************************************************************************
   GFX_SetVideoMode13() - sets 256 color 320x200 mode
 **************************************************************************/
void GFX_SetVideoMode13( void );

/**************************************************************************
   GFX_RestoreMode() - Restores Original video mode
 **************************************************************************/
void GFX_RestoreMode( void );

/**************************************************************************
  GFX_InitSystem() - Sets up palette tables, allocs buff, set video mode
 **************************************************************************/
void GFX_InitSystem( void );

/**************************************************************************
GFX_InitVideo() - Inits things related to Video, and sets up fade tables
 **************************************************************************/
void GFX_InitVideo( BYTE* curpal );

/**************************************************************************
  GFX_EndSystem() - Frees up all resources used by GFX system
 **************************************************************************/
void GFX_EndSystem( void );

/**************************************************************************
  GFX_GetPalette() - Sets 256 color palette
 **************************************************************************/
void GFX_GetPalette(
    BYTE* curpal // OUTPUT : pointer to palette data
);

/**************************************************************************
GFX_FadeOut () - Fade Palette out to ( Red, Green , and Blue Value
 **************************************************************************/
void GFX_FadeOut(
    INT red, // INPUT : red ( 0 - 63 )
    INT green, // INPUT : green ( 0 - 63 )
    INT blue, // INPUT : blue ( 0 - 63 )
    INT speed // INPUT : speed of fade
);

/**************************************************************************
GFX_FadeIn () - Fades From current palette to new palette
 **************************************************************************/
void GFX_FadeIn(
    BYTE* palette, // INPUT : palette to fade into
    INT steps // INPUT : steps of fade ( 0 - 255 )
);

/**************************************************************************
GFX_FadeStart () - Sets up fade for GFX_FadeFrame()
 **************************************************************************/
void GFX_FadeStart( void );

/**************************************************************************
GFX_FadeFrame () - Fades Individual Frames
 **************************************************************************/
void GFX_FadeFrame(
    BYTE* palette, // INPUT : palette to fade into
    INT cur_step, // INPUT : cur step position
    INT steps // INPUT : total steps of fade ( 0 - 255 )
);

/**************************************************************************
GFX_SetPalRange() - Sets start and end range for remaping stuff
 **************************************************************************/
void GFX_SetPalRange( INT start, INT end );

/**************************************************************************
  GFX_GetRGB() - gets R,G and B values from pallete data
 **************************************************************************/
void GFX_GetRGB(
    BYTE* pal, // INPUT : pointer to palette data
    INT num, // INPUT : palette entry
    INT* red, // OUTPUT: red value
    INT* green, // OUTPUT: green value
    INT* blue // OUTPUT: blue value
);

/**************************************************************************
  GFX_Remap() - Finds the closest color avalable
 **************************************************************************/
INT // RETURN: new color number
GFX_Remap(
    BYTE* pal, // INPUT : pointer to palette data
    INT red, // INPUT : red  ( 0 - 63 )
    INT green, // INPUT : green( 0 - 63 )
    INT blue // INPUT : blue ( 0 - 63 )
);

/**************************************************************************
  GFX_MakeLightTable() - make a light/dark palette lookup table
 **************************************************************************/
void GFX_MakeLightTable(
    BYTE* palette, // INPUT : pointer to palette data
    BYTE* ltable, // OUTPUT: pointer to lookup table
    INT level // INPUT : - 63 to + 63
);

/**************************************************************************
  GFX_MakeGreyTable() - make a grey palette lookup table
 **************************************************************************/
void GFX_MakeGreyTable(
    BYTE* palette, // INPUT : pointer to palette data
    BYTE* ltable // OUTPUT: pointer to lookup table
);

/*************************************************************************
   GFX_GetScreen() -     Gets A block of screen memory to CPU memory
 *************************************************************************/
void GFX_GetScreen(
    BYTE* outmem, // OUTPUT: pointer to CPU mem
    INT x, // INPUT : x pos
    INT y, // INPUT : y pos
    INT lx, // INPUT : x length
    INT ly // INPUT : y length
);

/*************************************************************************
   GFX_PutTexture() - Repeats a Picture though the area specified
 *************************************************************************/
void GFX_PutTexture(
    BYTE* intxt, // INPUT : color texture
    INT x, // INPUT : x pos
    INT y, // INPUT : y pos
    INT lx, // INPUT : x length
    INT ly // INPUT : y length
);

/*************************************************************************
   GFX_ShadeArea()- lightens or darkens and area of the screen
 *************************************************************************/
void GFX_ShadeArea(
    SHADE opt, // INPUT : GFX_Dark or GFX_Lite
    INT x, // INPUT : x position
    INT y, // INPUT : y position
    INT lx, // INPUT : x length
    INT ly // INPUT : y length
);

/*************************************************************************
   GFX_ShadeShape()- lightens or darkens and area of the screen
 *************************************************************************/
void GFX_ShadeShape(
    SHADE opt, // INPUT : DARK/LIGHT or GREY
    BYTE* mask, // INPUT : mask 0 = no shade ( GFX format pic )
    INT x, // INPUT : x position
    INT y // INPUT : y position
);

/*************************************************************************
   GFX_VShadeLine () - Shades a vertical line
 *************************************************************************/
void GFX_VShadeLine(
    SHADE opt, // INPUT : DARK or LIGHT
    INT x, // INPUT : x position
    INT y, // INPUT : y position
    INT ly // INPUT : length of line
);

/*************************************************************************
   GFX_HShadeLine () Shades a Horizontal Line
 *************************************************************************/
void GFX_HShadeLine(
    SHADE opt, // INPUT : DARK or LIGHT
    INT x, // INPUT : x position
    INT y, // INPUT : y position
    INT lx // INPUT : length of line
);

/*************************************************************************
   GFX_LightBox()- Draws a rectangle border with light source
 *************************************************************************/
void GFX_LightBox(
    CORNER opt, // INPUT : light source
    INT x, // INPUT : x position
    INT y, // INPUT : y position
    INT lx, // INPUT : x length
    INT ly // INPUT : y length
);

/*************************************************************************
   GFX_ColorBox () - sets a rectangular area to color
 *************************************************************************/
void GFX_ColorBox(
    INT x, // INPUT : x position
    INT y, // INPUT : y position
    INT lx, // INPUT : width
    INT ly, // INPUT : length
    INT color // INPUT : fill color ( 0 - 255 )
);

/*************************************************************************
   GFX_HLine () - plots a horizontal line in color
 *************************************************************************/
void GFX_HLine(
    INT x, // INPUT : x position
    INT y, // INPUT : y position
    INT lx, // INPUT : width
    INT color // INPUT : fill color ( 0 - 255 )
);

/*************************************************************************
   GFX_VLine () plots a vertical line in color
 *************************************************************************/
void GFX_VLine(
    INT x, // INPUT : x position
    INT y, // INPUT : y position
    INT ly, // INPUT : length
    INT color // INPUT : fill color ( 0 - 255 )
);

/*************************************************************************
   GFX_Rectangle () - sets a rectangular border to color
 *************************************************************************/
void GFX_Rectangle(
    INT x, // INPUT : x position
    INT y, // INPUT : y position
    INT lx, // INPUT : width
    INT ly, // INPUT : length
    INT color // INPUT : fill color ( 0 - 255 )
);

/*************************************************************************
   GFX_Line () plots a line in color
 *************************************************************************/
void GFX_Line(
    INT x, // INPUT : x start point
    INT y, // INPUT : y start point
    INT x2, // INPUT : x2 end point
    INT y2, // INPUT : y2 end point
    INT color // INPUT : color ( 0 - 255 )
);

/*************************************************************************
   GFX_ScalePic () - Scales picture optionaly make color 0 see thru
 *************************************************************************/
void GFX_ScalePic(
    BYTE* buffin, // INPUT : pointer to pic data
    INT x, // INPUT : x display position
    INT y, // INPUT : y display position
    INT new_lx, // INPUT : new x length
    INT new_ly, // INPUT : new y length
    BOOL see_thru // INPUT : TRUE = see thru
);

/*************************************************************************
   GFX_MarkUpdate () Marks an area to be draw with GFX_DrawScreen()
 *************************************************************************/
void GFX_MarkUpdate(
    INT x, // INPUT : x position
    INT y, // INPUT : y position
    INT lx, // INPUT : x length
    INT ly // INPUT : y length
);

/*************************************************************************
   GFX_ForceUpdate () Marks an area to be draw with GFX_DrawScreen()
 *************************************************************************/
void GFX_ForceUpdate(
    INT x, // INPUT : x position
    INT y, // INPUT : y position
    INT lx, // INPUT : x length
    INT ly // INPUT : y length
);

/***************************************************************************
   GFX_SetFrameHook () sets function to call before every screen update
 ***************************************************************************/
void GFX_SetFrameHook( void ( *func )( void ( * )( void ) ) // INPUT : pointer to function
);

/***************************************************************************
 GFX_Delay () - Delay for ( count ) of screen frames ( sec/70 )
 ***************************************************************************/
void GFX_Delay(
    INT count // INPUT : wait # of frame ticks
);

/***************************************************************************
   GFX_WaitUpdate () - Updates screen at specified frame rate
 ***************************************************************************/
void GFX_WaitUpdate(
    INT count // INPUT : frame rate ( MAX = 70 )
);

/***************************************************************************
   GFX_DisplayUpdate () - Displays Marked Areas to Display
 ***************************************************************************/
void GFX_DisplayUpdate( void );

/***************************************************************************
   GFX_PutImage() - places image in displaybuffer and performs cliping
 ***************************************************************************/
void GFX_PutImage(
    BYTE* image, // INPUT : image data
    INT x, // INPUT : x position
    INT y, // INPUT : y position
    BOOL see_thru // INPUT : true = masked, false = put block
);

/***************************************************************************
   GFX_Print () - prints a string using specified font with basecolor
 ***************************************************************************/
INT // RETURN: length of print
GFX_Print(
    INT printx, // INPUT : x position
    INT printy, // INPUT : y position
    CHAR* str, // INPUT : string to print
    void* infont, // INPUT : pointer to font
    INT basecolor // INPUT : basecolor of font
);

/***************************************************************************
   GFX_PutSprite () -Puts a Sprite into display buffer
 ***************************************************************************/
void GFX_PutSprite(
    BYTE* inmem, // INPUT : inmem
    INT x, // INPUT : x pos
    INT y // INPUT : y pos
);

/***************************************************************************
   GFX_OverlayImage() - places image in displaybuffer and performs cliping
 ***************************************************************************/
void GFX_OverlayImage(
    BYTE* baseimage, // INPUT : base image data
    BYTE* overimage, // INPUT : overlay image data
    INT x, // INPUT : x position
    INT y // INPUT : y position
);

/***************************************************************************
   GFX_StrPixelLen() - Calculates the length of a GFX string
 ***************************************************************************/
INT // RETURNS : pixel length
GFX_StrPixelLen(
    void* infont, // INPUT : pointer to current font
    CHAR* instr, // INPUT : pointer to string
    size_t maxloop // INPUT : length of string
);

/***************************************************************************
   GFX_3D_SetView() Sets user view in 3d space
 ***************************************************************************/
void GFX_3D_SetView(
    INT x, // INPUT : x position
    INT y, // INPUT : y position
    INT z // INPUT : z position
);

/***************************************************************************
   GFX_3D_PutImage() - places image in displaybuffer and performs cliping
 ***************************************************************************/
void GFX_3D_PutImage(
    BYTE* image, // INPUT : image data
    INT x, // INPUT : x position
    INT y, // INPUT : y position
    INT z, // INPUT : z position ( distance )
    BOOL see_thru // INPUT : true = masked, false = put block
);

/***************************************************************************
   GFX_Print () - prints a string using specified font with basecolor
 ***************************************************************************/
INT GFX_Print(
    INT printx, // INPUT : x position
    INT printy, // INPUT : y position
    CHAR* str, // INPUT : string to print
    void* infont, // INPUT : pointer to font
    INT basecolor // INPUT : basecolor of font
);

/***************************************************************************
   GFX_3D_SetView() Sets user view in 3d space
 ***************************************************************************/
void GFX_3D_SetView(
    INT x, // INPUT : x position
    INT y, // INPUT : y position
    INT z // INPUT : z position
);

/***************************************************************************
   GFX_3DPoint () plots a points in 3D space
 ***************************************************************************/
void GFX_3DPoint( void );

/**************************************************************************
  GFX_SetPalette() - Sets 256 color palette
 **************************************************************************/
void GFX_SetPalette(
    BYTE* curpal, // INPUT : pointer to palette data
    INT startpal // INPUT : palette start number 0 -256
);

/*========================================================================
;  GFX_ScaleLine() - scale from scale table (stable) color 0 transparent
 ========================================================================*/
void __cdecl GFX_ScaleLine(
    BYTE* outmem, // INPUT : pointer to destination
    BYTE* inmem // INPUT : pointer to picture data
);

/*========================================================================
; GFX_CScaleLine() - scale from scale table (stable) color 0 transparent
 ========================================================================*/
void __cdecl GFX_CScaleLine(
    BYTE* outmem, // INPUT : pointer to destination
    BYTE* inmem // INPUT : pointer to picture data
);

/*========================================================================
GFX_ShadeSprite () - Shades a Sprite shape into displaybuffer
 ========================================================================*/
#pragma aux GFX_ShadeSprite "_*" parm[EDX][ESI][EAX] modify[EBX ECX EDI]
void GFX_ShadeSprite( BYTE* dest, BYTE* inmem, BYTE* dtable );

/*========================================================================
GFX_DrawSprite () - Draws a Sprite into displaybuffer
 ========================================================================*/
#pragma aux GFX_DrawSprite "_*" parm[EDX][ESI] modify[EAX EBX ECX EDI]
void GFX_DrawSprite( BYTE* dest, BYTE* inmem );

/*========================================================================
; GFX_Shade() - Remaps Bytes according to shade table
 ========================================================================*/
#pragma aux GFX_Shade "_*" parm[EDI][ECX][EAX] modify[EBX EDX ESI]
void GFX_Shade(
    BYTE* outmem, // INPUT : EAX pointer to destination
    INT maxlen, // INPUT : EDX length of buffer to shade
    BYTE* dtable // INPUT : EBX pointer to shade table
);

/*========================================================================
; GFX_PutPic() - Puts Picture into buffer
 ========================================================================*/
#pragma aux GFX_PutPic "_*" modify[EAX EBX ECX EDX ESI EDI]
void GFX_PutPic( void );

/*========================================================================
; GFX_PutMaskPic() - Puts Picture into buffer with color 0 see thru
 ========================================================================*/
#pragma aux GFX_PutMaskPic "_*" modify[EAX EBX ECX EDX ESI EDI]
void GFX_PutMaskPic( void );

/*========================================================================
 ========================================================================*/
void __cdecl GFX_DrawChar(
    BYTE* dest, // OUTPUT: destination buffer
    BYTE* inmem, // INPUT : source buffer
    INT width, // INPUT : width of character
    INT height, // INPUT : height of character
    INT addx, // INPUT : addx offset
    INT color // INPUT : base color
);

/*========================================================================
; GFX_DisplayScreen() - Puts Display Buffer into Video memory
 ========================================================================*/
#pragma aux GFX_DisplayScreen "_*" modify[EAX EBX ECX EDX ESI EDI]
void GFX_DisplayScreen( void );

void GFX_SetRetraceFlag( BOOL flag );

#define GFX_SetRetraceFlag( x ) ( retraceflag = x )

#define FRAME_COUNT             ( framecount )

extern BYTE* displaybuffer;
extern BYTE* displayscreen;
extern BYTE* ltable;
extern BYTE* dtable;
extern BYTE* gtable;
extern INT g_rseed;
extern BOOL retraceflag;
extern DWORD ylookup[];
extern volatile INT framecount;

extern INT G3D_x; // input: x position
extern INT G3D_y; // input: y position
extern INT G3D_z; // input: z position
extern INT G3D_screenx; // output: screen x pos
extern INT G3D_screeny; // output: screen y pos
extern INT G3D_viewx; // user view x pos
extern INT G3D_viewy; // user view y pos
extern INT G3D_viewz; // user view z pos

#endif
