

#include "public.h"

#include "inc/file0001.inc"
#include "inc/file0002.inc"
#include "inc/file0003.inc"
#include "inc/file0004.inc"

#include "anims.h"
#include "bonus.h"
#include "eshot.h"
#include "flame.h"
#include "shadows.h"
#include "tile.h"

#include "enemy.h"

SPRITE_SHIP ships[MAX_ONSCREEN];

#define NORM_SHOOT  ( -1 )
#define START_SHOOT 0

ESHOT_TYPE ashot[2] = { ES_ANGLELEFT, ES_ANGLERIGHT };

int tiley = 0;

int numships;
int numboss;
CSPRITE* csprite;
PRIVATE int numslib[4];
PRIVATE SPRITE* slib[4];
PRIVATE bool spriteflag[4];
PRIVATE SPRITE_SHIP* onscreen[MAX_ONSCREEN];
PRIVATE SPRITE_SHIP* rscreen[MAX_ONSCREEN];
PRIVATE int cur_visable;
PRIVATE bool boss_sound;

DWORD cur_diff = EB_EASY_LEVEL;
SPRITE_SHIP first_enemy;
SPRITE_SHIP last_enemy;
PRIVATE SPRITE_SHIP* free_enemy;
PRIVATE CSPRITE* end_enemy = NULL;
PRIVATE CSPRITE* cur_enemy = NULL;
bool end_waveflag = false;
PRIVATE DWORD spriteitm[4] = { SPRITE1_ITM, SPRITE2_ITM, SPRITE3_ITM, SPRITE4_ITM };
int g_numslibs;

/***************************************************************************
   MoveEobj() - gets next postion for an Object at speed
 ***************************************************************************/
int MoveEobj(
    MOVEOBJ* cur, // INPUT : pointer to MOVEOBJ
    int speed // INPUT : speed to plot at
) {
    if ( speed == 0 ) {
        return 0;
    }

    if ( cur->delx >= cur->dely ) {
        while ( speed ) {
            speed--;
            cur->maxloop--;

            if ( cur->maxloop == 0 ) {
                cur->done = true;
                return speed;
            }

            cur->x += cur->addx;
            cur->err += cur->dely;

            if ( cur->err > 0 ) {
                cur->y += cur->addy;
                cur->err -= cur->delx;
            }
        }
    } else {
        while ( speed ) {
            speed--;
            cur->maxloop--;

            if ( cur->maxloop == 0 ) {
                cur->done = true;
                return speed;
            }

            cur->y += cur->addy;
            cur->err += cur->delx;

            if ( cur->err > 0 ) {
                cur->x += cur->addx;
                cur->err -= cur->dely;
            }
        }
    }

    if ( cur->maxloop < 1 ) {
        cur->done = true;
    }

    return speed;
}

/***************************************************************************
ENEMY_FreeSprites () - Free Memory Used by Sprites use in last level
 ***************************************************************************/
void ENEMY_FreeSprites( void ) {
    int j, i;
    SPRITE* curlib;
    CSPRITE* curfld;

    for ( j = 0; j < 4; j++ ) {
        if ( spriteflag[j] ) {
            GLB_FreeItem( spriteitm[j] );
        }
    }

    for ( j = 0; j < ml->numsprites; j++ ) {
        curfld = &csprite[j];
        curlib = &slib[csprite[j].game][csprite[j].slib];

        if ( curfld->level & cur_diff ) {
            for ( i = 0; i < curlib->num_frames; i++ ) {
                GLB_FreeItem( curlib->item + (DWORD) i );
            }
        }
    }
}

/***************************************************************************
ENEMY_LoadSprites() -
 ***************************************************************************/
