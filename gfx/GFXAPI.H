/****************************************************************************
 * GFXapi  - Graphic Library for 320x200 256 color vga/mcga
 *---------------------------------------------------------------------------*/

#ifndef GFXAPI_H
#define GFXAPI_H

#include <stddef.h>
#include "types.h"

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
    int opt1; // option 1
    int opt2; // option 2
    int width; // width of pic
    int height; // heigth of pic
} GFX_PIC;

typedef struct {
    int x; // X POS OF SEG
    int y; // Y POS OF SEG
    int offset; // OFFSET FROM X, Y
    int length; // LENGTH OF LINE
} GFX_SPRITE;

typedef struct {
    WORD opt;
    WORD fill;
    WORD offset;
    WORD length;
} ANIMLINE;

typedef struct {
    int height;
    short charofs[256];
    char width[256];
} FONT;

/*==========================================================================
   GFX_TimeFrameRate () - Should be interrupt called at 70 fps
 ==========================================================================*/
void GFX_TimeFrameRate( void );

/***************************************************************************
GFX_SetDebug () - Sets Debug mode
 ***************************************************************************/
void GFX_SetDebug( bool flag );

/*************************************************************************
   GFX_ClipLines ()
 *************************************************************************/
int // RETURN: 0 = Off, 1 == No Clip , 2 == Cliped
GFX_ClipLines(
    BYTE** image, // INOUT : pointer to image or NULL
    int* x, // INOUT : pointer to x pos
    int* y, // INOUT : pointer to y pos
    int* lx, // INOUT : pointer to width
    int* ly // INOUT : pointer to length
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
    int red, // INPUT : red ( 0 - 63 )
    int green, // INPUT : green ( 0 - 63 )
    int blue, // INPUT : blue ( 0 - 63 )
    int speed // INPUT : speed of fade
);

/**************************************************************************
GFX_FadeIn () - Fades From current palette to new palette
 **************************************************************************/
void GFX_FadeIn(
    BYTE* palette, // INPUT : palette to fade into
    int steps // INPUT : steps of fade ( 0 - 255 )
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
    int cur_step, // INPUT : cur step position
    int steps // INPUT : total steps of fade ( 0 - 255 )
);

/**************************************************************************
GFX_SetPalRange() - Sets start and end range for remaping stuff
 **************************************************************************/
void GFX_SetPalRange( int start, int end );

/**************************************************************************
  GFX_GetRGB() - gets R,G and B values from pallete data
 **************************************************************************/
void GFX_GetRGB(
    BYTE* pal, // INPUT : pointer to palette data
    int num, // INPUT : palette entry
    int* red, // OUTPUT: red value
    int* green, // OUTPUT: green value
    int* blue // OUTPUT: blue value
);

/**************************************************************************
  GFX_Remap() - Finds the closest color avalable
 **************************************************************************/
int // RETURN: new color number
GFX_Remap(
    BYTE* pal, // INPUT : pointer to palette data
    int red, // INPUT : red  ( 0 - 63 )
    int green, // INPUT : green( 0 - 63 )
    int blue // INPUT : blue ( 0 - 63 )
);

/**************************************************************************
  GFX_MakeLightTable() - make a light/dark palette lookup table
 **************************************************************************/
void GFX_MakeLightTable(
    BYTE* palette, // INPUT : pointer to palette data
    BYTE* ltable, // OUTPUT: pointer to lookup table
    int level // INPUT : - 63 to + 63
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
    int x, // INPUT : x pos
    int y, // INPUT : y pos
    int lx, // INPUT : x length
    int ly // INPUT : y length
);

/*************************************************************************
   GFX_PutTexture() - Repeats a Picture though the area specified
 *************************************************************************/
void GFX_PutTexture(
    BYTE* intxt, // INPUT : color texture
    int x, // INPUT : x pos
    int y, // INPUT : y pos
    int lx, // INPUT : x length
    int ly // INPUT : y length
);

/*************************************************************************
   GFX_ShadeArea()- lightens or darkens and area of the screen
 *************************************************************************/
