

#include "public.h"

#include "inc/file0001.inc"

#include "anims.h"
#include "objects.h"
#include "shots.h"

PRIVATE OBJ objs[MAX_OBJS];

OBJ first_objs;
OBJ last_objs;
OBJ* free_objs;
PRIVATE int obj_cnt;
OBJ_LIB obj_lib[S_LAST_OBJECT];
OBJ* p_objs[S_LAST_OBJECT];

PRIVATE bool objuse_flag;
PRIVATE int think_cnt;

#define CHARGE_SHIELD ( 24 * 4 )

/***************************************************************************
OBJS_Clear () - Clears out All Objects
 ***************************************************************************/
void OBJS_Clear( void ) {
    int i;

    obj_cnt = 0;

    first_objs.prev = NULL;
    first_objs.next = &last_objs;

    last_objs.prev = &first_objs;
    last_objs.next = NULL;

    free_objs = objs;

    memset( objs, 0, sizeof( objs ) );
    memset( p_objs, 0, sizeof( p_objs ) );

    for ( i = 0; i < MAX_OBJS - 1; i++ ) {
        objs[i].next = &objs[i + 1];
    }
}

/*-------------------------------------------------------------------------*
OBJS_Get () - Gets A Free OBJ from Link List
 *-------------------------------------------------------------------------*/
PRIVATE OBJ* OBJS_Get( void ) {
    OBJ* pNew;

    if ( !free_objs ) {
        return NULL;
    }

    pNew = free_objs;
    free_objs = free_objs->next;

    memset( pNew, 0, sizeof( OBJ ) );

    pNew->next = &last_objs;
    pNew->prev = last_objs.prev;
    last_objs.prev = pNew;
    pNew->prev->next = pNew;

    obj_cnt++;
    return pNew;
}

/*-------------------------------------------------------------------------*
OBJS_Remove () Removes OBJ from Link List
 *-------------------------------------------------------------------------*/
PRIVATE OBJ* OBJS_Remove( OBJ* sh ) {
    OBJ* next;

    next = sh->prev;

    sh->next->prev = sh->prev;
    sh->prev->next = sh->next;

    memset( sh, 0, sizeof( OBJ ) );

    sh->next = free_objs;

    free_objs = sh;

    obj_cnt--;

    return next;
}

/***************************************************************************
OBJS_CachePics () - PreLoad bonus/object pictures
 ***************************************************************************/
void OBJS_CachePics( void ) {
    OBJ_LIB* lib;
    int j, i;

    for ( j = 0; j < S_LAST_OBJECT; j++ ) {
        lib = &obj_lib[j];

        if ( lib && lib->item != EMPTY ) {
            for ( i = 0; i < lib->numframes; i++ ) {
                GLB_CacheItem( lib->item + (DWORD) i );
            }
        }
    }

    GLB_CacheItem( SMSHIELD_PIC );
    GLB_CacheItem( SMBOMB_PIC );
}

/***************************************************************************
OBJS_FreePics () - Free bonus/object picstures
 ***************************************************************************/
void OBJS_FreePics( void ) {
    OBJ_LIB* lib;
    int j, i;

    for ( j = 0; j < S_LAST_OBJECT; j++ ) {
        lib = &obj_lib[j];

        if ( lib && lib->item != EMPTY ) {
            for ( i = 0; i < lib->numframes; i++ ) {
                GLB_FreeItem( lib->item + (DWORD) i );
            }
        }
    }

    GLB_FreeItem( SMSHIELD_PIC );
    GLB_FreeItem( SMBOMB_PIC );
}

/***************************************************************************
OBJS_Init () - Sets up object stuff
 ***************************************************************************/