void ENEMY_LoadSprites( void ) {
    int j, i;
    SPRITE* curlib;
    CSPRITE* curfld;
    DWORD item;

    ENEMY_Clear();
    cur_visable = 0;
    boss_sound = false;

    for ( j = 0; j < ml->numsprites; j++ ) {
        curfld = &csprite[j];
        curlib = &slib[csprite[j].game][curfld->slib];
        curlib->item = GLB_GetItemID( curlib->iname );

        switch ( curfld->level ) {
            default:
                curfld->level = EB_NOT_USED;
                break;

            case E_SECRET_1:
                curfld->level = EB_SECRET_1;
                break;

            case E_SECRET_2:
                curfld->level = EB_SECRET_2;
                break;

            case E_SECRET_3:
                curfld->level = EB_SECRET_3;
                break;

            case E_EASY_LEVEL:
                curfld->level = EB_EASY_LEVEL;
                break;

            case E_MED_LEVEL:
                curfld->level = EB_MED_LEVEL;
                break;

            case E_HARD_LEVEL:
                curfld->level = EB_HARD_LEVEL;
                break;
        }

        if ( curfld->level & cur_diff ) {
            if ( curlib->item != EMPTY ) {
                for ( i = 0; i < curlib->num_frames; i++ ) {
                    item = curlib->item + (DWORD) i;

                    GLB_CacheItem( item );
                }
            } else {
                curfld->level = EB_NOT_USED;
            }
        } else {
            curfld->level = EB_NOT_USED;
        }
    }
}

/***************************************************************************
ENEMY_LoadLib () - Loads and Locks spritelib's MUST becalled b4 LoadSprites
 ***************************************************************************/
void ENEMY_LoadLib( void ) {
    int i;

    memset( spriteflag, 0, sizeof( spriteflag ) );

    for ( i = 0; i < ml->numsprites; i++ ) {
        spriteflag[csprite[i].game] = true;
    }

    g_numslibs = 0;
    for ( i = 0; i < 4; i++ ) {
        slib[i] = NULL;
        numslib[i] = 0;

        if ( spriteflag[i] ) {
            g_numslibs++;
        }
    }

    if ( g_numslibs > 1 ) {
        if ( !spriteflag[2] && !spriteflag[3] ) {
            EXIT_Error(
                "ENEMY_LoadSprites() - F:%d  G1:%d G2:%d G3:%d G4:%d", g_numslibs, spriteflag[0], spriteflag[1],
                spriteflag[2], spriteflag[3] );
        }
    }

    for ( i = 0; i < 4; i++ ) {
        if ( spriteflag[i] ) {
            slib[i] = (SPRITE*) GLB_LockItem( spriteitm[i] );

            if ( slib[i] == NULL ) {
                EXIT_Error( "ENEMY_LoadSprites() - memory" );
            }

            numslib[i] = GLB_ItemSize( spriteitm[i] );
            numslib[i] = numslib[i] / sizeof( SPRITE );
        }
    }
}

/***************************************************************************
ENEMY_Clear()
 ***************************************************************************/
void ENEMY_Clear( void ) {
    int i;

    numboss = 0;
    numships = 0;
    end_waveflag = false;

    first_enemy.prev = NULL;
    first_enemy.next = &last_enemy;

    last_enemy.prev = &first_enemy;
    last_enemy.next = NULL;

    free_enemy = ships;

    memset( ships, 0, sizeof( ships ) );

    for ( i = 0; i < MAX_ONSCREEN - 1; i++ ) {
        ships[i].next = &ships[i + 1];
    }

    if ( ml->numsprites ) {
        end_enemy = csprite + ( ml->numsprites - 1 );
        cur_enemy = csprite;
    } else {
        end_enemy = NULL;
        cur_enemy = NULL;
    }
}

/*-------------------------------------------------------------------------*
ENEMY_Get() - Gets An Free Enemy from link list
 *-------------------------------------------------------------------------*/
PRIVATE SPRITE_SHIP* ENEMY_Get( void ) {
    SPRITE_SHIP* sh;

    if ( !free_enemy ) {
        EXIT_Error( "ENEMY_Get() - Max Sprites" );
    }

    numships++;

    sh = free_enemy;
    free_enemy = free_enemy->next;

    memset( sh, 0, sizeof( SPRITE_SHIP ) );

    sh->next = &last_enemy;
    sh->prev = last_enemy.prev;
    last_enemy.prev = sh;
    sh->prev->next = sh;

    return sh;
}

/*-------------------------------------------------------------------------*
ENEMY_Remove () - Removes an Enemy OBJECT from linklist
 *-------------------------------------------------------------------------*/
PRIVATE SPRITE_SHIP* ENEMY_Remove( SPRITE_SHIP* sh ) {
    SPRITE_SHIP* next;

    if ( sh->lib->bossflag ) {
        numboss--;
    }

    numships--;

    if ( end_waveflag && numships < 1 ) {
        startendwave = END_DURATION;
    }

    next = sh->prev;

    sh->next->prev = sh->prev;
    sh->prev->next = sh->next;

    memset( sh, 0, sizeof( SPRITE_SHIP ) );
    sh->item = ~0;

    sh->next = free_enemy;

    free_enemy = sh;

    return next;
}

