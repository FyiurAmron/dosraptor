/****************************************************************************
 * SWDapi  - windowing routines
 *---------------------------------------------------------------------------*/

#ifndef SWDAPI_H
#define SWDAPI_H

// required for FONT etc.
#include "gfxapi.h"

#include "types.h"

typedef enum {
    C_IDLE,
    F_DOWN,
    F_UP,
    F_NEXT,
    F_PREV,
    F_RIGHT,
    F_LEFT,
    F_TOP,
    F_BOTTOM,
    F_FIRST,
    F_SELECT,
    F_OBJ_AREA,
    F_VIEW_AREA,
    W_NEW,
    W_NEXT,
    W_MOVE,
    W_CLOSE,
    W_CLOSE_ALL
} CMD;

typedef enum { S_IDLE, S_FLD_COMMAND, S_WIN_COMMAND, S_FIRSTCALL, S_REDRAW, S_UPDATE, S_END } ACT;

typedef enum { R_IDLE, R_CLOSED, R_CLOSE_ALL, R_END_DIALOG } FCMD;

typedef enum { FILL, TEXTURE, PICTURE, SEE_THRU, INVISABLE } DSTYLE;

typedef enum { NORMAL, UP, DOWN } BUTTON;
typedef enum { I_NORM, I_TOUPPER, I_NUMERIC } INPUTOPT;

#define FLD_OFF      0 // 0
#define FLD_TEXT     1 // 1
#define FLD_BUTTON   2 // 2 *
#define FLD_INPUT    3 // 3 *
#define FLD_MARK     4 // 5 *
#define FLD_CLOSE    5 // 6
#define FLD_DRAGBAR  6 // 7 *
#define FLD_BUMPIN   7 // 8
#define FLD_BUMPOUT  8 // 9
#define FLD_ICON     9 // 4
#define FLD_OBJAREA  10 // 10
#define FLD_VIEWAREA 11 // 11

#define SWD_FILENUM  1

// basic format of a window data object
//
//  SWIN structure
//  SFIELDS..
//  TEXT AREA for window
//

#define MAX_WINDOWS  12
#define MAX_FONTS    2

extern int active_field;
extern BOOL g_button_flag;
extern BOOL usekb_flag;

void SWD_SetButtonFlag( BOOL );
#define SWD_SetButtonFlag( c ) ( g_button_flag = c )

typedef struct {
    int opt; // FIELD TYPE
    int id; // FIELD ID
    DWORD hotkey; // SCAN CODE OF HOT KEY
    BOOL kbflag; // TRUE if field should be KBACTIVE
    DWORD opt3; // not used
    DWORD opt4; // not used
    INPUTOPT input_opt; // OPTIONS used in INPUT FIELDS
    BUTTON bstatus; // BUTTON STATUS NORMAL/UP/DOWN
    char name[16]; // TEXT NAME OF FIELD ( NOT DISPLAYED )
    char item_name[16]; // TEXT NAME OF ITEM #
    DWORD item; // ITEM ID NUMBER
    char font_name[16]; // FONT .GLB NAME
    DWORD fontid; // FONT NUMBER
    int fontbasecolor; // FONT BASE COLOR
    int maxchars; // MAXcharS IN FIELD TEXT
    DSTYLE picflag; // PICTURE TRUE/FALSE
    int color; // COLOR OF FIELD
    int lite; // HIGHLIGHT COLOR
    BOOL mark; // FIELD MARK ( TRUE/FLASE )
    BOOL saveflag; // MEM TO SAVE PIC UNDER FIELD ( Y/N )
    BOOL shadow; // SHADOW ON/OFF
    BOOL selectable; // SELECTABLE ON/OFF
    int x; // X POSITION ON SCREEN
    int y; // Y POSITION ON SCREEN
    int lx; // WIDTH IN PIXELS
    int ly; // HEIGHT IN PIXELS
    DWORD txtoff; // OFFSET TO TEXT DATA ( BYTES )
    BYTE* sptr; // SEG POINTER TO SAVE INFO
} SFIELD;

