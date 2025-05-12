#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <genesis.h>
#include <sprite_eng.h>

extern char text[5];

#define SCREEN_W 320
#define SCREEN_H 224

#define SCREEN_W_F16 FIX16(320)
#define SCREEN_H_F16 FIX16(224)

#define SCREEN_TILES_W SCREEN_W/8
#define SCREEN_TILES_H SCREEN_H/8

// Map is made of 16x16 metatiles
#define METATILE_W 16
#define SCREEN_METATILES_W SCREEN_W/METATILE_W
#define SCREEN_METATILES_H SCREEN_H/METATILE_W

#define MAP_TILES_W 60
#define MAP_TILES_H 42

#define MAP_W MAP_TILES_W * TILE_W
#define MAP_H MAP_TILES_H * TILE_W

#define PAL_PLAYER 		PAL0
#define PAL_NPC 		PAL1
#define PAL_MAP 		PAL2
#define PAL_BACKGROUND 	PAL3

#define NUMBER_OF_JOYPADS 2

#endif