/*-------------------------------------------------------------------------*
ENEMY_Add () - Adds Enemy to attack player
 *-------------------------------------------------------------------------*/
PRIVATE void ENEMY_Add( CSPRITE* sprite ) {
    SPRITE* curlib = slib[sprite->game] + sprite->slib;
    SPRITE_SHIP* pNew;
    GFX_PIC* h;
    BYTE* pic;

    pNew = ENEMY_Get();
    pic = GLB_GetItem( curlib->item );
    h = (GFX_PIC*) pic;

    pNew->item = curlib->item;
    pNew->width = h->width;
    pNew->height = h->height;
    pNew->hlx = h->width >> 1;
    pNew->hly = h->height >> 1;

    pNew->kami = KAMI_FLY;
    pNew->hits = curlib->hits;
    pNew->lib = slib[sprite->game] + sprite->slib;
    pNew->y = tileyoff - ( tiley - sprite->y ) * 32 - 97;
    pNew->x = sprite->x * 32 + MAP_LEFT;

    pNew->edir = E_FORWARD;
    pNew->x += 16;
    pNew->y += 16;
    pNew->x -= pNew->hlx;
    pNew->y -= pNew->hly;
    pNew->x2 = pNew->x + pNew->width;
    pNew->y2 = pNew->y + pNew->height;
    pNew->move.x = pNew->sx = pNew->x;
    pNew->move.y = pNew->sy = pNew->y;
    pNew->frame_rate = curlib->frame_rate;
    pNew->speed = curlib->movespeed;

    pNew->countdown = curlib->countdown + -pNew->move.y;
    pNew->shoot_disable = false;
    pNew->shoot_on = false;
    pNew->shootagain = NORM_SHOOT;
    pNew->shootcount = curlib->shootcnt;
    pNew->shootflag = curlib->shootstart;

    if ( curlib->bossflag ) {
        if ( curplr_diff <= DIFF_1 ) {
            pNew->hits = pNew->hits - ( pNew->hits >> 1 );
            pNew->shootcount = pNew->shootcount - ( pNew->shootcount >> 2 );
        }
    }

    switch ( curlib->animtype ) {
        default:
            EXIT_Error( "ENEMY_Add() - Invalid ANIMTYPE" );
        case GANIM_NORM:
            pNew->anim_on = true;
            pNew->num_frames = curlib->num_frames;
            break;

        case GANIM_SHOOT:
            pNew->anim_on = false;
            pNew->num_frames = curlib->num_frames;
            break;

        case GANIM_MULTI:
            pNew->anim_on = true;
            pNew->num_frames = curlib->rewind;
            break;
    }

    switch ( curlib->flighttype ) {
        case F_REPEAT:
        case F_LINEAR:
        case F_KAMI:
            pNew->groundflag = false;
            pNew->sy = 100 - pNew->hly;
            pNew->move.x2 = pNew->sx + curlib->flightx[0];
            pNew->move.y2 = pNew->sy + curlib->flighty[0];
            pNew->movepos = 1;
            InitMobj( &pNew->move );
            MoveMobj( &pNew->move );
            break;

        case F_GROUND:
            pNew->groundflag = true;
            pNew->move.x2 = pNew->x;
            pNew->move.y2 = 211;
            break;

        case F_GROUNDRIGHT:
            pNew->x -= pNew->width;
            pNew->move.x = pNew->sx = pNew->x;
            pNew->groundflag = true;
            pNew->move.x2 = 335;
            pNew->move.y2 = 211;
            break;

        case F_GROUNDLEFT:
            pNew->x += pNew->width;
            pNew->move.x = pNew->sx = pNew->x;
            pNew->groundflag = true;
            pNew->move.x2 = -pNew->hlx;
            pNew->move.y2 = 211;
            break;
    }

    pNew->suckagain = curlib->hits >> 4;

    if ( curlib->song != -1 ) {
        boss_sound = true;
    }
}

/***************************************************************************
ENEMY_GetRandom () - Returns a random ship thats visable
 ***************************************************************************/
SPRITE_SHIP* ENEMY_GetRandom( void ) {
    int pos;

    if ( !cur_visable ) {
        return NULL;
    }

    pos = random( cur_visable );

    return onscreen[pos];
}