typedef struct {
    int version; // VERSION NUMBER
    int swdsize; // SIZE OF WIN/FIELD AND TEXT
    BOOL arrowflag; // Use Arrow Keys ( TRUE/FALSE )
    BOOL display; // DISPLAY FLAG
    DWORD opt3; // OPTION 3 ( not used )
    DWORD opt4; // OPTION 4 ( not used )
    int id; // WINDOW ID NUMBER
    int type; // WINDOW TYPE NUMBER
    char name[16]; // TEXT NAME OF WINDOW ( NOT DISPLAYED )
    char item_name[16]; // TEXT NAME OF ITEM
    DWORD item; // ITEM ID NUMBER
    DSTYLE picflag; // FILL/TEXTURE/PICTURE
    BOOL lock; // TRUE = cannot goto other windows
    int fldofs; // OFFSET IN BYTES TO FIRST FIELD
    int txtofs; // OFFSET IN BYTES TO TEXT AREA
    int firstfld; // FIELD TO GOTO FIRST
    int opt; // WINDOW TYPE
    int color; // COLOR OF WINDOW
    int numflds; // NUMBER OF FIELDS
    int x; // X POSITON ON SCREEN
    int y; // Y POSITION ON SCREEN
    int lx; // WIDTH IN PIXELS
    int ly; // HEIGHT IN PIXELS
    BOOL shadow; // SHADOW TRUE/FALSE
} SWIN;

typedef struct {
    DWORD gitem; // GLB ITEM ID
    BOOL flag; // TRUE = in use ,FALSE = not in use
    BOOL viewflag; // TRUE = has viewarea(s) FALSE = none
    SWIN* win; // POINTER TO WINDOW
} SWD_WIN;

typedef struct {
    int window;
    int field;
    ACT cur_act;
    CMD cur_cmd;
    int keypress;
    int id;
    int type;
    int x;
    int y;
    int height;
    int width;
    BOOL objactive;
    BOOL viewactive;
    int sfield;
    int sx;
    int sy;
    int sheight;
    int swidth;
} SWD_DLG;

/***************************************************************************
 SWD_Install() - Initializes Window system
 ***************************************************************************/
void SWD_Install(
    BOOL moveflag // INPUT : Use Move Window feature ( 64k )
);

/***************************************************************************
   SWD_End () - Frees up resources used by SWD System
 ***************************************************************************/
void SWD_End( void );

/***************************************************************************
SWD_FillText () - Fills text from GLB intro an AREA
 ***************************************************************************/
void SWD_FillText(
    FONT* font, // INPUT : pointer to FONT
    DWORD item, // INPUT : GLB text Item
    int color, // INPUT : field color
    int x, // INPUT : x position
    int y, // INPUT : y position
    int lx, // INPUT : width of field
    int ly // INPUT : height of field
);

/***************************************************************************
 SWD_InitWindow() - Adds window to list and initializes
 ***************************************************************************/
int // RETURN: handle to window
SWD_InitWindow(
    DWORD handle // INPUT : GLB Item Number
);

/***************************************************************************
   SWD_InitMasterWindow () - Inits the master window (must be full screen)
 ***************************************************************************/
int SWD_InitMasterWindow(
    DWORD handle // INPUT : GLB Item Number
);

/***************************************************************************
   SWD_SetWinDrawFunc () - Function called after window is drawn
 ***************************************************************************/
void SWD_SetWinDrawFunc(
    int handle, // INPUT :handle of window
    void ( *infunc )( SWD_DLG* ) // INPUT :pointer to function
);

/***************************************************************************
SWD_SetClearFlag() - Turns ON/OFF memsetting of display buffer in showallwins
 ***************************************************************************/
void SWD_SetClearFlag( BOOL inflag );

/***************************************************************************
 SWD_ShowAllWindows()- Displays all windows & puts active window on top
 ***************************************************************************/
