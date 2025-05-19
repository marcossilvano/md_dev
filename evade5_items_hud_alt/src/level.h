#ifndef _LEVEL_H_
#define _LEVEL_H_

#include <genesis.h>
#include "globals.h"
#include "gameobject.h"
#include "resources.h"

#define NUMBER_OF_LEVELS 5

#define IDX_EMPTY 	   10
#define IDX_ITEM  	   8
#define IDX_ITEM_DONE  80
#define IDX_WALL_FIRST 0
#define IDX_WALL_LAST  5

#define OFFSCREEN_TILES 3

#define COLLISION_LEFT   0b0001
#define COLLISION_RIGHT  0b0010
#define COLLISION_HORIZ  0b0011

#define COLLISION_TOP    0b0100
#define COLLISION_BOTTOM 0b1000
#define COLLISION_VERT   0b1100

extern Map* map;

extern u8 collision_map[SCREEN_METATILES_W + OFFSCREEN_TILES*2][SCREEN_METATILES_H + OFFSCREEN_TILES*2]; // screen collision map
// extern u8 collision_map[MAP_METATILES_W + OFFSCREEN_TILES*2][MAP_METATILES_H + OFFSCREEN_TILES*2]; // screen collision map

extern u8 collision_result;

// screen cell position in map
// extern u16 screen_x;
// extern u16 screen_y;

/*
  COLLECTED TILE ITEMS TABLE
  This table controls which item(s) was collected in each room (screen).
  - Each index is mapped to a room
  - In each position, each bit is mapped to a item in that room (order is from left to right, top to bottom)
  - IMPORTANT: this implementation only allows 64 items (64 bits) per room.
*/
// extern u32 items_table[NUMBER_OF_ROOMS][2];

////////////////////////////////////////////////////////////////////////////
// INITIALIZATION

u16 LEVEL_init(u16 ind);

////////////////////////////////////////////////////////////////////////////
// GAME LOOP/LOGIC

inline u8 LEVEL_collision_result() {
	return collision_result;
}

inline u8 LEVEL_wallXY(s16 x, s16 y) {
	return collision_map[x/METATILE_W + OFFSCREEN_TILES][y/METATILE_W + OFFSCREEN_TILES] == 1;
}

inline u8 LEVEL_tileXY(s16 x, s16 y) {
	return collision_map[x/METATILE_W + OFFSCREEN_TILES][y/METATILE_W + OFFSCREEN_TILES];
}

inline u8 LEVEL_tileIDX16(s16 metatile_x, s16 metatile_y) {
	return collision_map[metatile_x + OFFSCREEN_TILES][metatile_y + OFFSCREEN_TILES];
}

/**
 * Define valor para tile no mapa de colisão.
 * @param x Posição X em pixels.
 * @param y Posição Y em pixels.
 */
inline void LEVEL_set_tileXY(s16 x, s16 y, u8 value) {
	collision_map[x/METATILE_W + OFFSCREEN_TILES][y/METATILE_W + OFFSCREEN_TILES] = value;
}

/**
 * Define valor para tile no mapa de colisão.
 * @param x Posição X em metatiles 16x16.
 * @param y Posição Y em metatiles 16x16.
 */
inline void LEVEL_set_tileIDX16(s16 x, s16 y, u8 value) {
	collision_map[x + OFFSCREEN_TILES][y + OFFSCREEN_TILES] = value;
}

u8 LEVEL_check_wall(GameObject* obj);
void LEVEL_move_and_slide(GameObject* obj);

void LEVEL_remove_tileXY(s16 x, s16 y, u8 new_tile);
void LEVEL_update_camera(GameObject* obj);
void LEVEL_check_map_boundaries(GameObject* obj);


////////////////////////////////////////////////////////////////////////////
// DRAWING AND FX

void LEVEL_draw_collision_map();
void LEVEL_draw_tile_map();

// DEBUG: change for the map you want to draw
inline void LEVEL_draw_map() {
	LEVEL_draw_collision_map();
	// LEVEL_draw_tile_map();
}

#endif // _LEVEL_H_