/***************************************************************************
ENEMY_GetRandomAir () - Returns a random ship thats visable
 ***************************************************************************/
SPRITE_SHIP* ENEMY_GetRandomAir( void ) {
    int pos;
    int i;

    if ( !cur_visable ) {
        return NULL;
    }

    pos = 0;
    for ( i = 0; i < cur_visable; i++ ) {
        if ( onscreen[i]->groundflag ) {
            continue;
        }
        rscreen[pos] = onscreen[i];
        pos++;
    }

    if ( pos > 0 ) {
        pos = random( pos );
        return rscreen[pos];
    }

    return NULL;
}

/***************************************************************************
ENEMY_DamageAll () - Tests to see if hit occured at x/y and applys damage
 ***************************************************************************/
bool ENEMY_DamageAll(
    int x, // INPUT : x position
    int y, // INPUT : y position
    int damage // INPUT : damage
) {
    int i;
    SPRITE_SHIP* cur;

    for ( i = 0; i < cur_visable; i++ ) {
        cur = onscreen[i];

        if ( x > cur->x && x < cur->x2 && y > cur->y && y < cur->y2 ) {
            cur->hits -= damage;
            return true;
        }
    }

    return false;
}

/***************************************************************************
ENEMY_DamageGround () - Tests to see if hit occured at x/y and applys damage
 ***************************************************************************/
bool ENEMY_DamageGround(
    int x, // INPUT : x position
    int y, // INPUT : y position
    int damage // INPUT : damage
) {
    int i;
    SPRITE_SHIP* cur;

    for ( i = 0; i < cur_visable; i++ ) {
        cur = onscreen[i];

        if ( !cur->groundflag ) {
            continue;
        }

        if ( x > cur->x && x < cur->x2 && y > cur->y && y < cur->y2 ) {
            cur->hits -= damage;
            if ( curplr_diff == DIFF_0 ) {
                cur->hits -= damage;
            }
            return true;
        }
    }

    return false;
}

/***************************************************************************
ENEMY_DamageAir () - Tests to see if hit occured at x/y and applys damage
 ***************************************************************************/
bool ENEMY_DamageAir(
    int x, // INPUT : x position
    int y, // INPUT : y position
    int damage // INPUT : damage
) {
    int i;
    SPRITE_SHIP* cur;

    for ( i = 0; i < cur_visable; i++ ) {
        cur = onscreen[i];

        if ( cur->groundflag ) {
            continue;
        }

        if ( x > cur->x && x < cur->x2 && y > cur->y && y < cur->y2 ) {
            cur->hits -= damage;
            if ( curplr_diff == DIFF_0 ) {
                cur->hits -= damage;
            }
            return true;
        }
    }

    return false;
}
/***************************************************************************
ENEMY_DamageEnergy () - Tests to see if hit occured at x/y and applys damage
 ***************************************************************************/
SPRITE_SHIP* ENEMY_DamageEnergy(
    int x, // INPUT : x position
    int y, // INPUT : y position
    int damage // INPUT : damage
) {
    int i;
    SPRITE_SHIP* cur;

    for ( i = 0; i < cur_visable; i++ ) {
        cur = onscreen[i];

        if ( cur->groundflag ) {
            continue;
        }

        if ( x > cur->x && x < cur->x2 && y > cur->y && y < cur->y2 ) {
            cur->hits--;

            if ( cur->lib->suck ) {
                if ( cur->suckagain > 0 ) {
                    cur->suckagain -= damage;
                } else {
                    cur->shoot_on = false;
                    cur->shoot_disable = true;
                    cur->shootagain = NORM_SHOOT;
                    SND_3DPatch( FX_EGRAB, cur->x + cur->hlx, cur->x + cur->hlx );
                }
            }
            return cur;
        }
    }

    return NULL;
}

/***************************************************************************
ENEMY_Think() - Does all thinking for enemy ships ( ground/air )
 ***************************************************************************/
