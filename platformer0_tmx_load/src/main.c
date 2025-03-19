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
 * Para fazer:
 * ----------------------
 * @todo Fundo com parallax
 * @todo Controle de DOWN, PRESSED e RELEASED para botoes
 * @todo Colisão com tiles sólidos
 * @todo Adicionar obstaculos
 * @todo Adicionar inimigos
 *
 * Feito:
 * ----------------------
 * Carrega mapa TMX
 * Controle e pulo do personagem
 * Câmera no level
 * Câmera segue jogador
 * Impedir que o personagem saia do mapa (laterais)
 * Impedir que saia pelo topo da tela
 * Geração de mapa de colisão a partir do tmx * 
  */
#include <genesis.h>

#include "sprite_eng.h"
#include "resources.h"

#include "structs.h"

// index for tiles in VRAM (first tile reserved for SGDK)
u16 ind = 1;
//u16 ind = TILE_USER_INDEX;
Map* map;

s16 map_scroll_x = 0;
s16 map_scroll_y = 0;

////////////////////////////////////////////////////////////////////////////
// DRAWING AND FX

inline void draw_info() {
	VDP_drawText("TMX 16x16 Load Example", 1, 1);
	VDP_drawText("Use DPAD to scroll", 1, 3);
}

////////////////////////////////////////////////////////////////////////////
// MAIN CODE

inline void CAMERA_move() {
	u16 value = JOY_readJoypad(JOY_1);
	
	if (value & BUTTON_LEFT) {
		map_scroll_x -= 3;
	} else
	if (value & BUTTON_RIGHT) {
		map_scroll_x += 3;
	} 

	if (value & BUTTON_UP) {
		map_scroll_y -= 3;
	} else
	if (value & BUTTON_DOWN) {
		map_scroll_y += 3;
	} 

	map_scroll_x = clamp(map_scroll_x, 0, MAP_W - SCREEN_W);
	map_scroll_y = clamp(map_scroll_y, 0, MAP_H - SCREEN_H);

	MAP_scrollTo(map, map_scroll_x, map_scroll_y); 	
}

inline void update() {
	CAMERA_move();
}

inline void init() {
	PAL_setPalette(PAL_LEVEL, level1_palette.data, DMA);
	VDP_loadTileSet(&level1_tileset, ind, DMA);
	map = MAP_create(&level1_map_tmx, BG_B, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, ind));
	ind += level1_tileset.numTile;
	MAP_scrollTo(map, 0, 0); // MAP_scrollToEx?

	VDP_setScrollingMode(HSCROLL_TILE, VSCROLL_COLUMN);
}

int main()
{
	VDP_setScreenWidth320();
	//VDP_setPlaneSize(64, 64, TRUE);

	SPR_init();

	// Initilizes text UI
	VDP_setTextPalette(PAL_LEVEL);
	VDP_setBackgroundColor(13);
	
	init();

	//VDP_setBackgroundColor(PAL_getColor(5));
	draw_info();

	while (1)
	{
		update();
		
		//VDP_setHorizontalScroll(BG_B, hscroll_offset);
		//VDP_setVerticalScroll(BG_B, hscroll_offset);
		//hscroll_offset--;

		SYS_doVBlankProcess();
		// update VDP scroll
	}

	return 0;
}
