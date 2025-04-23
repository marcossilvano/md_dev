#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <genesis.h>
#include <sprite_eng.h>

extern int hscroll_offset;
extern char text[5];

#define TILE_W 16
#define SCREEN_W 320
#define SCREEN_H 224

#define SCREEN_TILES_H SCREEN_H / 8

#define MAP_TILES_W 120
#define MAP_TILES_H 50

#define MAP_W MAP_TILES_W * TILE_W
#define MAP_H MAP_TILES_H * TILE_W

#define PAL_PLAYER 		PAL0
#define PAL_MAP 		PAL1
#define PAL_BACKGROUND 	PAL2
#define PAL_NPC 		PAL4

#define NUMBER_OF_JOYPADS 2

#endif