void ENEMY_Think( void ) {
    SPRITE_SHIP* sprite;
    SPRITE* curlib;
    CSPRITE* old_enemy;
    int speed;
    int i;
    int x;
    int y;
    int area;
    int suben;

    if ( boss_sound ) {
        if ( !SND_IsPatchPlaying( FX_BOSS1 ) ) {
            SND_Patch( FX_BOSS1, 127 );
        }
    }

    tiley = tilepos / MAP_COLS - 3;

    if ( !end_waveflag ) {
        while ( cur_enemy->y == tiley ) {
            for ( ;; ) {
                old_enemy = cur_enemy;

                if ( cur_enemy->level != EB_NOT_USED ) {
                    ENEMY_Add( cur_enemy );
                }

                if ( cur_enemy == end_enemy ) {
                    end_waveflag = true;
                    break;
                }

                cur_enemy++;

                if ( old_enemy->link == EMPTY ) {
                    break;
                }
                if ( old_enemy->link == 1 ) {
                    break;
                }
            }

            if ( end_waveflag ) {
                break;
            }
        }
    }

    cur_visable = 0;

    for ( sprite = first_enemy.next; sprite != &last_enemy; sprite = sprite->next ) {
        curlib = sprite->lib;

        if ( curlib->num_frames > 1 ) {
            sprite->item = curlib->item + (DWORD) sprite->curframe;

            if ( sprite->frame_rate < 1 ) {
                sprite->frame_rate = curlib->frame_rate;

                if ( sprite->anim_on ) {
                    sprite->curframe++;
                    if ( sprite->curframe >= sprite->num_frames ) {
                        sprite->curframe -= curlib->rewind;
                        switch ( curlib->animtype ) {
                            default:
                                EXIT_Error( "ENEMY_Think() - Invalid ANIMTYPE1" );
                            case GANIM_NORM:
                                break;

                            case GANIM_SHOOT:
                                sprite->anim_on = false;
                                sprite->shoot_on = true;
                                break;

                            case GANIM_MULTI:
                                switch ( sprite->multi ) {
                                    case MULTI_START:
                                        sprite->num_frames = curlib->num_frames;
                                        sprite->multi = MULTI_END;
                                        break;

                                    case MULTI_END:
                                        sprite->shoot_on = true;
                                        break;

                                    default:
                                        break;
                                }
                                break;
                        }
                    }
                }
            } else {
                sprite->frame_rate--;
            }

            if ( sprite->countdown < 1 ) {
                switch ( curlib->animtype ) {
                    default:
                        EXIT_Error( "ENEMY_Think() - Invalid ANIMTYPE2" );
                    case GANIM_NORM:
                        sprite->shoot_on = true;
                        break;

                    case GANIM_SHOOT:
                        sprite->anim_on = true;
                        break;

                    case GANIM_MULTI:
                        if ( sprite->multi == MULTI_OFF ) {
                            sprite->multi = MULTI_START;
                        }
                        break;
                }
            } else {
                sprite->countdown -= curlib->movespeed;
            }
        } else {
            if ( sprite->countdown < 1 ) {
                sprite->countdown = -1;
                sprite->shoot_on = true;
            } else {
                sprite->countdown -= curlib->movespeed;
            }
        }

        switch ( curlib->flighttype ) {
            case F_REPEAT:
                sprite->x = sprite->move.x;
                sprite->y = sprite->move.y;
                sprite->x2 = sprite->x + sprite->width - 1;
                sprite->y2 = sprite->y + sprite->height - 1;

                speed = curlib->movespeed;

                speed = MoveEobj( &sprite->move, speed );

                if ( sprite->move.done ) {
                    sprite->move.x = sprite->move.x2;
                    sprite->move.y = sprite->move.y2;
                    sprite->move.x2 = sprite->sx + curlib->flightx[sprite->movepos];

                    sprite->move.y2 = sprite->sy + curlib->flighty[sprite->movepos];

                    InitMobj( &sprite->move );
                    MoveMobj( &sprite->move );

                    speed = MoveEobj( &sprite->move, speed );

                    if ( sprite->edir == E_FORWARD ) {
                        sprite->movepos++;
                        if ( sprite->movepos >= curlib->numflight ) {
                            sprite->edir = E_BACKWARD;
                            sprite->movepos = curlib->numflight - 1;
                        }
                    } else {
                        sprite->movepos--;
                        if ( sprite->movepos <= curlib->repos ) {
                            sprite->edir = E_FORWARD;
                            sprite->movepos = curlib->repos;
                        }
                    }
                }
                break;

            case F_KAMI:
                sprite->x = sprite->move.x;
                sprite->y = sprite->move.y;
                sprite->x2 = sprite->x + sprite->width - 1;
                sprite->y2 = sprite->y + sprite->height - 1;

                speed = curlib->movespeed;
                speed = MoveEobj( &sprite->move, speed );

                if ( sprite->kami == KAMI_END ) {
                    if ( sprite->move.y > 201 //
                         || ( sprite->move.x > 320 + sprite->hlx ) //
                         || ( sprite->move.y + sprite->height < 0 ) //
                         || ( sprite->move.x + sprite->width < 0 ) //
                    ) {
                        sprite->doneflag = true;
                    }
                }

                if ( sprite->move.done && sprite->kami != KAMI_END ) {
                    sprite->move.x = sprite->move.x2;
                    sprite->move.y = sprite->move.y2;
                    sprite->x2 = sprite->x + sprite->width - 1;
                    sprite->y2 = sprite->y + sprite->height - 1;

                    if ( sprite->kami == KAMI_CHASE ) {
                        sprite->move.x2 = player_cx;
                        sprite->move.y2 = player_cy;
                        sprite->kami = KAMI_END;
                    } else {
                        sprite->move.x2 = sprite->sx + curlib->flightx[sprite->movepos];
                        sprite->move.y2 = sprite->sy + curlib->flighty[sprite->movepos];
                    }

                    InitMobj( &sprite->move );
                    MoveMobj( &sprite->move );
                    speed = MoveEobj( &sprite->move, speed );

                    if ( sprite->movepos < curlib->numflight - 1 ) {
                        sprite->movepos++;
                    } else if ( sprite->kami == KAMI_FLY ) {
                        sprite->kami = KAMI_CHASE;
                    }
                }
                break;
            case F_LINEAR:
                sprite->x = sprite->move.x;
                sprite->y = sprite->move.y;
                sprite->x2 = sprite->x + sprite->width - 1;
                sprite->y2 = sprite->y + sprite->height - 1;

                speed = curlib->movespeed;

                speed = MoveEobj( &sprite->move, speed );

                if ( sprite->move.done ) {
                    sprite->move.x = sprite->move.x2;
                    sprite->move.y = sprite->move.y2;
                    sprite->move.x2 = sprite->sx + curlib->flightx[sprite->movepos];
                    sprite->move.y2 = sprite->sy + curlib->flighty[sprite->movepos];

                    InitMobj( &sprite->move );
                    MoveMobj( &sprite->move );

                    speed = MoveEobj( &sprite->move, speed );

                    sprite->movepos++;

                    if ( sprite->movepos > curlib->numflight ) {
                        sprite->doneflag = true;
                    }
                }
                break;

            case F_GROUND:
                if ( scroll_flag ) {
                    sprite->y++;
                }
                if ( sprite->y > sprite->move.y2 ) {
                    sprite->doneflag = true;
                }
                sprite->x2 = sprite->x + sprite->width - 1;
                sprite->y2 = sprite->y + sprite->height - 1;
                break;

            case F_GROUNDRIGHT:
                if ( scroll_flag ) {
                    sprite->y++;
                }
                if ( sprite->y >= 0 ) {
                    sprite->x += curlib->movespeed;
                    if ( sprite->x > sprite->move.x2 ) {
                        sprite->doneflag = true;
                    } else if ( sprite->y > sprite->move.y2 ) {
                        sprite->doneflag = true;
                    }
                }
                sprite->x2 = sprite->x + sprite->width - 1;
                sprite->y2 = sprite->y + sprite->height - 1;
                break;

            case F_GROUNDLEFT:
                if ( scroll_flag ) {
                    sprite->y++;
                }
                if ( sprite->y >= 0 ) {
                    sprite->x -= curlib->movespeed;
                    if ( sprite->x < sprite->move.x2 ) {
                        sprite->doneflag = true;
                    } else if ( sprite->y > sprite->move.y2 ) {
                        sprite->doneflag = true;
                    }
                }
                sprite->x2 = sprite->x + sprite->width - 1;
                sprite->y2 = sprite->y + sprite->height - 1;
                break;
        }

        if ( sprite->shoot_on ) {
            switch ( sprite->shootagain ) {
                case NORM_SHOOT:
                    sprite->shootflag--;

                    if ( sprite->shootflag >= 0 ) {
                        break;
                    }

                    sprite->shootflag = curlib->shotspace;

                    if ( !sprite->shoot_disable ) {
                        for ( i = 0; i < curlib->numguns; i++ ) {
                            ESHOT_Shoot( sprite, i );
                        }
                    }

                    sprite->shootcount--;
                    if ( sprite->shootcount < 1 ) {
                        sprite->shootagain = curlib->shootframe;
                    }
                    break;

                case START_SHOOT:
                    sprite->shootagain = NORM_SHOOT;
                    sprite->shootcount = curlib->shootcnt;
                    sprite->shootflag = curlib->shotspace;
                    break;

                default:
                    sprite->shootagain--;
                    break;
            }
        }

        if ( sprite->doneflag ) {
            sprite = ENEMY_Remove( sprite );
            continue;
        }

        if ( curlib->shadow ) {
            if ( sprite->groundflag ) {
                SHADOW_GAdd( sprite->item, sprite->x, sprite->y );
            } else {
                SHADOW_Add( sprite->item, sprite->x, sprite->y );
            }
        }

        if ( curlib->bossflag ) {
            numboss++;
            if ( sprite->hits < 50 && gl_cnt & 2 ) {
                x = sprite->x + random( sprite->width );
                y = sprite->y + random( sprite->height );
                ANIMS_StartAnim( A_SMALL_AIR_EXPLO, x, y );
            }
        }

        if ( !sprite->groundflag ) {
            if ( player_cx > sprite->x && player_cx < sprite->x2 ) {
                if ( player_cy > sprite->y && player_cy < sprite->y2 ) {
                    sprite->hits -= PLAYERWIDTH / 2;
                    if ( sprite->width > sprite->height ) {
                        suben = sprite->width;
                    } else {
                        suben = sprite->height;
                    }

                    OBJS_SubEnergy( suben >> 2 );
                    x = player_cx + ( random( 8 ) - 4 );
                    y = player_cy + ( random( 8 ) - 4 );
                    ANIMS_StartAnim( A_SMALL_AIR_EXPLO, x, y );
                    SND_Patch( FX_CRASH, 127 );
                }
            }
        }

        if ( sprite->hits <= 0 ) {
            plr.score += curlib->money;

            SND_3DPatch( FX_AIREXPLO, sprite->x + sprite->hlx, sprite->x + sprite->hlx );

            switch ( curlib->exptype ) {
                case EXP_ENERGY:
                    ANIMS_StartAnim( A_ENERGY_AIR_EXPLO, sprite->x + sprite->hlx, sprite->y + sprite->hly );
                    BONUS_Add( S_ITEMBUY6, sprite->x, sprite->y );
                    break;

                case EXP_AIRSMALL1:
                    ANIMS_StartAnim( A_MED_AIR_EXPLO, sprite->x + sprite->hlx, sprite->y + sprite->hly );
                    break;

                case EXP_AIRSMALL2:
                    ANIMS_StartAnim( A_MED_AIR_EXPLO2, sprite->x + sprite->hlx, sprite->y + sprite->hly );
                    break;

                case EXP_AIRMED:
                    ANIMS_StartAnim( A_LARGE_AIR_EXPLO, sprite->x + sprite->hlx, sprite->y + sprite->hly );
                    break;

                case EXP_AIRLARGE:
                    ANIMS_StartAnim( A_LARGE_AIR_EXPLO, sprite->x + sprite->hlx, sprite->y + sprite->hly );
                    area = ( sprite->width >> 4 ) * ( sprite->height >> 4 );
                    for ( i = 0; i < area; i++ ) {
                        x = sprite->x + random( sprite->width );
                        y = sprite->y + random( sprite->height );
                        if ( i & 1 ) {
                            ANIMS_StartAnim( A_MED_AIR_EXPLO, x, y );
                        } else {
                            ANIMS_StartAAnim( A_MED_AIR_EXPLO2, x, y );
                        }
                    }
                    break;

                case EXP_GRDSMALL:
                    ANIMS_StartAnim( A_SMALL_GROUND_EXPLO, sprite->x + sprite->hlx, sprite->y + sprite->hly );
                    break;

                case EXP_GRDMED:
                    x = sprite->x + random( sprite->width );
                    y = sprite->y + random( sprite->height );
                    if ( random( 2 ) == 0 ) {
                        ANIMS_StartAnim( A_GROUND_SPARKLE, x, y );
                    } else {
                        ANIMS_StartAnim( A_GROUND_FLARE, x, y );
                    }
                    ANIMS_StartAnim( A_LARGE_GROUND_EXPLO1, sprite->x + sprite->hlx, sprite->y + sprite->hly );
                    break;

                case EXP_GRDLARGE:
                    ANIMS_StartAnim( A_LARGE_GROUND_EXPLO1, sprite->x + sprite->hlx, sprite->y + sprite->hly );
                    area = ( sprite->width >> 4 ) * ( sprite->height >> 4 );
                    for ( i = 0; i < area; i++ ) {
                        x = sprite->x + random( sprite->width );
                        y = sprite->y + random( sprite->height );

                        if ( random( 2 ) == 0 ) {
                            ANIMS_StartAnim( A_GROUND_FLARE, x, y );
                        } else {
                            ANIMS_StartAnim( A_GROUND_SPARKLE, x, y );
                        }

                        ANIMS_StartAnim( A_SMALL_GROUND_EXPLO, x, y );
                    }
                    break;

                case EXP_BOSS:
                    ANIMS_StartAnim( A_LARGE_AIR_EXPLO, sprite->x + sprite->hlx, sprite->y + sprite->hly );
                    area = ( sprite->width >> 4 ) * ( sprite->height >> 4 );
                    for ( i = 0; i < area; i++ ) {
                        x = sprite->x + random( sprite->width );
                        y = sprite->y + random( sprite->height );
                        ANIMS_StartAAnim( A_GROUND_FLARE, x, y );
                        if ( i & 1 ) {
                            ANIMS_StartAnim( A_LARGE_AIR_EXPLO, x, y );
                        } else {
                            ANIMS_StartAnim( A_MED_AIR_EXPLO2, x, y );
                        }
                    }
                    break;

                case EXP_PERSON:
                    ANIMS_StartAnim( A_PERSON, sprite->x + sprite->hlx, sprite->y + sprite->hly );
                    break;

                case EXP_PLATOON:
                    ANIMS_StartAnim( A_PLATOON, sprite->x + sprite->hlx, sprite->y + sprite->hly );
                    break;
            }

            if ( curlib->bonus != EMPTY ) {
                BONUS_Add( curlib->bonus, sprite->x, sprite->y );
            }

            sprite = ENEMY_Remove( sprite );

            continue;
        }

        y = sprite->y + sprite->height;

        if ( y > 0 && sprite->y < 200 ) {
            x = sprite->x + sprite->width;

            if ( x > 0 && sprite->x < 320 ) {
                onscreen[cur_visable] = sprite;
                cur_visable++;
            }
        }
    }
}

