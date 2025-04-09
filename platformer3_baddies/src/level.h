#ifndef _LEVEL_H_
#define _LEVEL_H_

#include <genesis.h>
#include "structs.h"
#include "resources.h"

#define NUMBER_OF_LEVELS 5

typedef struct {
	u8 animation_delay;
	u8 current_frame;
	u8 number_of_frames;
	TileSet* animated_tiles;
	u16 dest_top_tileindex;
	u16 dest_bottom_tileindex;
} AnimatedTile;

extern Map* map;
extern Map* map_bg;
extern u8 collision_map[MAP_TILES_W][MAP_TILES_H]; // map tiles 120x50

extern const u8* const animated_tiles_map[5];

extern fix16 offset_mask[SCREEN_H/TILE_W]; // 224 px / 8 px = 28
extern fix16 offset_speed[SCREEN_H/TILE_W];

extern u8 tile_frame;

////////////////////////////////////////////////////////////////////////////
// INITIALIZATION

void LEVEL_generate_collision_map_preview();
void LEVEL_generate_collision_map(u16* ground_tiles, u16 n);
u16 LEVEL_init(u16 ind);

////////////////////////////////////////////////////////////////////////////
// GAME LOOP/LOGIC
inline u8 LEVEL_wall_at(u16 x, u16 y) {
	return collision_map[x/TILE_W][y/TILE_W];
}

u8 LEVEL_check_wall(GameObject* obj);

////////////////////////////////////////////////////////////////////////////
// DRAWING AND FX

void LEVEL_draw_collision_map();
void LEVEL_animate_tiles();

#endif // _LEVEL_H_