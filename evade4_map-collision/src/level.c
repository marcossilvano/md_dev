#include <genesis.h>
#include "level.h"

Map* map;
u8 collision_map[SCREEN_W/METATILE_W][SCREEN_H/METATILE_W]; // screen collision map

// Top-Left screen position in map
u16 screen_x = 0;
u16 screen_y = 0;

u8 collision_result;

////////////////////////////////////////////////////////////////////////////
// INIT

u16 LEVEL_init(u16 ind) {
	PAL_setPalette(PAL_MAP, level1_pal.data, DMA);
	VDP_loadTileSet(&level1_tiles, ind, DMA);
	map = MAP_create(&level1_map, BG_A, TILE_ATTR_FULL(PAL_MAP, FALSE, FALSE, FALSE, ind));
	
	MAP_scrollToEx(map, 0, 0, TRUE);
	// VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);
	
	ind += level1_tiles.numTile;

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
			
			// if (tile_index != 8) {
			// 	KLog_U1("No zero tile: ", tile_index);
			// }
			if (tile_index >= first_index && tile_index <= last_index) {
				collision_map[x][y] = 1;
			}
		}
	}	
}

////////////////////////////////////////////////////////////////////////////
// UPDATE

// GameObject box must be updated before calling this function
// OBS: this function only checks for objects that are multiple of 16 pixels
u8 LEVEL_check_wall(GameObject* obj) {
	for (u16 x = obj->box.left; x <= obj->box.right ; x += METATILE_W) {
		for (u16 y = obj->box.top; y <= obj->box.bottom; y += METATILE_W) {
			if (LEVEL_wall_at(x, y) != 0)
				return true;
		}
	}
	return false;
}

/**
 * Checks and resolves wall collisions. *  
 * OBS:
 * - To access collision result, use LEVEL_collision_result()
 */ 
void LEVEL_move_and_slide(GameObject* obj) {
	collision_result = 0;
	GAMEOBJECT_update_boundbox(obj->next_x, obj->y, obj);

	/*
	+---------+  <- right,top
	|         |     				} up to 16 px
	|         |  <- right,top+h/2
	|         |      				} up to 16 px
	+---------+  <- right, bottom
	*/
	if (obj->speed_x > 0) {				// moving right
		if (LEVEL_wall_at(obj->box.right, obj->box.top) || 
	    	LEVEL_wall_at(obj->box.right, obj->box.top + obj->h/2) || 
			LEVEL_wall_at(obj->box.right, obj->box.bottom-1)) {
				obj->next_x = FIX16(obj->box.right/METATILE_W * METATILE_W - obj->w);
				collision_result |= COLLISION_RIGHT;
		}
	}
	/*
	    left, top ->  +---------+
	                  |         |    } up to 16 px
	left, top+h/2 ->  |         |
	                  |         |    } up to 16 px
	 left, bottom ->  +---------+
	*/
	else 
	if (obj->speed_x < 0) {			// moving left
		if (LEVEL_wall_at(obj->box.left, obj->box.top) || 
			LEVEL_wall_at(obj->box.left, obj->box.top + obj->h/2) || 
			LEVEL_wall_at(obj->box.left, obj->box.bottom-1)) {
				obj->next_x = FIX16((obj->box.left/METATILE_W + 1) * METATILE_W);
				collision_result |= COLLISION_LEFT;
		}
	}

	GAMEOBJECT_update_boundbox(obj->next_x, obj->next_y, obj);
	/*
	         left+w/2,
	           top
    left,top    |    right,top
	       |    |    |
 	       +---------+ 
	       |         | 
	       |         | 
	       |         | 
	       +---------+ 
	*/
	if (obj->speed_y < 0) {        // moving up
		if (LEVEL_wall_at(obj->box.left,  obj->box.top) || 
			LEVEL_wall_at(obj->box.left + obj->w/2, obj->box.top) || 
			LEVEL_wall_at(obj->box.right-1, obj->box.top)) {
				obj->next_y = FIX16((obj->box.top/METATILE_W + 1) * METATILE_W);
				collision_result |= COLLISION_TOP;
		}
    }
	/*
	         left+w/2,
	           top
    left,top    |    right,top
	       |    |    |
 	       +---------+ 
	       |         | 
	       |         | 
	       |         | 
	       +---------+ 
	*/
	else
	if (obj->speed_y > 0) {   // moving down
		if (LEVEL_wall_at(obj->box.left,  obj->box.bottom) || 
			LEVEL_wall_at(obj->box.left + obj->w/2, obj->box.bottom) || 
			LEVEL_wall_at(obj->box.right-1, obj->box.bottom)) {
				obj->next_y = FIX16((obj->box.bottom/METATILE_W) * METATILE_W - obj->h);
				collision_result |= COLLISION_BOTTOM;
		}
    }
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