void GFX_ShadeArea(
    SHADE opt, // INPUT : GFX_Dark or GFX_Lite
    int x, // INPUT : x position
    int y, // INPUT : y position
    int lx, // INPUT : x length
    int ly // INPUT : y length
);

/*************************************************************************
   GFX_ShadeShape()- lightens or darkens and area of the screen
 *************************************************************************/
void GFX_ShadeShape(
    SHADE opt, // INPUT : DARK/LIGHT or GREY
    BYTE* mask, // INPUT : mask 0 = no shade ( GFX format pic )
    int x, // INPUT : x position
    int y // INPUT : y position
);

/*************************************************************************
   GFX_VShadeLine () - Shades a vertical line
 *************************************************************************/
void GFX_VShadeLine(
    SHADE opt, // INPUT : DARK or LIGHT
    int x, // INPUT : x position
    int y, // INPUT : y position
    int ly // INPUT : length of line
);

/*************************************************************************
   GFX_HShadeLine () Shades a Horizontal Line
 *************************************************************************/
void GFX_HShadeLine(
    SHADE opt, // INPUT : DARK or LIGHT
    int x, // INPUT : x position
    int y, // INPUT : y position
    int lx // INPUT : length of line
);

/*************************************************************************
   GFX_LightBox()- Draws a rectangle border with light source
 *************************************************************************/
void GFX_LightBox(
    CORNER opt, // INPUT : light source
    int x, // INPUT : x position
    int y, // INPUT : y position
    int lx, // INPUT : x length
    int ly // INPUT : y length
);

/*************************************************************************
   GFX_ColorBox () - sets a rectangular area to color
 *************************************************************************/
void GFX_ColorBox(
    int x, // INPUT : x position
    int y, // INPUT : y position
    int lx, // INPUT : width
    int ly, // INPUT : length
    int color // INPUT : fill color ( 0 - 255 )
);

/*************************************************************************
   GFX_HLine () - plots a horizontal line in color
 *************************************************************************/
void GFX_HLine(
    int x, // INPUT : x position
    int y, // INPUT : y position
    int lx, // INPUT : width
    int color // INPUT : fill color ( 0 - 255 )
);

/*************************************************************************
   GFX_VLine () plots a vertical line in color
 *************************************************************************/
void GFX_VLine(
    int x, // INPUT : x position
    int y, // INPUT : y position
    int ly, // INPUT : length
    int color // INPUT : fill color ( 0 - 255 )
);

/*************************************************************************
   GFX_Rectangle () - sets a rectangular border to color
 *************************************************************************/
void GFX_Rectangle(
    int x, // INPUT : x position
    int y, // INPUT : y position
    int lx, // INPUT : width
    int ly, // INPUT : length
    int color // INPUT : fill color ( 0 - 255 )
);

/*************************************************************************
   GFX_Line () plots a line in color
 *************************************************************************/
void GFX_Line(
    int x, // INPUT : x start point
    int y, // INPUT : y start point
    int x2, // INPUT : x2 end point
    int y2, // INPUT : y2 end point
    int color // INPUT : color ( 0 - 255 )
);

/*************************************************************************
   GFX_MarkUpdate () Marks an area to be draw with GFX_DrawScreen()
 *************************************************************************/
void GFX_MarkUpdate(
    int x, // INPUT : x position
    int y, // INPUT : y position
    int lx, // INPUT : x length
    int ly // INPUT : y length
);

/*************************************************************************
   GFX_ForceUpdate () Marks an area to be draw with GFX_DrawScreen()
 *************************************************************************/
void GFX_ForceUpdate(
    int x, // INPUT : x position
    int y, // INPUT : y position
    int lx, // INPUT : x length
    int ly // INPUT : y length
);

/***************************************************************************
   GFX_SetFrameHook () sets function to call before every screen update
 ***************************************************************************/
void GFX_SetFrameHook(
    void ( *func )( void ( * )( void ) ) // INPUT : pointer to function
);

/***************************************************************************
 GFX_Delay () - Delay for ( count ) of screen frames ( sec/70 )
 ***************************************************************************/
void GFX_Delay(
    int count // INPUT : wait # of frame ticks
);

