#ifndef OBJECTS_H
#define OBJECTS_H

#include "../gfx/types.h"
#include "fx.h"

#define MAX_OBJS 20

typedef enum { OBJ_GOTIT, OBJ_NOMONEY, OBJ_SHIPFULL, OBJ_ERROR } BUYSTUFF;

typedef enum {
    S_FORWARD_GUNS, // 00 * DUAL MACHINE GUNS
    S_PLASMA_GUNS, // 01 * PLASMA GUNS
    S_MICRO_MISSLE, // 02 * SMALL WING MISSLES
    S_DUMB_MISSLE, // 03 DUMB FIRE MISSLE         // 1
    S_MINI_GUN, // 04 AUTO TRACK MINI GUN      // 2
    S_TURRET, // 05 AUTO TRACK LASER TURRET  // 3
    S_MISSLE_PODS, // 06 MULTIPLE MISSLE SHOTS    // 4
    S_AIR_MISSLE, // 07 AIR TO AIR MISSLE        // 5
    S_GRD_MISSLE, // 08 AIR TO GROUND            // 6
    S_BOMB, // 09 GROUND BOMB              // 7
    S_ENERGY_GRAB, // 10 ENERGY GRAB              // 8
    S_MEGA_BOMB, // 11 MEGA BOMB KILLS ALL AIR  //
    S_PULSE_CANNON, // 12 WAVE WEAPON              // 9
    S_FORWARD_LASER, // 13 ALTERNATING LASER        // MINUS
    S_DEATH_RAY, // 14 DEATH RAY                // 0
    S_SUPER_SHIELD, // 15 SUPER SHIELD             //
    S_ENERGY, // 16 NORMAL SHIELD ENERGY     //
    S_DETECT, // 18 DAMAGE DETECTOR          //
    S_ITEMBUY1, // 19
    S_ITEMBUY2, // 20
    S_ITEMBUY3, // 21
    S_ITEMBUY4, // 22
    S_ITEMBUY5, // 23
    S_ITEMBUY6, // 24
    S_LAST_OBJECT // 25 LAST OBJECT ( NOT USED )
} OBJ_TYPE;

#define LAST_WEAPON   ( S_DEATH_RAY )
#define FIRST_SPECIAL ( S_DUMB_MISSLE )

typedef struct {
    DWORD item; // LUMPNUM OF PIC
    int numframes; // NUMBER OF FRAMES
    int cost; // COST OF OBJECT
    int start_cnt; // START AMOUNT OF ITEMS
    int max_cnt; // MAX COUNT OF UNITS/ITEMS
    bool ( *actf )( OBJ_TYPE ); // FUNCTION PTR FOR ACTION
    DEFX fxtype; // SOUND FX TYPE
    bool forever; // LAST FOREVER
    bool onlyflag; // UNIQUE FLAG ( ENERGY )
    bool loseit; //
    bool specialw; // SPECIAL WEAPON
    bool moneyflag; // ADDS MONEY TO PLAYER WHEN PICKED UP
    bool game1flag; // true=AVALABLE IN ALL GAMES
    bool shieldflag; // SHOW SHIELD WHEN BONUS
} OBJ_LIB;

// note: this is read from save files; sizeof() = 6 * 4 = 24
typedef struct OBJ_S {
    struct OBJ_S* prev; // LINK LIST PREV
    struct OBJ_S* next; // LINK LIST NEXT
    int num; // NUMBER OF ITEM(S)
    int type; // OBJECT TYPE
    OBJ_LIB* lib; // POINTER TO LIB
    uint32_t inuse; // is currently equipped? (bool)
} OBJ;

/***************************************************************************
OBJS_Clear () - Clears out All Objects
 ***************************************************************************/
void OBJS_Clear( void );

/***************************************************************************
OBJS_CachePics () - PreLoad bonus/object picstures
 ***************************************************************************/
void OBJS_CachePics( void );

/***************************************************************************
OBJS_FreePics () - Free bonus/object picstures
 ***************************************************************************/
void OBJS_FreePics( void );

/***************************************************************************
OBJS_Init () - Sets up object stuff
 ***************************************************************************/
void OBJS_Init( void );

/***************************************************************************
OBJS_DisplayStats() - Display game screen object stuff
 ***************************************************************************/
void OBJS_DisplayStats( void );

/***************************************************************************
OBJS_Del () - Removes Object From User Posession
 ***************************************************************************/
void OBJS_Del(
    OBJ_TYPE type // INPUT : OBJ type
);

/***************************************************************************
OBJS_Load () - Adds new OBJ from OBJ
 ***************************************************************************/