BOOL // RETURN : TRUE = OK, FALSE = Error
SWD_ShowAllWindows( void );

/***************************************************************************
   SWD_SetViewDrawHook () Sets function to draw after the master window
 ***************************************************************************/
void SWD_SetViewDrawHook(
    void ( *func )( void ) // INPUT : pointer to function
);

/***************************************************************************
SWD_SetWindowPtr() - Sets pointer to center of active field
 ***************************************************************************/
void SWD_SetWindowPtr(
    int handle // INPUT : number/handle of window
);

/***************************************************************************
SWD_SetFieldPtr () - Sets pointer on a field
 ***************************************************************************/
void SWD_SetFieldPtr(
    int handle, // INPUT : number/handle of window
    int field // INPUT : field
);

/***************************************************************************
 SWD_SetActiveWindow() - Sets the current working window
 ***************************************************************************/
void SWD_SetActiveWindow(
    int handle // INPUT : number/handle of window
);

/***************************************************************************
 SWD_SetActiveField() - Sets the current working field
 ***************************************************************************/
void SWD_SetActiveField(
    int handle, // INPUT : handle of window
    int field_id // INPUT : number/handle of field
);

/***************************************************************************
 SWD_DestroyWindow() - removes a window from SWD system
 ***************************************************************************/
void SWD_DestroyWindow(
    int handle // INPUT : handle of window
);

/***************************************************************************
   SWD_Dialog () - performs all window in/out/display/move stuff
 ***************************************************************************/
void SWD_Dialog(
    SWD_DLG* swd_dlg // OUTPUT: pointer to info structure
);

/***************************************************************************
   SWD_SetWindowLock() - Locks window so no others can be selected
 ***************************************************************************/
void SWD_SetWindowLock(
    int handle, // INPUT : handle to window
    BOOL lock // INPUT : TRUE/FALSE
);

/***************************************************************************
 SWD_SetWindowXY() - Sets the window x,y position
 ***************************************************************************/
int // RETURN: window opt flag
SWD_SetWindowXY(
    int handle, // INPUT : handle to window
    int xpos, // INPUT : x position
    int ypos // INPUT : y position
);

/***************************************************************************
 SWD_GetWindowXYL () - gets the window x,y, x length, y length
 ***************************************************************************/
int // RETURN: window opt flag
SWD_GetWindowXYL(
    int handle, // INPUT : handle to window
    int* xpos, // OUTPUT: x position
    int* ypos, // OUTPUT: y position
    int* lx, // OUTPUT: x length
    int* ly // OUTPUT: y length
);

/***************************************************************************
 SWD_GetFieldText() - Gets the field text
 ***************************************************************************/
int // RETURN: text max length
SWD_GetFieldText(
    int handle, // INPUT : window handle
    int field_id, // INPUT : field handle
    char* out_text // OUTPUT: text
);

/***************************************************************************
 SWD_SetFieldText() - Sets the default field text
 ***************************************************************************/
int // RETURN: text max length
SWD_SetFieldText(
    int handle, // INPUT : window handle
    int field_id, // INPUT : field handle
    char* in_text // OUTPUT: pointer to string
);

/***************************************************************************
   SWD_GetFieldValue () Returns int value of field text string
 ***************************************************************************/
int SWD_GetFieldValue(
    int handle, // INPUT : window handle
    int field_id // INPUT : field handle
);

/***************************************************************************
   SWD_SetFieldValue () Sets int value into field text
 ***************************************************************************/
int SWD_SetFieldValue(
    int handle, // INPUT : window handle
    int field_id, // INPUT : field handle
    int num // INPUT : number to set in fld text
);

/***************************************************************************
SWD_SetFieldSelect() - Sets field selectable status
 ***************************************************************************/
void SWD_SetFieldSelect(
    int handle, // INPUT : window handle
    int field_id, // INPUT : field handle
    BOOL opt // INPUT : TRUE, FALSE
);

