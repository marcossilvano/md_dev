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

////////////////////////////////////////////////////////////////////////////
// INITIALIZATION

u16 LEVEL_init(u16 ind);
void LEVEL_generate_screen_collision_map(u8 first_index, u8 last_index);

////////////////////////////////////////////////////////////////////////////
// GAME LOOP/LOGIC

static inline u8 LEVEL_wall_at(u16 x, u16 y);
u8 LEVEL_check_wall(GameObject* obj);
bool LEVEL_move_and_slide(GameObject* obj);

////////////////////////////////////////////////////////////////////////////
// DRAWING AND FX

void LEVEL_draw_collision_map();

#endif // _LEVEL_H_