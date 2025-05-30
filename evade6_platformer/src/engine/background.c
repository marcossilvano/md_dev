#include "background.h"


////////////////////////////////////////////////////////////////////////////
// PRIVATE MEMBERS

// parallax scroling (raster effects)
static fix16 offset_pos[SCREEN_TILES_H] = {0}; // 224 px / 8 px = 28
static fix16 offset_speed[SCREEN_TILES_H] = {0};
static s16 values[SCREEN_TILES_H] = {0};

static inline void set_offset_speed(u8 start, u8 len, fix16 speed);

////////////////////////////////////////////////////////////////////////////
// INIT

u16 BACKGROUND_init(u16 ind, f16 start_speed, f16 speed_increase) {
	VDP_setPlaneSize(64, 64, TRUE);
	
	// PAL_setPalette(PAL_BACKGROUND, img_background.palette->data, CPU);
	VDP_drawImageEx(BG_BACKGROUND, &img_background, TILE_ATTR_FULL(PAL_BACKGROUND, 0, 0, 0, ind), 0, 0, TRUE, DMA);
	
	VDP_setScrollingMode(HSCROLL_TILE , VSCROLL_COLUMN);
	
	f16 speed = start_speed;
	for (u8 i = 11; i < 255; --i) {
		set_offset_speed(i, 1, speed);
		set_offset_speed(SCREEN_TILES_H-i-1, 1, speed);
		speed += speed_increase;
	}
    set_offset_speed(11, 6, FIX16(-0.05));
	
	// set the window to 1 to last row
	// VDP_setWindowVPos(true, 1);
	
	return img_background.tileset->numTile;
}

////////////////////////////////////////////////////////////////////////////
// UPDATE

void BACKGROUND_update() {
	for (u8 i = 0; i < SCREEN_TILES_H; i++) {
		// restart plane position when reaching screen width
		// if (offset_pos[i] > FIX16(SCREEN_W)) {
		// 	offset_pos[i] -= FIX16(SCREEN_W);
		// }

		// store next offset in fix16
		offset_pos[i] += offset_speed[i];
		
		// cast to integer to input on VDP
		values[i] = fix16ToInt(offset_pos[i]);
	}

	VDP_setHorizontalScrollTile(BG_BACKGROUND, 0, values, SCREEN_TILES_H, DMA);
}

////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS

/**
 * Utilitary function to simplify setting more than one offset_speed vector position
 */
static inline void set_offset_speed(u8 start, u8 len, fix16 speed) {
	if (start+len-1 >= SCREEN_TILES_H) {
		return;
	}
	for (u8 i = start; i <= start+len-1; i++) {
		offset_speed[i] = speed;
	}
}