/***************************************************************************
 SWD_GetFieldMark() - Gets the field mark status ( TRUE or FALSE )
 ***************************************************************************/
BOOL // RETURN: mark status ( TRUE, FALSE )
SWD_GetFieldMark(
    int handle, // INPUT : window handle
    int field_id // INPUT : field handle
);

/***************************************************************************
 SWD_GetFieldInputOpt() - Gets the field InputOpt status
 ***************************************************************************/
INPUTOPT // RETURN: InputOpt status
SWD_GetFieldInputOpt(
    int handle, // INPUT : window handle
    int field_id // INPUT : field handle
);

/***************************************************************************
 SWD_SetFieldInputOpt() - Sets the field InputOpt ( button )
 ***************************************************************************/
INPUTOPT // RETURN: InputOpt status
SWD_SetFieldInputOpt(
    int handle, // INPUT : window handle
    int field_id, // INPUT : field handle
    INPUTOPT opt // INPUT : input option
);

/***************************************************************************
   SWD_SetFieldItem () - Sets field Item ID ( picture )
 ***************************************************************************/
void SWD_SetFieldItem(
    int handle, // INPUT : window handle
    int field_id, // INPUT : field handle
    DWORD item // INPUT : GLB item id
);

/***************************************************************************
   SWD_GetFieldItem () - Returns field item number
 ***************************************************************************/
DWORD // RETURN: Item GLB ID
SWD_GetFieldItem(
    int handle, // INPUT : window handle
    int field_id // INPUT : field handle
);

/***************************************************************************
   SWD_SetFieldItemName () - Sets Field Item Name and Loads it in
 ***************************************************************************/
void SWD_SetFieldItemName(
    int handle, // INPUT : window handle
    int field_id, // INPUT : field handle
    char* item_name // INPUT : pointer to Item Name
);

/***************************************************************************
   SWD_GetFieldItemName () - Gets Field Item Name
 ***************************************************************************/
void SWD_GetFieldItemName(
    int handle, // INPUT : window handle
    int field_id, // INPUT : field handle
    char* item_name // OUTPUT: pointer to Item Name
);

/***************************************************************************
 SWD_SetFieldMark() - Sets the Field Mark ( button )
 ***************************************************************************/
void SWD_SetFieldMark(
    int handle, // INPUT : window handle
    int field_id, // INPUT : field handle
    BOOL opt // INPUT : TRUE, FALSE
);

/***************************************************************************
   SWD_SetWindowID () - Sets Window ID number
 ***************************************************************************/
int // RETURN: old Window ID
SWD_SetWindowID(
    int handle, // INPUT : window handle
    int id // INPUT : NEW window ID
);

/***************************************************************************
   SWD_GetWindowID () - Returns Window ID number
 ***************************************************************************/
int SWD_GetWindowID(
    int handle // INPUT : window handle
);

/***************************************************************************
SWD_SetWindowFlag () - Sets A window to be turned on/off
 ***************************************************************************/
int SWD_SetWindowFlag(
    int handle, // INPUT : window handle
    BOOL flag // INPUT : TRUE/FALSE
);

/***************************************************************************
   SWD_SetWindowType () Sets Window TYPE number
 ***************************************************************************/
int // RETURN: old Window TYPE
SWD_SetWindowType(
    int handle, // INPUT : window handle
    int type // INPUT : NEW window TYPE
);

/***************************************************************************
   SWD_GetWindowType () - Returns Window TYPE number
 ***************************************************************************/
int // RETURN: window TYPE
SWD_GetWindowType(
    int handle // INPUT : window handle
);

/***************************************************************************
   SWD_GetFieldXYL () Gets Field X,Y, WIDTH, HEIGHT
 ***************************************************************************/
int SWD_GetFieldXYL(
    int handle, // INPUT : window handle
    int field_id, // INPUT : field handle
    int* x, // OUTPUT: x
    int* y, // OUTPUT: y
    int* lx, // OUTPUT: width
    int* ly // OUTPUT: height
);

#endif