void OBJS_Init( void ) {
    OBJ_LIB* lib;

    OBJS_Clear();

    memset( obj_lib, 0, sizeof( obj_lib ) );
    memset( p_objs, 0, sizeof( p_objs ) );

    // == FORWARD GUNS ===  *
    lib = &obj_lib[S_FORWARD_GUNS];
    lib->cost = 12000;
    lib->start_cnt = 1;
    lib->max_cnt = 1;
    lib->actf = SHOTS_PlayerShoot;
    lib->item = BONUS00_PIC;
    lib->numframes = 1;
    lib->forever = true;
    lib->onlyflag = false;
    lib->loseit = false;
    lib->specialw = false;
    lib->fxtype = FX_GUN;
    lib->moneyflag = false;
    lib->game1flag = true;
    lib->shieldflag = true;

    // == PLASMA GUNS ===  *
    lib = &obj_lib[S_PLASMA_GUNS];
    lib->cost = 78800;
    lib->start_cnt = 1;
    lib->max_cnt = 1;
    lib->actf = SHOTS_PlayerShoot;
    lib->item = BONUS01_PIC;
    lib->numframes = 2;
    lib->forever = true;
    lib->onlyflag = false;
    lib->loseit = true;
    lib->specialw = false;
    lib->fxtype = FX_GUN;
    lib->moneyflag = false;
    lib->game1flag = true;
    lib->shieldflag = true;

    // == MICRO MISSLES ===  *
    lib = &obj_lib[S_MICRO_MISSLE];
    lib->cost = 175600;
    lib->start_cnt = 1;
    lib->max_cnt = 1;
    lib->actf = SHOTS_PlayerShoot;
    lib->item = BONUS02_PIC;
    lib->numframes = 2;
    lib->forever = true;
    lib->onlyflag = false;
    lib->loseit = true;
    lib->specialw = false;
    lib->fxtype = FX_GUN;
    lib->moneyflag = false;
    lib->game1flag = true;
    lib->shieldflag = true;

    // == DUMB MISSLES === *
    lib = &obj_lib[S_DUMB_MISSLE];
    lib->cost = 145200;
    lib->start_cnt = 1;
    lib->max_cnt = 1;
    lib->actf = SHOTS_PlayerShoot;
    lib->item = BONUS03_PIC;
    lib->numframes = 1;
    lib->forever = true;
    lib->onlyflag = false;
    lib->loseit = true;
    lib->specialw = true;
    lib->fxtype = FX_MISSLE;
    lib->moneyflag = false;
    lib->game1flag = true;
    lib->shieldflag = true;

    // == MINI GUN ===
    lib = &obj_lib[S_MINI_GUN];
    lib->cost = 250650;
    lib->start_cnt = 1;
    lib->max_cnt = 1;
    lib->actf = SHOTS_PlayerShoot;
    lib->item = BONUS04_PIC;
    lib->numframes = 4;
    lib->forever = true;
    lib->onlyflag = false;
    lib->loseit = true;
    lib->specialw = true;
    lib->fxtype = FX_GUN;
    lib->moneyflag = false;
    lib->game1flag = true;
    lib->shieldflag = true;

    // == LASER TURRET ===
    lib = &obj_lib[S_TURRET];
    lib->cost = 512850;
    lib->start_cnt = 1;
    lib->max_cnt = 1;
    lib->actf = SHOTS_PlayerShoot;
    lib->item = BONUS05_PIC;
    lib->numframes = 4;
    lib->forever = true;
    lib->onlyflag = false;
    lib->loseit = true;
    lib->specialw = true;
    lib->fxtype = FX_LASER;
    lib->moneyflag = false;
    lib->game1flag = false;
    lib->shieldflag = true;

    // == MISSLE PODS ===
    lib = &obj_lib[S_MISSLE_PODS];
    lib->cost = 204950;
    lib->start_cnt = 1;
    lib->max_cnt = 1;
    lib->actf = SHOTS_PlayerShoot;
    lib->item = BONUS06_PIC;
    lib->numframes = 1;
    lib->forever = true;
    lib->onlyflag = false;
    lib->loseit = true;
    lib->specialw = true;
    lib->fxtype = FX_GUN;
    lib->moneyflag = false;
    lib->game1flag = true;
    lib->shieldflag = true;

    // == AIR TO AIR MISSLES === *
    lib = &obj_lib[S_AIR_MISSLE];
    lib->cost = 63500;
    lib->start_cnt = 1;
    lib->max_cnt = 1;
    lib->actf = SHOTS_PlayerShoot;
    lib->item = BONUS07_PIC;
    lib->numframes = 1;
    lib->forever = true;
    lib->onlyflag = false;
    lib->loseit = true;
    lib->specialw = true;
    lib->fxtype = FX_MISSLE;
    lib->moneyflag = false;
    lib->game1flag = true;
    lib->shieldflag = true;

    // == AIR TO GROUND MISSLES === *
    lib = &obj_lib[S_GRD_MISSLE];
    lib->cost = 110000;
    lib->start_cnt = 1;
    lib->max_cnt = 1;
    lib->actf = SHOTS_PlayerShoot;
    lib->item = BONUS08_PIC;
    lib->numframes = 1;
    lib->forever = true;
    lib->onlyflag = false;
    lib->loseit = true;
    lib->specialw = true;
    lib->fxtype = FX_MISSLE;
    lib->moneyflag = false;
    lib->game1flag = true;
    lib->shieldflag = true;

    // == GROUND BOMB ===
    lib = &obj_lib[S_BOMB];
    lib->cost = 98200;
    lib->start_cnt = 1;
    lib->max_cnt = 1;
    lib->actf = SHOTS_PlayerShoot;
    lib->item = BONUS21_PIC;
    lib->numframes = 1;
    lib->forever = true;
    lib->onlyflag = false;
    lib->loseit = true;
    lib->specialw = true;
    lib->fxtype = FX_MISSLE;
    lib->moneyflag = false;
    lib->game1flag = false;
    lib->shieldflag = true;

    // == ENERGY GRAB ===
    lib = &obj_lib[S_ENERGY_GRAB];
    lib->cost = 300750;
    lib->start_cnt = 1;
    lib->max_cnt = 1;
    lib->actf = SHOTS_PlayerShoot;
    lib->item = BONUS09_PIC;
    lib->numframes = 4;
    lib->forever = true;
    lib->onlyflag = false;
    lib->loseit = true;
    lib->specialw = true;
    lib->fxtype = FX_GUN;
    lib->moneyflag = false;
    lib->game1flag = false;
    lib->shieldflag = true;

    // == MEGA BOMB === *
    lib = &obj_lib[S_MEGA_BOMB];
    lib->cost = 32250;
    lib->start_cnt = 1;
    lib->max_cnt = 5;
    lib->actf = SHOTS_PlayerShoot;
    lib->item = BONUS10_PIC;
    lib->numframes = 1;
    lib->forever = false;
    lib->onlyflag = true;
    lib->loseit = true;
    lib->specialw = false;
    lib->fxtype = FX_GUN;
    lib->moneyflag = false;
    lib->game1flag = true;
    lib->shieldflag = true;

    // == PULSE CANNON ===
    lib = &obj_lib[S_PULSE_CANNON];
    lib->cost = 725000;
    lib->start_cnt = 1;
    lib->max_cnt = 1;
    lib->actf = SHOTS_PlayerShoot;
    lib->item = BONUS11_PIC;
    lib->numframes = 2;
    lib->forever = true;
    lib->onlyflag = false;
    lib->loseit = true;
    lib->specialw = true;
    lib->fxtype = FX_GUN;
    lib->moneyflag = false;
    lib->game1flag = true;
    lib->shieldflag = true;

    // == FORWARD LASER ===
    lib = &obj_lib[S_FORWARD_LASER];
    lib->cost = 1750000;
    lib->start_cnt = 1;
    lib->max_cnt = 1;
    lib->actf = SHOTS_PlayerShoot;
    lib->item = BONUS12_PIC;
    lib->numframes = 4;
    lib->forever = true;
    lib->onlyflag = false;
    lib->loseit = true;
    lib->specialw = true;
    lib->fxtype = FX_LASER;
    lib->moneyflag = false;
    lib->game1flag = false;
    lib->shieldflag = true;

    // == DEATH RAY ===
    lib = &obj_lib[S_DEATH_RAY];
    lib->cost = 950000;
    lib->start_cnt = 1;
    lib->max_cnt = 1;
    lib->actf = SHOTS_PlayerShoot;
    lib->item = BONUS13_PIC;
    lib->numframes = 4;
    lib->forever = true;
    lib->onlyflag = false;
    lib->loseit = true;
    lib->specialw = true;
    lib->fxtype = FX_LASER;
    lib->moneyflag = false;
    lib->game1flag = false;
    lib->shieldflag = true;

    // == SUPER SHIELD ===
    lib = &obj_lib[S_SUPER_SHIELD];
    lib->cost = 78500;
    lib->start_cnt = MAX_SHIELD;
    lib->max_cnt = MAX_SHIELD;
    lib->item = BONUS14_PIC;
    lib->numframes = 1;
    lib->forever = false;
    lib->onlyflag = false;
    lib->loseit = false;
    lib->specialw = false;
    lib->fxtype = EMPTY;
    lib->moneyflag = false;
    lib->game1flag = true;
    lib->shieldflag = true;

    // == SHIP NORM ENERGY ===
    lib = &obj_lib[S_ENERGY];
    lib->cost = 400;
    lib->start_cnt = MAX_SHIELD / 4;
    lib->max_cnt = MAX_SHIELD;
    lib->item = BONUS15_PIC;
    lib->numframes = 4;
    lib->forever = true;
    lib->onlyflag = true;
    lib->loseit = false;
    lib->specialw = false;
    lib->fxtype = EMPTY;
    lib->moneyflag = false;
    lib->game1flag = true;
    lib->shieldflag = true;

    // == ENEMY DAMAGE DETECTOR ===
    lib = &obj_lib[S_DETECT];
    lib->cost = 10000;
    lib->start_cnt = 1;
    lib->max_cnt = 1;
    lib->item = BONUS16_PIC;
    lib->numframes = 1;
    lib->forever = false;
    lib->onlyflag = true;
    lib->loseit = false;
    lib->specialw = false;
    lib->fxtype = EMPTY;
    lib->moneyflag = false;
    lib->game1flag = true;
    lib->shieldflag = false;

    // == BUY ITEM 1 ===
    lib = &obj_lib[S_ITEMBUY1];
    lib->cost = 93800;
    lib->start_cnt = lib->cost;
    lib->max_cnt = lib->cost;
    lib->item = BONUS16_PIC;
    lib->numframes = 1;
    lib->forever = false;
    lib->onlyflag = false;
    lib->loseit = true;
    lib->specialw = false;
    lib->fxtype = EMPTY;
    lib->moneyflag = true;
    lib->game1flag = true;
    lib->shieldflag = true;

    // == BUY ITEM 2 ===
    lib = &obj_lib[S_ITEMBUY2];
    lib->cost = 76000;
    lib->start_cnt = lib->cost;
    lib->max_cnt = lib->cost;
    lib->item = BONUS17_PIC;
    lib->numframes = 1;
    lib->forever = false;
    lib->onlyflag = false;
    lib->loseit = true;
    lib->specialw = false;
    lib->fxtype = EMPTY;
    lib->moneyflag = true;
    lib->game1flag = true;
    lib->shieldflag = true;

    // == BUY ITEM 3 ===
    lib = &obj_lib[S_ITEMBUY3];
    lib->cost = 55700;
    lib->start_cnt = lib->cost;
    lib->max_cnt = lib->cost;
    lib->item = BONUS18_PIC;
    lib->numframes = 1;
    lib->forever = false;
    lib->onlyflag = false;
    lib->loseit = true;
    lib->specialw = false;
    lib->fxtype = EMPTY;
    lib->moneyflag = true;
    lib->game1flag = true;
    lib->shieldflag = true;

    // == BUY ITEM 4 ===
    lib = &obj_lib[S_ITEMBUY4];
    lib->cost = 35200;
    lib->start_cnt = lib->cost;
    lib->max_cnt = lib->cost;
    lib->item = BONUS19_PIC;
    lib->numframes = 1;
    lib->forever = false;
    lib->onlyflag = false;
    lib->loseit = true;
    lib->specialw = false;
    lib->fxtype = EMPTY;
    lib->moneyflag = true;
    lib->game1flag = true;
    lib->shieldflag = true;

    // == BUY ITEM 5 ===
    lib = &obj_lib[S_ITEMBUY5];
    lib->cost = 122500;
    lib->start_cnt = lib->cost;
    lib->max_cnt = lib->cost;
    lib->item = BONUS20_PIC;
    lib->numframes = 1;
    lib->forever = false;
    lib->onlyflag = false;
    lib->loseit = true;
    lib->specialw = false;
    lib->fxtype = EMPTY;
    lib->moneyflag = true;
    lib->game1flag = true;
    lib->shieldflag = true;

    // == BUY ITEM 6 ===
    lib = &obj_lib[S_ITEMBUY6];
    lib->cost = 50;
    lib->start_cnt = lib->cost;
    lib->max_cnt = lib->cost;
    lib->item = BONUS22_PIC;
    lib->numframes = 4;
    lib->forever = false;
    lib->onlyflag = false;
    lib->loseit = true;
    lib->specialw = false;
    lib->fxtype = EMPTY;
    lib->moneyflag = true;
    lib->game1flag = true;
    lib->shieldflag = false;
}

