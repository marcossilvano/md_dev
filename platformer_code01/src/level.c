#include "level.h"

Map* map;
u8 collision_map[MAP_W/TILE_W][MAP_H/TILE_W] = {0}; // size of screen

fix16 offset_mask[SCREEN_H/TILE_W] = {0}; // 224 px / 8 px = 28
fix16 offset_speed[SCREEN_H/TILE_W];

////////////////////////////////////////////////////////////////////////////
// INITIALIZATION

void LEVEL_generate_collision_map_preview() {
	for (u8 x = 0; x < MAP_TILES_W; x++) {
		for (u8 y = 0; y < MAP_TILES_H; y++) {									          // 0000 0111 1111 1111
			collision_map[x][y] = (MAP_getTile(map, x*TILE_W/8, y*TILE_W/8) & 0x07FF) % 10;   //    0    7    F    F
		}
	}
}

void LEVEL_generate_collision_map(u16* ground_tiles, u16 n) {
	for (u8 x = 0; x < MAP_TILES_W; x++) {
		for (u8 y = 0; y < MAP_TILES_H; y++) {
			u16 tile_index = MAP_getTile(map, x*TILE_W/8, y*TILE_W/8) & 0x07FF;
			collision_map[x][y] = 0;
			
			// if (tile_index != 0) {
				KLog_U1("No zero tile: ", tile_index);
				for (u8 i = 0; i < n; i++) {
					if (tile_index == ground_tiles[i]) {
						collision_map[x][y] = 1;
						break;
					}
				}
			// }
		}
	}	
}

void LEVEL_init(u16* ind) {
/*
	PAL_setPalette(PAL_BACKGROUND, img_bg.palette->data, DMA);
	VDP_drawImageEx(BG_B, &img_bg, TILE_ATTR_FULL(PAL_BACKGROUND, 0, 0, 0, *ind), 0, 0, FALSE, DMA);
	*ind += img_bg.tileset->numTile;
*/
	PAL_setPalette(PAL_LEVEL, level1_palette.data, DMA);
	VDP_loadTileSet(&level1_tileset, *ind, DMA);
	map = MAP_create(&level1_map_tmx, BG_B, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, *ind));
	*ind += level1_tileset.numTile;
	MAP_scrollTo(map, 0, 0); // MAP_scrollToEx?

	VDP_setScrollingMode(HSCROLL_TILE, VSCROLL_COLUMN);

	/* IMPORTANTE
	   Os indices dos tiles de colisao podem ser obtidos pelo Gens em:
	   	CPU > Debug > Genesis > VDP
	   OBS: O SGDK considera o índice (idx) da tabela de tiles no Gens, excluindo 1, isto é: idx - 1.
	 */
	LEVEL_generate_collision_map((u16[]){0, 1, 2, 13}, 4);
	//LEVEL_generate_collision_map_preview();
}

////////////////////////////////////////////////////////////////////////////
// GAME LOOP/LOGIC

u8 LEVEL_tile_at(u16 x, u16 y) {
	return collision_map[x/TILE_W][y/TILE_W];
}

u8 LEVEL_wall_at(GameObject* obj) {
	for (u16 x = obj->x; x <= obj->x + obj->w ; x += TILE_W) {
	// for (u16 x = obj->x; x <= obj->x + 32; x += 8) {
		for (u16 y = obj->y; y <= obj->y + obj->h; y += TILE_W) {
		// for (u16 y = obj->y; y <= obj->y + 32; y += 8) {
			if (LEVEL_tile_at(x, y) != 0)
				return TRUE;
		}
	}
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////
// DRAWING AND FX

void LEVEL_draw_collision_map() {
	for (u8 x = 0; x < SCREEN_W/TILE_W; x++) {
		for (u8 y = 0; y < SCREEN_H/TILE_W; y++) {
			if (collision_map[x][y] != 0) {
				intToStr(collision_map[x][y], text, 1);
				// TILE_W/8 = how many hardware tiles are being used for a game tile
				VDP_drawText(text, x*TILE_W/8, y*TILE_W/8); 
			} else {
				VDP_drawText(" ", x*TILE_W/8, y*TILE_W/8); 
			}
		}
	}
}