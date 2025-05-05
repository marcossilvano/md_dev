/**
 * Para compilar & rodar:
 * ---------------------
 *   CTRL + SHIFT + B   (gera out/rom.bin )			<< compilar
 *   [F1], Run Task, Run Gens						<< executar
 *
 * Para compilar & rodar (com extensão Genesis Code):
 * -------------------------------------------------
 *  OBS: instalar extensao Genesis Code e configurar "Gens Path"
 *
 *  Executar: $ cmd									<< pelo terminal do VSCode (extensão não funciona em PowerShell)
 *  F1 -> Genesis Code Compile Project				<< compilar
 *  F1 -> Genesis Code Compiler & Run Project		<< compilar & executar
 * 
  * LINUX
 * ==================================================================
 * Considerando que o caminho seja ~/sgdk200, para fazer build:
 * 
 * $ make GDK=~/sgdk200 -f ~/sgdk200/makefile_wine.gen
 */
#include <genesis.h>
#include <sprite_eng.h>

#include "globals.h"
#include "resources.h"

#include "gameobject.h"
#include "utils.h"
#include "player.h"

// index for tiles in VRAM (first tile reserved for SGDK)
// u16 ind = 1;
u16 ind = TILE_USER_INDEX;

#define PLANE_W FIX16(256)

// parallax scroling
fix16 offset_pos[SCREEN_TILES_H] = {0}; // 224 px / 8 px = 28
fix16 offset_speed[SCREEN_TILES_H] = {0};
s16 values[SCREEN_TILES_H];

////////////////////////////////////////////////////////////////////////////
// DRAWING AND FX

inline void draw_info() {
	VDP_drawText("PARALLAX SCROLLING", 1, 1);
	VDP_drawText("DPAD - moves the ship", 3, 2);
}

////////////////////////////////////////////////////////////////////////////
// GAME LOGIC

static inline void set_offset_speed(u8 start, u8 len, fix16 speed) {
	if (start+len-1 >= SCREEN_TILES_H) {
		return;
	}
	for (u8 i = start; i <= start+len-1; i++) {
		offset_speed[i] = speed;
	}
}

void BACKGROUND_update() {
	for (u8 i = 0; i < SCREEN_TILES_H; i++) {
		if (offset_pos[i] > PLANE_W) {
			offset_pos[i] -= PLANE_W;
		}
		// store next offset in fix16
		offset_pos[i] += offset_speed[i];
		
		// cast to integer to input on VDP
		values[i] = fix16ToInt(offset_pos[i]);
	}

	VDP_setHorizontalScrollTile(BG_B, 0, values, SCREEN_TILES_H, DMA);
}

inline void game_update() {
	update_input();

	PLAYER_update();
	BACKGROUND_update();
}

////////////////////////////////////////////////////////////////////////////
// GAME INIT

u16 BACKGROUND_init() {
	VDP_setPlaneSize(32,32, TRUE);

	// PAL_setPalette(PAL_BACKGROUND, img_background.palette->data, CPU);
	VDP_drawImageEx(BG_B, &img_background, TILE_ATTR_FULL(PAL_BACKGROUND, 0, 0, 0, ind), 0, 0, TRUE, DMA);

	VDP_setScrollingMode(HSCROLL_TILE , VSCROLL_COLUMN);
	
	f16 speed = FIX16(-0.05);
	for (u8 i = 11; i < 255; --i) {
		set_offset_speed(i, 1, speed);
		set_offset_speed(SCREEN_TILES_H-i-1, 1, speed);
		speed += FIX16(-0.05);
	}	
	
	return img_background.tileset->numTile;
}

void game_init() {
	VDP_setScreenWidth320();
	SPR_init();

	ind += BACKGROUND_init();
	ind += PLAYER_init(ind);
	
	// draw_info();
}

int main(bool resetType) {
	// Soft reset doesn't clear RAM. Can lead to bugs.
	if (!resetType) {
		SYS_hardReset();
	}

	game_init();
	SYS_doVBlankProcess();
	
	while (true) {
		game_update();
		
		SPR_update();
		SYS_doVBlankProcess();
		// update VDP scroll
	}

	return 0;
}
