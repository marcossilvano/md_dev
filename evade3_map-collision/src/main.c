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
 * 
 * TILED -> customizar grade em preferências
 * GENS -> habilitar recursos avançados de debug
 *
 * PROBLEMAS
 * ------------------------------
 * Mudar imagem de tileset fez com que o Rescomp nao encotrasse mais os tiles.
 * SOLUCAO: criar novo arquivo tsx com a mesma PNG dos tiles e usar a opção "replace tilset" do Tiled.
 * 
 * IDEIAS
 * ------------------------------
 * Bola que quebra blocos (mas vc não). Pode quebrar na primeia, passando através do bloco.
 * Bolas passam pelas paredes de choque.
 */
#include <genesis.h>
#include <sprite_eng.h>

#include "globals.h"
#include "resources.h"

#include "gameobject.h"
#include "utils.h"
#include "player.h"
#include "background.h"
#include "level.h"

// index for tiles in VRAM (first tile reserved for SGDK)
// u16 ind = 1;
u16 ind = TILE_USER_INDEX;

// glow color effect
u8 bg_colors_delay = 5;
const u16 const bg_color_glow[] = {0x0, 0x222, 0x444, 0x666, 0x888};

#define MAX_OBJ 1
GameObject balls[MAX_OBJ];

////////////////////////////////////////////////////////////////////////////
// DRAWING AND FX

inline void draw_info() {
	// VDP_setTextPlane(WINDOW);
	// VDP_setWindowVPos(FALSE, 3);

	VDP_drawText("ENERGY |||||||||   GEMS 255", 1, 0);
	VDP_drawText("DPAD - moves the ship", 1, 1);
	VDP_drawText("DPAD - moves the ship", 1, 2);
}

////////////////////////////////////////////////////////////////////////////
// GAME INIT

void game_init() {
	VDP_setScreenWidth320();
	SPR_init();
	
	// VDP_setTextPlane(WINDOW);
	// VDP_setWindowHPos(FALSE, 0);
	VDP_setWindowVPos(FALSE, 3);

	#ifdef DEBUG
	VDP_setTextPlane(BG_B);
	#else	
	ind += BACKGROUND_init(ind);
	#endif

	ind += LEVEL_init(ind);
	LEVEL_generate_screen_collision_map(0, 5);

	#ifdef DEBUG
	LEVEL_draw_collision_map();
	#endif
	PLAYER_init(ind);

	VDP_drawImageEx(WINDOW, &img_hud, TILE_ATTR_FULL(PAL_MAP, 1, 0, 0, ind), 0, 0, FALSE, DMA);
	ind += img_hud.tileset->numTile;

	ind += GAMEOBJECT_init(&balls[0], &spr_ball, 100, 100, PAL_NPC, ind);
	SPR_setAnim(balls[0].sprite, 3);
	// for (u8 i = 0; i < MAX_OBJ; ++i) {
		// GAMEOBJECT_init(&objects[i], &spr_ball, (i+1)*24, 20*16);
		
	// 	SPR_setAutoTileUpload(objects[i].sprite, FALSE);
	// 	SPR_setFrameChangeCallback(objects[i].sprite, &frame_changed);

	// 	robot_tile_indexes = SPR_loadAllFrames(&spr_robot, ind, &num_tiles);
	// 	ind += num_tiles;

	// 	SPR_setAnim(objects[i].sprite, 1);
	// }

	// draw_info();
}

void hud_init() {
	// Initilizes text UI
	// VDP_setTextPalette(PAL_MAP);
	// VDP_setBackgroundColor(PAL_MAP*16 + 12);
	

	// draw_info();
	// LEVEL_draw_collision_map();
}

////////////////////////////////////////////////////////////////////////////
// GAME LOGIC

static inline void color_effects() {
	--bg_colors_delay;
	if (bg_colors_delay == 0) {
		// rotate_colors_left(50, 55);
		glow_color(bg_color_glow, 5);

		bg_colors_delay = 15;
	}
}

inline void update_camera(GameObject* obj) {
	if (obj->x > (FIX16(SCREEN_W) - obj->w/2)) {
		obj->x = 0;
		screen_x += SCREEN_W;
		MAP_scrollTo(map, screen_x, screen_y);
		//LEVEL_generate_screen_collision_map();
	} else
	if (obj->x < (FIX16(-obj->w/2))) {
		obj->x = FIX16(SCREEN_W - obj->w);
		screen_x -= SCREEN_W;
		MAP_scrollTo(map, screen_x, screen_y);
	}

	if (obj->y > (FIX16(SCREEN_H) - obj->h/2)) {
		obj->y = 0;
		screen_y += SCREEN_H;
		MAP_scrollTo(map, screen_x, screen_y);
	} else
	if (obj->y < (FIX16(-obj->h/2))) {
		obj->y = FIX16(SCREEN_H - obj->h);
		screen_y -= SCREEN_H;
		MAP_scrollTo(map, screen_x, screen_y);
	}
}

static inline void game_update() {
	update_input();

	PLAYER_update();

	#ifndef DEBUG
	BACKGROUND_update();
	#endif

	// update_camera(&player);
	color_effects();
}

////////////////////////////////////////////////////////////////////////////
// MAIN

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
