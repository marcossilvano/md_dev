#include <genesis.h>
#include "level.h"

Map* map;
u8 collision_map[SCREEN_W/METATILE_W][SCREEN_H/METATILE_W]; // screen collision map

// Top-Left screen position in map
u16 screen_x = 0;
u16 screen_y = 0;

////////////////////////////////////////////////////////////////////////////
// INIT

u16 LEVEL_init(u16 ind) {
	PAL_setPalette(PAL_MAP, level1_pal.data, DMA);
	VDP_loadTileSet(&level1_tiles, ind, DMA);
	map = MAP_create(&level1_map, BG_A, TILE_ATTR_FULL(PAL_MAP, FALSE, FALSE, FALSE, ind));
	
	MAP_scrollToEx(map, 0, 0, TRUE);
	// VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);
	
	ind += level1_tiles.numTile;
	//LEVEL_generate_collision_map((u16[]){0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15}, 16);
	
	LEVEL_generate_screen_collision_map(0, 5);

	return ind;
}

// Para otimizar os tiles de parede devem estar de forma contigua, na mesma "linha" do tilset
void LEVEL_generate_screen_collision_map(u8 first_index, u8 last_index) {
/*
    IMPORTANTE
	Os indices dos TILES sao os mesmos do tileset, embora os tiles no VDP sejam 8x8.
	Logo, precisa considerar que os METATILES 16x16 serao separados em 4 tiles e colocados
	na VRAM continuamento de acordo com as linhas do tileset.
*/
	s16 start_x = screen_x/METATILE_W; // find screen top-left cell in map
	s16 start_y = screen_y/METATILE_W;
	
	for (u8 x = start_x; x < start_x + SCREEN_METATILES_W; ++x) {
		for (u8 y = start_y; y < start_y + SCREEN_METATILES_H; ++y) {
			u16 tile_index = MAP_getTile(map, x*(METATILE_W/8), y*(METATILE_W/8)) & 0x03FF;
			collision_map[x][y] = 0;
			
			if (tile_index != 8) {
				KLog_U1("No zero tile: ", tile_index);
			}
			if (tile_index >= first_index && tile_index <= last_index) {
				collision_map[x][y] = 1;
			}
		}
	}	
}

////////////////////////////////////////////////////////////////////////////
// UPDATE

static inline u8 LEVEL_wall_at(u16 x, u16 y) {
	return collision_map[x/METATILE_W][y/METATILE_W];
}

// GameObject box must be updated before calling this function
u8 LEVEL_check_wall(GameObject* obj) {
	for (u16 x = fix16ToInt(obj->x); x <= obj->right ; x += METATILE_W) {
		for (u16 y = fix16ToInt(obj->y); y <= obj->bottom; y += METATILE_W) {
			if (LEVEL_wall_at(x, y) != 0)
				return TRUE;
		}
	}
	return FALSE;
}

bool LEVEL_move_and_slide(GameObject* obj) {
	// n = (obj->y + obj->height) >> 4;
    // p = (obj->y + (obj->height >> 1)) >> 4;

    // if (obj->speed_x > 0) {      // moving right
    //     m = (obj->next_x + obj->width -1) / 16;
    //     if (map[(obj->y+1) >> 4][m] || map[p][m] || map[n][m]) {
    //         obj->nextx = (m << 4) - obj->width;
    //         set_palette_entry(0, 1, 0b00000011);
    //     }
    // }
    // else if (obj->spdx < 0) { // moving left
    //     m = obj->nextx >> 4;
    //     if (map[(obj->y+1) >> 4][m] || map[p][m] || map[n][m]) {
    //         obj->nextx = (m + 1) << 4;
    //         set_palette_entry(0, 1, 0b00000011);
    //     }
    // }

    // m = (obj->nextx + obj->width -1) >> 4; // update next x position

    // if (obj->spdy > 0) {      // moving down
    //     n = (obj->nexty + obj->height) >> 4;
    //     if (map[n][obj->nextx >> 4] || map[n][m]) {
    //         obj->nexty = (n << 4) - obj->height - 1;
    //         set_palette_entry(0, 1, 0b00000011);
    //     }
    // }
    // else if (obj->spdy < 0) { // moving up
    //     n = (obj->nexty+1) >> 4;
    //     if (map[n][obj->nextx >> 4] || map[n][m]) {
    //         obj->nexty = ((n + 1) << 4) - 1;
    //         set_palette_entry(0, 1, 0b00000011);
    //     }
    // }
}

////////////////////////////////////////////////////////////////////////////
// DRAWING AND FX

void LEVEL_draw_collision_map() {
    VDP_setTextPlane(BG_B);
	for (u8 x = 0; x < SCREEN_METATILES_W; ++x) {
		for (u8 y = 0; y < SCREEN_METATILES_H; ++y) {
			// if (collision_map[x][y] != 8) {
				intToStr(collision_map[x][y], text, 1);
				// TILE_W/8 = how many hardware tiles are being used for a game tile
				VDP_drawText(text, x*METATILE_W/8, y*METATILE_W/8); 
			// } else {
				// VDP_drawText(" ", x*TILE_W/8, y*TILE_W/8); 
			// }
		}
	}
}