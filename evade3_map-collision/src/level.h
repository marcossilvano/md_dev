#ifndef _LEVEL_H_
#define _LEVEL_H_

#include <genesis.h>
#include "globals.h"
#include "gameobject.h"
#include "resources.h"

#define NUMBER_OF_LEVELS 5

extern Map* map;
extern u8 collision_map[SCREEN_METATILES_W][SCREEN_METATILES_H]; // screen collision map

// screen cell position in map
extern u16 screen_x;
extern u16 screen_y;

#define COLLISION_LEFT   0xb0001
#define COLLISION_RIGHT  0xb0010
#define COLLISION_HORIZ  0xb0011

#define COLLISION_TOP    0xb0100
#define COLLISION_BOTTOM 0xb1000
#define COLLISION_VERT   0xb1100

extern u8 collision_result;

////////////////////////////////////////////////////////////////////////////
// INITIALIZATION

u16 LEVEL_init(u16 ind);
void LEVEL_generate_screen_collision_map(u8 first_index, u8 last_index);

////////////////////////////////////////////////////////////////////////////
// GAME LOOP/LOGIC

inline u8 LEVEL_collision_result() {
	return collision_result;
}

u8 LEVEL_check_wall(GameObject* obj);
void LEVEL_move_and_slide(GameObject* obj);

inline u8 LEVEL_wall_at(s16 x, s16 y) {
	return collision_map[x/METATILE_W][y/METATILE_W];
}

////////////////////////////////////////////////////////////////////////////
// DRAWING AND FX

void LEVEL_draw_collision_map();

#endif // _LEVEL_H_