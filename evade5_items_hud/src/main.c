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
 * > Mudar imagem de tileset fez com que o Rescomp nao encotrasse mais os tiles (problema no TMX/TSX).
 *   SOLUCAO: criar novo arquivo tsx com a mesma PNG dos tiles e usar a opção "replace tilset" do Tiled.
 * > Mensagens com KLog não aparecem no emulador
 *   SOLUCAO: Option -> Debug -> Active Development Features [x]
 * 
 * IDEIAS
 * ------------------------------
 * Bola que quebra blocos (mas vc não). Pode quebrar na primeia, passando através do bloco.
 * Bolas passam pelas paredes de choque.
 * 
 * TODO
 * ------------------------------
 * [ ] Contabilizar item coletado
 * [ ] Modificar tiles de itens no mapa de forma permanente (restaura quando faz scroll)
 * [ ] Aumentar mapa de colisão para 3 tiles a mais de cada lado, permitindo que o player saia da tela
 * [ ] Gerar mapa de colisão para cada troca de tela
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
#include "hud.h"

// index for tiles in VRAM (first tile reserved for SGDK)
// u16 ind = 1;
u16 ind = TILE_USER_INDEX;

// glow color effect
u8 bg_colors_delay = 5;
const u16 const bg_color_glow[] = {0x0, 0x222, 0x444, 0x666, 0x888};

#define MAX_OBJ 1
GameObject balls[MAX_OBJ];

////////////////////////////////////////////////////////////////////////////
// GAME INIT

void game_init() {
	VDP_setScreenWidth320();
	SPR_init();
	
	// init BACKGROUND, LEVEL AND HUD ///////////////////////////////

	#ifdef DEBUG
	VDP_setTextPlane(BG_BACKGROUND);
	#else	
	ind += BACKGROUND_init(ind);
	#endif

	ind += LEVEL_init(ind);
	LEVEL_generate_screen_collision_map(IDX_WALL_FIRST, IDX_WALL_LAST);
	
	#ifdef DEBUG
	LEVEL_draw_collision_map();
	#endif
	
	ind += HUD_init(ind);
	
	// init GAME OBJECTS ////////////////////////////////////////////

	PLAYER_init(ind);

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

}

////////////////////////////////////////////////////////////////////////////
// GAME LOGIC

static inline void color_effects() {
	--bg_colors_delay;
	if (bg_colors_delay == 0) {
		// rotate_colors_left(PAL_BACKGROUND*16, PAL_BACKGROUND*16+15);
		glow_color(PAL_BACKGROUND*16+8, bg_color_glow, 5);

		bg_colors_delay = 15;
	}
}

inline void scroll_and_update_collision(u16 x, u16 y) {
		MAP_scrollTo(map, x, y);
		LEVEL_generate_screen_collision_map(0, 5);
		#ifdef DEBUG
		LEVEL_draw_collision_map();
		#endif
}

inline void update_camera(GameObject* obj) {
	if (obj->x > (FIX16(SCREEN_W) - obj->w/2)) {
		obj->x = 0;
		screen_x += SCREEN_W;
		scroll_and_update_collision(screen_x, screen_y);
	} else
	if (obj->x < (FIX16(-obj->w/2))) {
		obj->x = FIX16(SCREEN_W - obj->w);
		screen_x -= SCREEN_W;
		MAP_scrollTo(map, screen_x, screen_y);
		scroll_and_update_collision(screen_x, screen_y);
	}
	
	if (obj->y > (FIX16(SCREEN_H) - obj->h/2)) {
		obj->y = 0;
		screen_y += SCREEN_H;
		scroll_and_update_collision(screen_x, screen_y);
	} else
	if (obj->y < (FIX16(-obj->h/2))) {
		obj->y = FIX16(SCREEN_H - obj->h);
		screen_y -= SCREEN_H;
		scroll_and_update_collision(screen_x, screen_y);
	}
}

static inline void game_update() {
	update_input();

	PLAYER_update();

	#ifndef DEBUG
	BACKGROUND_update();
	#endif

	update_camera(&player);
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
	
	SYS_doVBlankProcess();
	
	while (true) {
		game_update();

		SPR_update();
		SYS_doVBlankProcess();
		// update VDP scroll
	}

	return 0;
}