/***************************************************************************
   GFX_WaitUpdate () - Updates screen at specified frame rate
 ***************************************************************************/
void GFX_WaitUpdate(
    int count // INPUT : frame rate ( MAX = 70 )
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
    int x, // INPUT : x position
    int y, // INPUT : y position
    bool see_thru // INPUT : true = masked, false = put block
);

/***************************************************************************
   GFX_Print () - prints a string using specified font with basecolor
 ***************************************************************************/
int // RETURN: length of print
GFX_Print(
    int printx, // INPUT : x position
    int printy, // INPUT : y position
    char* str, // INPUT : string to print
    void* infont, // INPUT : pointer to font
    int basecolor // INPUT : basecolor of font
);

/***************************************************************************
   GFX_PutSprite () -Puts a Sprite into display buffer
 ***************************************************************************/
void GFX_PutSprite(
    BYTE* inmem, // INPUT : inmem
    int x, // INPUT : x pos
    int y // INPUT : y pos
);

/***************************************************************************
   GFX_OverlayImage() - places image in displaybuffer and performs cliping
 ***************************************************************************/
void GFX_OverlayImage(
    BYTE* baseimage, // INPUT : base image data
    BYTE* overimage, // INPUT : overlay image data
    int x, // INPUT : x position
    int y // INPUT : y position
);

/***************************************************************************
   GFX_StrPixelLen() - Calculates the length of a GFX string
 ***************************************************************************/
int // RETURNS : pixel length
GFX_StrPixelLen(
    void* infont, // INPUT : pointer to current font
    char* instr, // INPUT : pointer to string
    size_t maxloop // INPUT : length of string
);

/***************************************************************************
   GFX_3D_SetView() Sets user view in 3d space
 ***************************************************************************/
void GFX_3D_SetView(
    int x, // INPUT : x position
    int y, // INPUT : y position
    int z // INPUT : z position
);

/***************************************************************************
   GFX_Print () - prints a string using specified font with basecolor
 ***************************************************************************/
int GFX_Print(
    int printx, // INPUT : x position
    int printy, // INPUT : y position
    char* str, // INPUT : string to print
    void* infont, // INPUT : pointer to font
    int basecolor // INPUT : basecolor of font
);

/**************************************************************************
  GFX_SetPalette() - Sets 256 color palette
 **************************************************************************/
void GFX_SetPalette(
    BYTE* curpal, // INPUT : pointer to palette data
    int startpal // INPUT : palette start number 0 -256
);

/***************************************************************************
   GFX_3D_SetView() Sets user view in 3d space
 ***************************************************************************/
void GFX_3D_SetView(
    int x, // INPUT : x position
    int y, // INPUT : y position
    int z // INPUT : z position
);

/***************************************************************************
   GFX_3DPoint () plots a points in 3D space
 ***************************************************************************/
void GFX_3DPoint( void );

// gfxapi_a

void GFX_Shade( uint8_t* dst, int maxlen, const uint8_t* shading_table );
void GFX_ShadeSprite( uint8_t* dst, const uint8_t* src, const uint8_t* shading_table );
void GFX_DrawSprite( uint8_t* dst, const uint8_t* src );
void GFX_DrawChar( uint8_t* dst, const uint8_t* src, int lx, int ly, int add_x_src, uint8_t color );
void GFX_PutPic( void );
void GFX_PutMaskPic( void );
void GFX_DisplayScreen( void );

extern BYTE* displaybuffer;
extern BYTE* displayscreen;
extern BYTE* ltable;
extern BYTE* dtable;
extern BYTE* gtable;
extern int g_rseed;
extern bool retraceflag;
extern DWORD ylookup[];
extern volatile int framecount;

extern int G3D_x; // input: x position
extern int G3D_y; // input: y position
extern int G3D_z; // input: z position
extern int G3D_screenx; // output: screen x pos
extern int G3D_screeny; // output: screen y pos
extern int G3D_viewx; // user view x pos
extern int G3D_viewy; // user view y pos
extern int G3D_viewz; // user view z pos

inline void GFX_SetRetraceFlag( bool flag ) {
    retraceflag = flag;
}

#endif
