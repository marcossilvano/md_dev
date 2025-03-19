#ifndef _LEVEL_H_
#define _LEVEL_H_

#include <genesis.h>
#include "structs.h"
#include "resources.h"

extern Map* map;
extern u8 collision_map[MAP_TILES_W][MAP_TILES_H]; // map tiles 120x50

extern fix16 offset_mask[SCREEN_H/TILE_W]; // 224 px / 8 px = 28
extern fix16 offset_speed[SCREEN_H/TILE_W];

////////////////////////////////////////////////////////////////////////////
// INITIALIZATION

void LEVEL_generate_collision_map_preview();
void LEVEL_generate_collision_map(u16* ground_tiles, u16 n);
void LEVEL_init(u16* ind);

////////////////////////////////////////////////////////////////////////////
// GAME LOOP/LOGIC
u8 LEVEL_wall_at(u16 x, u16 y);
u8 LEVEL_check_wall(GameObject* obj);

////////////////////////////////////////////////////////////////////////////
// DRAWING AND FX

void LEVEL_draw_collision_map();

#endif // _LEVEL_H_