#include <genesis.h>
#include "level2.h"

TileMap* tilemap;

// Top-Left screen position in map
// static u16 screen_x = 0;
// static u16 screen_y = 0;


u16 LEVEL2_init(u16 ind) {
    tilemap = unpackTileMap(&level1_tilemap, NULL);
	
    PAL_setPalette(PAL_MAP, level1_pal.data, DMA);
	VDP_loadTileSet(&level1_tiles, ind, DMA);

	// map = MAP_create(&level1_map, BG_MAP, TILE_ATTR_FULL(PAL_MAP, FALSE, FALSE, FALSE, ind));
	
	// MAP_scrollToEx(map, 0, 0, TRUE);
	// // VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);
	
	// LEVEL_generate_screen_collision_map(IDX_WALL_FIRST, IDX_WALL_LAST);
	
    VDP_fillTileMapRect(VDP_BG_A, TILE_ATTR_FULL(PAL_MAP, 0, 0, 0, ind), 0, 0, 40, 28);

	ind += level1_tiles.numTile;

	return ind;
}