/***************************************************************************
OBJS_DisplayStats() - Display game screen object stuff
 ***************************************************************************/
void OBJS_DisplayStats( void ) {
    static int dpos = 0;
    DWORD item;
    int i;
    int x;
    int maxloop;

    if ( p_objs[S_DETECT] ) {
        i = ENEMY_GetBaseDamage();
        if ( i > 0 ) {
            GFX_ColorBox( 109, MAP_BOTTOM + 9, 102, 8, 74 );
            GFX_ColorBox( 110, MAP_BOTTOM + 10, i, 6, 68 );
        } else {
            GFX_VLine( 110 + dpos, MAP_BOTTOM + 8, 3, 68 );
            GFX_VLine( 110 + 99 - dpos, MAP_BOTTOM + 8, 3, 68 );
            dpos++;
            dpos = dpos % 50;
        }
    }

    if ( plr.sweapon != EMPTY ) {
        item = obj_lib[plr.sweapon].item;
        GFX_PutSprite( GLB_GetItem( item ), MAP_RIGHT - 18, MAP_TOP );
    }

    if ( p_objs[S_SUPER_SHIELD] ) {
        x = MAP_LEFT + 2;
        maxloop = OBJS_GetTotal( S_SUPER_SHIELD );
        for ( i = 0; i < maxloop; i++ ) {
            GFX_PutSprite( GLB_GetItem( SMSHIELD_PIC ), x, 1 );
            x += 13;
        }
    }

    if ( p_objs[S_MEGA_BOMB] ) {
        x = MAP_LEFT + 2;
        for ( i = 0; i < p_objs[S_MEGA_BOMB]->num; i++ ) {
            GFX_PutSprite( GLB_GetItem( SMBOMB_PIC ), x, 199 - 13 );
            x += 13;
        }
    }
}

