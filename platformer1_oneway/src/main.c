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
#include "player.h"
#include "level.h"

// index for tiles in VRAM (first tile reserved for SGDK)
u16 ind = 1;
//u16 ind = TILE_USER_INDEX;
u8 color_delay = 5;

s16 map_scroll_x = 0;
s16 map_scroll_y = 0;

////////////////////////////////////////////////////////////////////////////
// DRAWING AND FX

inline void draw_position()  {
	intToStr(player.x, text, 6);
	VDP_drawText(text, 5, 23);

	intToStr(player.y, text, 6);
	VDP_drawText(text, 5, 24);
}

inline void draw_info() {
	VDP_drawText("PLATFORMER SAMPLE", 1, 1);
	VDP_drawText("Use DPAD and A", 1, 3);
	
	VDP_drawText("X: ", 2, 23);
	VDP_drawText("Y: ", 2, 24);
}

////////////////////////////////////////////////////////////////////////////
// DRAWING AND FX

inline void rotate_colors(u8 first_index, u8 last_index, s8 direction) {
	if (color_delay-- == 0) {
		u16 last_color = PAL_getColor(first_index);
		for (u8 i = first_index; i != last_index; i += direction) {
			PAL_setColor(i, PAL_getColor(i + direction));
		}
		PAL_setColor(last_index, last_color);
		color_delay = 5;	
	}
}

inline void rotate_colors_left(u8 left_index, u8 right_index) {
	rotate_colors(left_index, right_index, 1);
}

inline void rotate_colors_right(u8 left_index, u8 right_index) {
	rotate_colors(right_index, left_index, -1);
}


////////////////////////////////////////////////////////////////////////////
// MAIN CODE

inline void CAMERA_follow(GameObject* obj) {
	map_scroll_x = obj->x - SCREEN_W/2 + obj->w/2;
	//map_scroll_y = obj->y - SCREEN_H/2;

	map_scroll_x = clamp(map_scroll_x, 0, MAP_W - SCREEN_W);
	map_scroll_y = clamp(map_scroll_y, 0, MAP_H - SCREEN_H);

	MAP_scrollTo(map, map_scroll_x, map_scroll_y); 	
	MAP_scrollTo(map, map_scroll_x, map_scroll_y); 	
}

inline void CAMERA_move() {
	u16 value = JOY_readJoypad(JOY_1);
	
	if (value & BUTTON_LEFT) {
		map_scroll_x -= 3;
	} else
	if (value & BUTTON_RIGHT) {
		map_scroll_x += 3;
	} 
	map_scroll_x = clamp(map_scroll_x, 0, MAP_W - SCREEN_W);

	MAP_scrollTo(map, map_scroll_x, map_scroll_y); 	
}

inline void update() {
	// PLAYER_update_nocollision();
	PLAYER_update();
	CAMERA_follow(&player);
	//SPR_setPosition(player.sprite, player.x - map_scroll_x, player.y - map_scroll_y);
	
	//CAMERA_move();
}

int main()
{
	VDP_setScreenWidth320();
	//VDP_setPlaneSize(64, 64, TRUE);
	
	LEVEL_init(&ind);
	SPR_init();
	PLAYER_init(&ind);

	// Initilizes text UI
	VDP_setTextPalette(PAL_LEVEL);
	VDP_setBackgroundColor(13);
	
	//VDP_setBackgroundColor(PAL_getColor(5));
	draw_info();
	// LEVEL_draw_collision_map();

	SYS_doVBlankProcess();
	for (int i = 14; i <= 21; i++) {
		VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL_LEVEL, TRUE, FALSE, FALSE, 5), 14, i);
		VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL_LEVEL, TRUE, FALSE, FALSE, 6), 15, i);
	}

	while (1)
	{
		// handle input
		update();

		draw_position();
		
		//VDP_setHorizontalScroll(BG_B, hscroll_offset);
		//VDP_setVerticalScroll(BG_B, hscroll_offset);
		//hscroll_offset--;

		rotate_colors_left(PAL_LEVEL*16+4, PAL_LEVEL*16+6);
		// update physics
		// update animations
		// update sprites
		
		SPR_setPosition(player.sprite, player.x-map_scroll_x, player.y-map_scroll_y);
		SPR_update();

		SYS_doVBlankProcess();
		// update VDP scroll
	}

	return 0;
}
