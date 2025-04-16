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
 * Feito:
 * ----------------------
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

u16 bg_scroll = 0;
u8 bg_colors_delay = 5;

#define MAX_OBJ 1
GameObject objects[MAX_OBJ];

////////////////////////////////////////////////////////////////////////////
// DRAWING AND FX

inline void draw_info() {
	VDP_drawText("E-VADE", 1, 1);
	VDP_drawText("Use DPAD and A", 1, 2);
	
	VDP_drawText("X: ", 2, 3);
	VDP_drawText("Y: ", 2, 4);
}

////////////////////////////////////////////////////////////////////////////
// GAME LOGIC

inline void game_update() {
	PLAYER_update();

	// --bg_colors_delay;
	// if (bg_colors_delay == 0) {
	// 	rotate_colors_left(35, 38);
	// 	bg_colors_delay = 10;
	// }
}

////////////////////////////////////////////////////////////////////////////
// GAME INIT

void game_init() {
	VDP_setScreenWidth320();
	SPR_init();

	// PAL_setPalette(PAL_BACKGROUND, img_background.palette->data, CPU);
	VDP_drawImageEx(BG_A, &img_background, TILE_ATTR_FULL(PAL_BACKGROUND, 0, 0, 0, ind), 0, 0, TRUE, DMA);
	ind += img_background.tileset->numTile;
	KLog_U1("bkg tiles: ", img_background.tileset->numTile);

	ind += PLAYER_init(ind);
	

	// for (u8 i = 0; i < MAX_OBJ; ++i) {
		// GAMEOBJECT_init(&objects[i], &spr_ball, (i+1)*24, 20*16);
		
	// 	SPR_setAutoTileUpload(objects[i].sprite, FALSE);
	// 	SPR_setFrameChangeCallback(objects[i].sprite, &frame_changed);

	// 	robot_tile_indexes = SPR_loadAllFrames(&spr_robot, ind, &num_tiles);
	// 	ind += num_tiles;

	// 	SPR_setAnim(objects[i].sprite, 1);
	// }
}

void hud_init() {
	// Initilizes text UI
	// VDP_setTextPalette(PAL_MAP);
	// VDP_setBackgroundColor(PAL_MAP*16 + 12);
	
	// VDP_setTextPlane(WINDOW);
	// VDP_setWindowHPos(FALSE, 0);
	// VDP_setWindowVPos(FALSE, 5);

	// draw_info();
	// LEVEL_draw_collision_map();
}

int main(bool resetType) {
	// Soft reset doesn't clear RAM. Can lead to bugs.
	if (!resetType) {
		SYS_hardReset();
	}
	
	// VDP_setPlaneSize(64, 64, TRUE);
	game_init();
	hud_init();
	
	SYS_doVBlankProcess();
	
	while (true) {
		game_update();
		SPR_update();
		SYS_doVBlankProcess();
		// update VDP scroll
	}

	return 0;
}