/*-------------------------------------------------------------------------*
OBJS_Equip () - Equips an OBJ to be used by Player
 *-------------------------------------------------------------------------*/
bool OBJS_Equip(
    OBJ_TYPE type // INPUT: OBJ type
) {
    OBJ* cur;

    for ( cur = first_objs.next; cur != &last_objs; cur = cur->next ) {
        if ( cur->type == type && p_objs[type] == NULL ) {
            cur->inuse = true;
            p_objs[type] = cur;
            return true;
        }
    }

    return false;
}

/***************************************************************************
OBJS_Load () - Adds new OBJ from OBJ
 ***************************************************************************/
bool OBJS_Load(
    OBJ* inobj // INPUT : pointer to OBJ
) {
    OBJ* cur;

    cur = OBJS_Get();
    if ( !cur ) {
        return false;
    }

    cur->num = inobj->num;
    cur->type = inobj->type;
    cur->lib = &obj_lib[inobj->type];
    cur->inuse = inobj->inuse;

    if ( cur->inuse ) {
        p_objs[inobj->type] = cur;
    }

    return true;
}

/***************************************************************************
OBJS_Add () - Adds OBJ ( type ) to players possesions
 ***************************************************************************/
BUYSTUFF OBJS_Add(
    OBJ_TYPE type // INPUT : OBJ type
) {
    extern int g_oldsuper;
    extern int g_oldshield;
    OBJ* cur;
    OBJ_LIB* lib;

    if ( type >= S_LAST_OBJECT ) {
        return OBJ_ERROR;
    }

    g_oldsuper = EMPTY;
    g_oldshield = EMPTY;

    lib = &obj_lib[type];

    if ( lib->moneyflag ) {
        plr.score += lib->cost;
        return OBJ_GOTIT;
    }

    if ( !reg_flag ) {
        if ( !lib->game1flag ) {
            return OBJ_GOTIT;
        }
    }

    if ( lib->onlyflag ) {
        for ( cur = first_objs.next; cur != &last_objs; cur = cur->next ) {
            if ( cur->type == type ) {
                if ( cur->num >= lib->max_cnt ) {
                    return OBJ_SHIPFULL;
                }

                cur->num += lib->start_cnt;
                if ( cur->num > lib->max_cnt ) {
                    cur->num = lib->max_cnt;
                }

                return OBJ_GOTIT;
            }
        }
    }

    cur = OBJS_Get();
    if ( !cur ) {
        return OBJ_SHIPFULL;
    }

    cur->num = lib->start_cnt;
    cur->type = type;
    cur->lib = lib;

    // == equip item if needed =====
    if ( p_objs[type] == NULL ) {
        cur->inuse = true;
        p_objs[type] = cur;
        if ( plr.sweapon == EMPTY && lib->specialw ) {
            plr.sweapon = type;
        }
    }

    return OBJ_GOTIT;
}

