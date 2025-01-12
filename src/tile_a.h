//
// Created by m on 2025-01-05.
//

#ifndef TILE_A_H
#define TILE_A_H

#define SCREENWIDTH  320
#define SCREENHEIGHT 200

#define TILEWIDTH    32
#define TILEHEIGHT   32

#define MAP_LEFT     16

void TILE_Draw( int max_y );
void TILE_DisplayScreen( int srcOffset, int dstOffset, int cols  );

#endif // TILE_A_H
