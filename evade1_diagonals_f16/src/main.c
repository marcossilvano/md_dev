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
	VDP_drawText("E-VADE", 1, 1);
	VDP_drawText("Use DPAD and A", 1, 2);
	
	VDP_drawText("X: ", 2, 3);
	VDP_drawText("Y: ", 2, 4);
}

////////////////////////////////////////////////////////////////////////////
// GAME LOGIC

inline void game_update() {
	PLAYER_update();
}

////////////////////////////////////////////////////////////////////////////
// GAME INIT

void game_init() {
	VDP_setScreenWidth320();
	// VDP_setPlaneSize(64, 64, TRUE);
	SPR_init();

	// PAL_setPalette(PAL_BACKGROUND, img_background.palette->data, CPU);
	VDP_drawImageEx(BG_A, &img_background, TILE_ATTR_FULL(PAL_BACKGROUND, 0, 0, 0, ind), 0, 0, TRUE, DMA);
	ind += img_background.tileset->numTile;
	KLog_U1("bkg tiles: ", img_background.tileset->numTile);

	ind += PLAYER_init(ind);
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