/***************************************************************************
OBJS_Del () - Removes Object From User Posession
 ***************************************************************************/
void OBJS_Del(
    OBJ_TYPE type // INPUT : OBJ type
) {
    OBJ* cur = p_objs[type];

    if ( cur == NULL ) {
        return;
    }

    OBJS_Remove( cur );
    p_objs[type] = NULL;
    OBJS_Equip( type );

    if ( type == plr.sweapon ) {
        OBJS_GetNext();
    }
}

/***************************************************************************
OBJS_GetNext () - Sets plr.sweapon to next avalable weapon
 ***************************************************************************/
void OBJS_GetNext( void ) {
    int i;
    int pos;
    int setval = EMPTY;
    OBJ* cur;

    if ( plr.sweapon < S_DUMB_MISSLE ) {
        pos = S_DUMB_MISSLE;
    } else {
        pos = plr.sweapon + 1;
    }

    for ( i = FIRST_SPECIAL; i <= LAST_WEAPON; i++ ) {
        if ( pos > LAST_WEAPON ) {
            pos = FIRST_SPECIAL;
        }

        cur = p_objs[pos];

        if ( cur && cur->num && cur->lib->specialw ) {
            setval = pos;
            break;
        }

        pos++;
    }

    plr.sweapon = setval;
}

