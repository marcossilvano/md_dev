#include "level.h"

Map* map;
Map* map_bg;
u8 collision_map[MAP_W/TILE_W][MAP_H/TILE_W] = {0}; // size of screen
u8 tile_frame = 0;

const u8* const animated_tiles_map[NUMBER_OF_LEVELS] = {
	(u8[]){1,2,3,4,5,6}

	// animation delay
	// TileSet pointer
	// tile indexes to be replaced
	// 
};

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

static u16 LEVEL_load_animated_tiles(u16 ind) {
	// animated tiles
	VDP_loadTileSet(&animated_tiles2, ind, DMA);
	ind += animated_tiles2.numTile;
	return ind;
}

u16 LEVEL_init(u16 ind) {
	PAL_setPalette(PAL_MAP, level1_palette.data, DMA);
	VDP_loadTileSet(&level1_tileset, ind, DMA);
	map = MAP_create(&level1_map, BG_A, TILE_ATTR_FULL(PAL_MAP, FALSE, FALSE, FALSE, ind));
	
	MAP_scrollToEx(map, 0, 0, TRUE);
	VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);
	
	PAL_setPalette(PAL_BACKGROUND, level1_bg_palette.data, DMA);
	map_bg = MAP_create(&level1_bg, BG_B, TILE_ATTR_FULL(PAL_BACKGROUND, FALSE, FALSE, FALSE, ind));
	MAP_scrollToEx(map_bg, 0, 0, TRUE);

	ind += level1_tileset.numTile;
/*
    IMPORTANTE
	Os indices dos tiles de colisao podem ser obtidos pelo Gens em:
	CPU > Debug > Genesis > VDP (idx - 1)
	
	OBS: Subtrair 1 da tabela mostrada no Gens para índice correto (idx - 1) 
*/
	LEVEL_generate_collision_map((u16[]){0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15}, 16);
	// LEVEL_fill_tmx_into_collision_map();

	ind += LEVEL_load_animated_tiles(ind);

	return ind;
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

void LEVEL_animate_tiles() {
	tile_frame = !tile_frame;
			
	if (tile_frame) {
		VDP_loadTileData( (const u32 *)animated_tiles2.tiles, 0x0F, 2, DMA_QUEUE);
		VDP_loadTileData( (const u32 *)animated_tiles2.tiles+16, 0x1B, 2, DMA_QUEUE);
	} else {
		VDP_loadTileData( (const u32 *)animated_tiles2.tiles+32, 0x0F, 2, DMA_QUEUE);
		VDP_loadTileData( (const u32 *)animated_tiles2.tiles+48, 0x1B, 2, DMA_QUEUE);
	}
}

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