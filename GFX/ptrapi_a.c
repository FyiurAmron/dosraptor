#include <stdint.h>
#include <string.h>

#include "types.h"

#define SCREENWIDTH  320
#define SCREENHEIGHT 200

#define CURSORWIDTH  16
#define CURSORHEIGHT 16

extern uint8_t* displaybuffer;
extern uint8_t* displayscreen;
extern uint8_t* cursorstart;
extern uint8_t* displaypic;
extern uint8_t* cursorsave;

extern int32_t cursorloopx, cursorloopy;
extern int32_t joy_x, joy_y, joy_buttons;

void PTR_ReadJoyStick( void ) {
    __asm {
        pushad
        mov edx, 0x201
        xor edi, edi
        xor esi, esi
        xor ebx, ebx
        xor eax, eax
        mov ecx, 0x3FF

        out dx, al
        in al, dx
        in al, dx
        not al
        shr eax, 4
        mov joy_buttons, eax
        xor eax, eax

    LR_1:
        in al, dx
        and al, 0xF
        jz LR_2

        rcr al, 1
        adc edi, ebx
        rcr al, 1
        adc esi, ebx
        loop LR_1

    LR_2:
        mov joy_y, esi
        mov joy_x, edi

        popad
    }
}

void PTR_Copy( const uint8_t* src, uint8_t* dst, int max_y ) {
    for ( int y = 0; y < max_y; y++ ) {
        memcpy( dst, src, CURSORWIDTH );
        dst += CURSORWIDTH;
        src += SCREENWIDTH;
    }
}

void PTR_Save( int max_y ) {
    PTR_Copy( cursorstart, cursorsave, max_y );
}

void PTR_Erase( void ) {
    uint8_t* src = cursorsave;
    uint8_t* dst = cursorstart;
    for ( int y = 0; y < CURSORHEIGHT; y++ ) {
        memcpy( dst, src, CURSORWIDTH );
        dst += SCREENWIDTH;
        src += CURSORWIDTH;
    }
}

void PTR_ClipErase( void ) {
    uint8_t* src = cursorsave;
    uint8_t* dst = cursorstart;

    for ( int y = 0; y < cursorloopy; y++ ) {
        memcpy( dst, src, cursorloopx );
        dst += SCREENWIDTH;
        src += CURSORWIDTH;
    }
}

void PTR_Draw( void ) {
    uint8_t* src = displaypic;
    uint8_t* dst = cursorstart;

    for ( int y = 0; y < cursorloopy; y++ ) {
        for ( int x = 0; x < cursorloopx; x++ ) {
            uint8_t pixel = *src++;
            if ( pixel != 0 ) {
                *dst = pixel;
            }
            dst++;
        }
        src += CURSORWIDTH - cursorloopx;
        dst += SCREENWIDTH - cursorloopx;
    }
}