/***************************************************************************
OBJS_Use () - Player Use An Object
 ***************************************************************************/
void OBJS_Use(
    OBJ_TYPE type // INPUT : OBJ type
) {
    OBJ* cur = p_objs[type];
    OBJ_LIB* lib = &obj_lib[type];

    if ( !cur ) {
        return;
    }

    objuse_flag = true;
    think_cnt = 0;

    if ( lib->actf( type ) ) {
        if ( !lib->forever ) {
            cur->num--;
        }
    }

    if ( cur->num <= 0 && !lib->forever ) {
        OBJS_Remove( cur );
        p_objs[type] = NULL;
        OBJS_Equip( type );
        if ( plr.sweapon == type ) {
            OBJS_GetNext();
        }
    }
}

/***************************************************************************
OBJS_Sell () - Sell Object from player posesion
 ***************************************************************************/
int // RETRUN: amount left
OBJS_Sell(
    OBJ_TYPE type // INPUT : OBJ type
) {
    OBJ* cur = p_objs[type];
    OBJ_LIB* lib = &obj_lib[type];
    int rval = 0;

    if ( !cur ) {
        return 0;
    }

    plr.score += OBJS_GetResale( type );

    if ( type == S_DETECT ) {
        p_objs[type] = NULL;
        return 0;
    }

    if ( lib->onlyflag ) {
        cur->num -= lib->start_cnt;

        if ( cur->num <= 0 ) {
            rval = 0;
            cur->num = 0;
            if ( !lib->forever ) {
                OBJS_Remove( cur );
                p_objs[type] = NULL;
                OBJS_Equip( type );
                if ( plr.sweapon == type ) {
                    OBJS_GetNext();
                }
            }
        } else {
            rval = cur->num;
        }
    } else {
        OBJS_Del( type );

        rval = OBJS_GetTotal( type );
    }

    return rval;
}

/***************************************************************************
OBJS_Buy () - Add Amount from TYPE that is equiped ( BUY )
 ***************************************************************************/
BUYSTUFF OBJS_Buy(
    OBJ_TYPE type // INPUT : OBJ type
) {
    BUYSTUFF rval = OBJ_NOMONEY;
    int num;

    if ( type == S_SUPER_SHIELD ) {
        num = OBJS_GetTotal( S_SUPER_SHIELD );
        if ( num >= 5 ) {
            return OBJ_SHIPFULL;
        }
    }

    if ( plr.score >= OBJS_GetCost( type ) ) {
        rval = OBJS_Add( type );

        if ( rval == OBJ_GOTIT ) {
            plr.score -= OBJS_GetCost( type );
        }
    }

    return rval;
}

/***************************************************************************
OBJS_SubAmt () - Subtract Amount From Equiped Item
 ***************************************************************************/