bool OBJS_Load(
    OBJ* inobj // INPUT : OBJ type
);

/***************************************************************************
OBJS_Add () - Adds OBJ ( type ) to players possesions
 ***************************************************************************/
BUYSTUFF OBJS_Add(
    OBJ_TYPE type // INPUT : OBJ type
);

/***************************************************************************
OBJS_GetNext () - Sets plr.sweapon to next avalable weapon
 ***************************************************************************/
void OBJS_GetNext( void );

/***************************************************************************
OBJS_Use () - Player Use An Object
 ***************************************************************************/
void OBJS_Use(
    OBJ_TYPE type // INPUT : OBJ type
);

/***************************************************************************
OBJS_Sell () - Sell Object from player posesion
 ***************************************************************************/
int // RETRUN: amount left
OBJS_Sell(
    OBJ_TYPE type // INPUT : OBJ type
);

/***************************************************************************
OBJS_Buy () - Add Amount from TYPE that is equiped ( BUY )
 ***************************************************************************/
BUYSTUFF OBJS_Buy(
    OBJ_TYPE type // INPUT : OBJ type
);

/***************************************************************************
OBJS_SubAmt () - Subtract Amount From Equiped Item
 ***************************************************************************/
int // RETURN: return nums in OBJ
OBJS_SubAmt(
    OBJ_TYPE type, // INPUT : OBJ type
    int amt // INPUT : amount to subtract
);

/***************************************************************************
OBJS_GetAmt() - Returns number of items within TYPE in Equiped Items
 ***************************************************************************/
int // RETURN: return nums in OBJ
OBJS_GetAmt(
    OBJ_TYPE type // INPUT : OBJ type
);

/***************************************************************************
OBJS_GetTotal() - Returns number of items within TYPE in all OBJS
 ***************************************************************************/
int // RETURN: return nums in OBJ
OBJS_GetTotal(
    OBJ_TYPE type // INPUT : OBJ type
);

/***************************************************************************
OBJS_IsOnly () - Is Onlyflag set
 ***************************************************************************/
bool OBJS_IsOnly(
    OBJ_TYPE type // INPUT : OBJ type
);

/***************************************************************************
OBJS_GetCost () - Returns The game COST of an object
 ***************************************************************************/
int // RETURN: cost of object
OBJS_GetCost(
    OBJ_TYPE type // INPUT : OBJ type
);

/***************************************************************************
OBJS_GetResale () - Returns The game Resale Value of an object
 ***************************************************************************/
int // RETURN: cost of object
OBJS_GetResale(
    OBJ_TYPE type // INPUT : OBJ type
);

/***************************************************************************
OBJS_CanBuy() - Returns true if player can buy object
 ***************************************************************************/
bool OBJS_CanBuy(
    OBJ_TYPE type // INPUT : OBJ type
);

/***************************************************************************
OBJS_CanSell() - Returns true if player can Sell object
 ***************************************************************************/
bool OBJS_CanSell(
    OBJ_TYPE type // INPUT : OBJ type
);

/***************************************************************************
OBJS_GetNum () - Returns number of Objects that player has
 ***************************************************************************/
int // RETURN: number of objects
    OBJS_GetNum( void );

/***************************************************************************
OBJS_GetLib () - Returns Pointer to Lib Object
 ***************************************************************************/
OBJ_LIB* OBJS_GetLib(
    OBJ_TYPE type // INPUT : OBJ type
);

/***************************************************************************
OBJS_IsEquip() - Returns true if item is Equiped
 ***************************************************************************/
bool // RETURN: return nums in OBJ
OBJS_IsEquip(
    OBJ_TYPE type // INPUT : OBJ type
);

/***************************************************************************
OBJS_AddEnergy()
 ***************************************************************************/
int // RETURN: return nums in OBJ
OBJS_AddEnergy(
    int amt // INPUT : amount to add
);

/***************************************************************************
OBJS_SubEnergy()
 ***************************************************************************/
int // RETURN: return nums in OBJ
OBJS_SubEnergy(
    int amt // INPUT : amount to subtract
);

/***************************************************************************
OBJS_LoseObj() - Lose random object
 ***************************************************************************/
bool OBJS_LoseObj( void );

/***************************************************************************
OBJS_Think () - Does all in game thinking ( recharing )
 ***************************************************************************/
void OBJS_Think( void );

/***************************************************************************
OBJS_MakeSpecial() - Makes the selected weapon the current special
 ***************************************************************************/
bool OBJS_MakeSpecial(
    OBJ_TYPE type // INPUT : OBJ type
);

#endif
