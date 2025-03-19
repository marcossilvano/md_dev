#include "level.h"

Map* map;
Map* map_bg;
u8 collision_map[MAP_W/TILE_W][MAP_H/TILE_W] = {0}; // size of screen

// fix16 offset_mask[SCREEN_H/TILE_W] = {0}; // 224 px / 8 px = 28
// fix16 offset_speed[SCREEN_H/TILE_W];

////////////////////////////////////////////////////////////////////////////
// INITIALIZATION

void LEVEL_fill_tmx_into_collision_map() {
	for (u8 x = 0; x < MAP_TILES_W; ++x) {
		for (u8 y = 0; y < MAP_TILES_H; ++y) {									          	  // 0000 0111 1111 1111
			collision_map[x][y] = (MAP_getTile(map, x*(TILE_W/8), y*(TILE_W/8)) & 0x07FF) % 10;   //    0    7    F    F
			// KLog_U1("No zero tile: ", collision_map[x][y]);
		}
		// KLog("New Line");
	}
}

void LEVEL_generate_collision_map(u16* ground_tiles, u16 n) {
	for (u8 x = 0; x < MAP_TILES_W; ++x) {
		for (u8 y = 0; y < MAP_TILES_H; ++y) {
			u16 tile_index = MAP_getTile(map, x*(TILE_W/8), y*(TILE_W/8)) & 0x07FF;
			collision_map[x][y] = 0;
			
			// if (tile_index != 8) {
			// 	KLog_U1("No zero tile: ", tile_index);
			// }
			for (u8 i = 0; i < n; i++) {
				if (tile_index == ground_tiles[i]) {
					collision_map[x][y] = 1;
					break;
				}
			}
		}
	}	
}

u16 LEVEL_init(u16 ind) {
	u16 index_vram = ind;

	PAL_setPalette(PAL_MAP, level1_palette.data, DMA);
	VDP_loadTileSet(&level1_tileset, ind, DMA);
	map = MAP_create(&level1_map, BG_A, TILE_ATTR_FULL(PAL_MAP, FALSE, FALSE, FALSE, index_vram));
	
	MAP_scrollToEx(map, 0, 0, TRUE);
	VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);
	
	PAL_setPalette(PAL_BACKGROUND, level1_bg_palette.data, DMA);
	map_bg = MAP_create(&level1_bg, BG_B, TILE_ATTR_FULL(PAL_BACKGROUND, FALSE, FALSE, FALSE, index_vram));
	MAP_scrollToEx(map_bg, 0, 0, TRUE);

	index_vram += level1_tileset.numTile;
/*
    IMPORTANTE
	Os indices dos tiles de colisao podem ser obtidos pelo Gens em:
	CPU > Debug > Genesis > VDP (idx - 1)
	
	OBS: Subtrair 1 da tabela mostrada no Gens para índice correto (idx - 1) 
*/
	LEVEL_generate_collision_map((u16[]){0,1,2,3,4,5,6,7,8,9,10,11,12,13}, 14);
	// LEVEL_fill_tmx_into_collision_map();

	return index_vram;
}

////////////////////////////////////////////////////////////////////////////
// GAME LOOP/LOGIC

u8 LEVEL_check_wall(GameObject* obj) {
	for (u16 x = obj->x; x <= obj->x + obj->w ; x += TILE_W) {
	// for (u16 x = obj->x; x <= obj->x + 32; x += 8) {
		for (u16 y = obj->y; y <= obj->y + obj->h; y += TILE_W) {
		// for (u16 y = obj->y; y <= obj->y + 32; y += 8) {
			if (LEVEL_wall_at(x, y) != 0)
				return TRUE;
		}
	}
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////
// DRAWING AND FX

void LEVEL_draw_collision_map() {
	for (u8 x = 0; x < SCREEN_W/TILE_W; ++x) {
		for (u8 y = 0; y < SCREEN_H/TILE_W; ++y) {
			// if (collision_map[x][y] != 8) {
				intToStr(collision_map[x][y], text, 1);
				// TILE_W/8 = how many hardware tiles are being used for a game tile
				VDP_drawText(text, x*TILE_W/8, y*TILE_W/8); 
			// } else {
				// VDP_drawText(" ", x*TILE_W/8, y*TILE_W/8); 
			// }
		}
	}
}