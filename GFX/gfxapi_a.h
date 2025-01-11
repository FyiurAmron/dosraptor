#ifndef GFXAPI_A_H
#define GFXAPI_A_H

#include "types.h"

void GFX_Shade( uint8_t* dst, int maxlen, const uint8_t* shading_table );
void GFX_ShadeSprite( uint8_t* dst, const uint8_t* src, const uint8_t* shading_table );
void GFX_DrawSprite( uint8_t* dst, const uint8_t* src );
void GFX_DrawChar( uint8_t* dst, const uint8_t* src, int lx, int ly, int add_x_src, uint8_t color );
void GFX_PutPic( void );
void GFX_PutMaskPic( void );
void GFX_DisplayScreen( void );

#endif
