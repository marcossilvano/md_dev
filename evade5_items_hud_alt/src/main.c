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
 * [ ] Implementar solução para o Mapa de Itens simplemente usando TileMap descompactado em RAM
 * [ ] Testar consumo de RAM com MAP e sem MAP
 * [X] Contabilizar item coletado
 * [X] Modificar tiles de itens no mapa de forma permanente (restaura quando faz scroll)
 * [X] Aumentar mapa de colisão para 3 tiles a mais de cada lado, permitindo que o player saia da tela
 * [X] Gerar mapa de colisão para cada troca de tela
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
#include "level2.h"
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

	#if MAP_SOLUTION == MAP_BY_COMPACT_MAP
	ind += LEVEL_init(ind);
	#endif

	#if MAP_SOLUTION == MAP_BY_TILEMAP_RAM
	ind += LEVEL2_init(ind);
	#endif

	#ifdef DEBUG
	LEVEL_draw_map();
	#endif
	
	#ifndef DEBUG
	ind += HUD_init(ind);
	#endif
	
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

static inline void game_update() {
	update_input();

	PLAYER_update();
	// LEVEL_generate_screen_collision_map(0,5);

	#ifndef DEBUG
	BACKGROUND_update();
	#endif

	#if MAP_SOLUTION == MAP_BY_COMPACT_MAP
	LEVEL_update_camera(&player);
	#endif
	color_effects();
}

////////////////////////////////////////////////////////////////////////////
// MAIN

int main(bool resetType) {
	// Soft reset doesn't clear RAM. Can lead to bugs.
	if (!resetType) {
		SYS_hardReset();
	}
	SYS_showFrameLoad(true);
	game_init();

	SYS_doVBlankProcess();
	
	kprintf("Free RAM after Game Init: %d", MEM_getFree());

	while (true) {
		game_update();

		SPR_update();
		SYS_doVBlankProcess();

		// VDP_clearTileMapRect(VDP_BG_A, 4, 4, 4, 4);
		// LEVEL_restore_items(screen_y/SCREEN_H * 3 + screen_x/SCREEN_W);

		// TileMap* tilemap = unpackTileMap();
		// VDP_setTileMapDataRect

		// kprintf("free RAM: %d", MEM_getFree());
		// update VDP scroll
	}

	return 0;
}
