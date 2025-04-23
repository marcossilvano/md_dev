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

////////////////////////////////////////////////////////////////////////////
// DRAWING AND FX

inline void draw_info() {
	VDP_drawText("PLAYER MOVEMENTS", 1, 1);
	VDP_drawText("A - 4 directions continuous", 2, 2);
	VDP_drawText("B - 8 directions", 2, 3);
	VDP_drawText("C - turn aod move", 2, 4);
}

inline void draw_arrow() {
	VDP_drawText(" ", 1, 2);
	VDP_drawText(" ", 1, 3);
	VDP_drawText(" ", 1, 4);

	VDP_drawText(">", 1, 2 + move_type);
}

////////////////////////////////////////////////////////////////////////////
// GAME LOGIC

inline void game_update() {
	update_input();

	if (key_pressed(JOY_1, BUTTON_A)) {
		move_type = 0;
		draw_arrow();
	} else
	if (key_pressed(JOY_1, BUTTON_B)) {
		move_type = 1;
		draw_arrow();
	} else
	if (key_pressed(JOY_1, BUTTON_C)) {
		move_type = 2;
		draw_arrow();
	}

	PLAYER_update();
}

////////////////////////////////////////////////////////////////////////////
// GAME INIT

void game_init() {
	VDP_setScreenWidth320();
	// VDP_setPlaneSize(64, 64, TRUE);
	SPR_init();

	// PAL_setPalette(PAL_BACKGROUND, img_background.palette->data, CPU);
	VDP_drawImageEx(BG_B, &img_background, TILE_ATTR_FULL(PAL_BACKGROUND, 0, 0, 0, ind), 0, 0, TRUE, DMA);
	ind += img_background.tileset->numTile;
	KLog_U1("bkg tiles: ", img_background.tileset->numTile);

	ind += PLAYER_init(ind);

	draw_info();
	draw_arrow();
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