/***************************************************************************
ENEMY_DisplayGround () - Displays Ground ENEMY pics
 ***************************************************************************/
void ENEMY_DisplayGround( void ) {
    SPRITE_SHIP* spt;

    for ( spt = first_enemy.next; spt != &last_enemy; spt = spt->next ) {
        if ( !spt->groundflag ) {
            continue;
        }
        GFX_PutSprite( GLB_GetItem( spt->item ), spt->x, spt->y );
    }
}

/***************************************************************************
ENEMY_DisplaySky () - Displays AIR ENEMY SHIPS
 ***************************************************************************/
void ENEMY_DisplaySky( void ) {
    SPRITE_SHIP* spt;
    int i;

    for ( spt = first_enemy.next; spt != &last_enemy; spt = spt->next ) {
        if ( spt->groundflag ) {
            continue;
        }

        GFX_PutSprite( GLB_GetItem( spt->item ), spt->x, spt->y );

        for ( i = 0; i < spt->lib->numengs; i++ ) {
            FLAME_Up( spt->x + spt->lib->engx[i], spt->y + spt->lib->engy[i], spt->lib->englx[i], spt->eframe );
        }

        spt->eframe ^= 1;
    }
}

/***************************************************************************
ENEMY_GetBaseDamage() - Gets Base Ship damage
 ***************************************************************************/
int ENEMY_GetBaseDamage( void ) {
    PRIVATE int nums = 0;
    int total = 0;
    SPRITE_SHIP* spt;
    int damage;

    //   if ( scroll_flag )
    nums = 0;

    for ( spt = first_enemy.next; spt != &last_enemy; spt = spt->next ) {
        if ( !spt->lib->bossflag ) {
            continue;
        }
        if ( spt->y + spt->hly < 0 ) {
            continue;
        }

        damage = spt->hits * 100 / spt->lib->hits;
        total += damage;

        //      if ( scroll_flag )
        nums++;
    }

    if ( nums ) {
        total /= nums;
    }

    return total;
}