int // RETURN: return nums in OBJ
OBJS_SubAmt(
    OBJ_TYPE type, // INPUT : OBJ type
    int amt // INPUT : amount to subtract
) {
    OBJ* cur = p_objs[type];

    if ( !cur ) {
        return 0;
    }

    cur->num -= amt;

    if ( cur->num < 0 ) {
        cur->num = 0;
    }

    return cur->num;
}

/***************************************************************************
OBJS_GetAmt() - Returns number of items within TYPE in Equiped Items
 ***************************************************************************/
int // RETURN: return nums in OBJ
OBJS_GetAmt(
    OBJ_TYPE type // INPUT : OBJ type
) {
    OBJ* cur = p_objs[type];

    if ( !cur ) {
        return 0;
    }

    return cur->num;
}

/***************************************************************************
OBJS_GetTotal() - Returns number of items within TYPE in all OBJS
 ***************************************************************************/
int // RETURN: return nums in OBJ
OBJS_GetTotal(
    OBJ_TYPE type // INPUT : OBJ type
) {
    OBJ* cur;
    int total;

    total = 0;

    for ( cur = first_objs.next; cur != &last_objs; cur = cur->next ) {
        if ( type == cur->type ) {
            total++;
        }
    }

    return total;
}

/***************************************************************************
OBJS_IsOnly () - Is Onlyflag set
 ***************************************************************************/
bool OBJS_IsOnly(
    OBJ_TYPE type // INPUT : OBJ type
) {
    OBJ_LIB* lib = &obj_lib[type];

    return lib->onlyflag;
}

/***************************************************************************
OBJS_GetCost () - Returns The game COST of an object
 ***************************************************************************/
int // RETURN: cost of object
OBJS_GetCost(
    OBJ_TYPE type // INPUT : OBJ type
) {
    OBJ_LIB* lib = &obj_lib[type];
    int cost;

    if ( !lib ) {
        return 99999999;
    }

    if ( lib->onlyflag ) {
        cost = lib->cost * lib->start_cnt;
    } else {
        cost = lib->cost;
    }

    return cost;
}

/***************************************************************************
OBJS_GetResale () - Returns The game Resale Value of an object
 ***************************************************************************/
int // RETURN: cost of object
OBJS_GetResale(
    OBJ_TYPE type // INPUT : OBJ type
) {
    OBJ* cur = p_objs[type];
    OBJ_LIB* lib = &obj_lib[type];
    int cost;

    if ( !cur ) {
        return 0;
    }

    if ( lib->onlyflag ) {
        cost = lib->cost * lib->start_cnt;
    } else {
        cost = lib->cost;
    }

    cost = cost >> 1;

    return cost;
}

/***************************************************************************
OBJS_CanBuy() - Returns true if player can buy object
 ***************************************************************************/
bool OBJS_CanBuy(
    OBJ_TYPE type // INPUT : OBJ type
) {
    OBJ_LIB* lib = &obj_lib[type];
    int cost;

    if ( type >= S_LAST_OBJECT ) {
        return false;
    }

#if 0
   if ( type == S_SUPER_SHIELD )
   {
      cost = OBJS_GetTotal ( S_SUPER_SHIELD );
      if ( cost >= 5 )
         return false;
   }
#endif

    if ( type == S_FORWARD_GUNS ) {
        if ( OBJS_IsEquip( type ) ) {
            return false;
        }
    }

    if ( !reg_flag ) {
        if ( !lib->game1flag ) {
            return false;
        }
    }

    cost = OBJS_GetCost( type );

    return cost != 0;
}
/***************************************************************************
OBJS_CanSell() - Returns true if player can Sell object
 ***************************************************************************/
bool OBJS_CanSell(
    OBJ_TYPE type // INPUT : OBJ type
) {
    OBJ* cur = p_objs[type];
    OBJ_LIB* lib = &obj_lib[type];

    return type < S_LAST_OBJECT //
        && cur != NULL //
        && ( !lib->onlyflag || type != S_ENERGY || cur->num > lib->start_cnt ) //
        && cur->num >= lib->start_cnt;
}

/***************************************************************************
OBJS_GetNum () - Returns number of Objects that player has
 ***************************************************************************/
int // RETURN: number of objects
OBJS_GetNum( void ) {
    return obj_cnt;
}

/***************************************************************************
OBJS_GetLib () - Returns Pointer to Lib Object
 ***************************************************************************/
OBJ_LIB* OBJS_GetLib(
    OBJ_TYPE type // INPUT : OBJ type
) {
    OBJ_LIB* lib = &obj_lib[type];

    return lib;
}

/***************************************************************************
OBJS_IsEquip() - Returns true if item is Equiped
 ***************************************************************************/
bool // RETURN: return nums in OBJ
OBJS_IsEquip(
    OBJ_TYPE type // INPUT : OBJ type
) {
    return p_objs[type];
}

/***************************************************************************
OBJS_SubEnergy()
 ***************************************************************************/
int // RETURN: return nums in OBJ
OBJS_SubEnergy(
    int amt // INPUT : amount to subtract
) {
    extern bool godmode;
    OBJ* cur;

    if ( godmode ) {
        return 0;
    }

    if ( startendwave != EMPTY ) {
        return 0;
    }

    cur = p_objs[S_SUPER_SHIELD];

    if ( curplr_diff == DIFF_0 && amt > 1 ) {
        amt = amt >> 1;
    }

    if ( cur ) {
        ANIMS_StartAnim( A_SUPER_SHIELD, 0, 0 );

        SND_Patch( FX_SHIT, 127 );

        cur->num -= amt;

        if ( cur->num < 0 ) {
            OBJS_Del( S_SUPER_SHIELD );
        }
    } else {
        cur = p_objs[S_ENERGY];
        if ( !cur ) {
            return 0;
        }

        SND_Patch( FX_HIT, 127 );

        cur->num -= amt;

        if ( cur->num < 0 ) {
            cur->num = 0;
        }
    }

    return cur->num;
}

/***************************************************************************
OBJS_AddEnergy()
 ***************************************************************************/
int // RETURN: return nums in OBJ
OBJS_AddEnergy(
    int amt // INPUT : amount to add
) {
    OBJ* cur;

    cur = p_objs[S_ENERGY];
    if ( !cur ) {
        return 0;
    }

    if ( cur->num < cur->lib->max_cnt ) {
        cur = p_objs[S_ENERGY];

        if ( cur->num == 0 ) {
            return 0;
        }

        cur->num += amt;

        if ( cur->num > cur->lib->max_cnt ) {
            cur->num = cur->lib->max_cnt;
        }
    } else {
        cur = p_objs[S_SUPER_SHIELD];
        if ( !cur ) {
            return 0;
        }

        if ( cur->num == 0 ) {
            return 0;
        }

        cur->num += amt >> 2;

        if ( cur->num > cur->lib->max_cnt ) {
            cur->num = cur->lib->max_cnt;
        }
    }

    return cur->num;
}

/***************************************************************************
OBJS_LoseObj() - Lose random object
 ***************************************************************************/
bool OBJS_LoseObj( void ) {
    OBJ_LIB* lib;
    int type;
    bool rval = true;

    if ( plr.sweapon == EMPTY ) {
        for ( type = S_LAST_OBJECT - 1; type >= 0; type-- ) {
            lib = &obj_lib[type];
            if ( p_objs[type] && lib->loseit ) {
                OBJS_Del( type );
                rval = true;
                break;
            }
        }
    } else {
        OBJS_Del( plr.sweapon );
        rval = true;
    }

    return rval;
}

/***************************************************************************
OBJS_Think () - Does all in game thinking ( recharing )
 ***************************************************************************/
void OBJS_Think( void ) {
    if ( curplr_diff >= DIFF_3 ) {
        return;
    }

    if ( objuse_flag ) {
        objuse_flag = false;
        return;
    }

    think_cnt++;

    if ( think_cnt > CHARGE_SHIELD ) {
        if ( startendwave == EMPTY ) {
            OBJS_AddEnergy( 1 );
        }
        think_cnt = 0;
    }
}

/***************************************************************************
OBJS_MakeSpecial() - Makes the selected weapon the current special
 ***************************************************************************/
bool OBJS_MakeSpecial(
    OBJ_TYPE type // INPUT : OBJ type
) {
    OBJ* cur = p_objs[type];
    OBJ_LIB* lib = &obj_lib[type];

    if ( type >= S_LAST_OBJECT ||  cur == NULL || !lib->specialw ) {
        return false;
    }

    plr.sweapon = type;

    return true;
}
