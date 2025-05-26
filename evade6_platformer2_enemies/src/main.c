/**
 * Upgrade para SGDK 2.11
 * ----------------------
 * Abrir C:\sgdk211\sample\basics\hello-world\src
 * > Copiar pasta "boot" para "src" deste projeto
 * 
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
 * [  ] Efeitos para rastro do movimento das bolas
 * [  ] Efeitos para coleta de itens
 * [  ] Efeitos para ricochetes
 * [  ] Tiles dos itens animados
 * [  ] Inimigos bolas que ricocheteiam
 * [NO] Implementar solução para o Mapa de Itens simplemente usando TileMap descompactado em RAM
 * [OK] Testar consumo de RAM com MAP e sem MAP
 * [OK] Contabilizar item coletado
 * [OK] Modificar tiles de itens no mapa de forma permanente (restaura quando faz scroll)
 * [OK] Aumentar mapa de colisão para 3 tiles a mais de cada lado, permitindo que o player saia da tela
 * [OK] Gerar mapa de colisão para cada troca de tela
 */
#include <genesis.h>
#include <sprite_eng.h>

#include "globals.h"
#include "resources.h"

#include "utils.h"
#include "hud.h"
#include "globals.h"
#include "entities/gameobject.h"
#include "entities/player.h"
#include "entities/enemy.h"
#include "level/background.h"
#include "level/level.h"
#include "level/level2.h"

// index for tiles in VRAM (first tile reserved for SGDK)
// u16 ind = 1;
u16 ind = TILE_USER_INDEX;

// glow color effect
u8 bg_colors_delay = 5;
const u16 bg_color_glow[] = {0x0, 0x222, 0x444, 0x666, 0x888};

#define MAX_OBJ 30
GameObject balls_list[MAX_OBJ];

////////////////////////////////////////////////////////////////////////////
// GAME INIT

/*
y * width + x

screen_width = 100
screen_width = 100
map_width = 300
map_height= 300

0...........99  100........199  200........299
300........399  400........499  500........599
600........699  700........799  800........899  
*/

void init_objects() {
	for (u16 i = 0; i < LEN(level1_objects); ++i) {
		MapObject* p = (MapObject*) level1_objects[i];
		kprintf("room: %d, pos %ld %ld, spd: %d %d", p->room, F32_toInt(p->x), F32_toInt(p->y), F16_toInt(p->speed_x), F16_toInt(p->speed_y));
	}

	// u8* p = (u8*)level1_objects;
	// for (u16 i = 0; i < 6; ++i) {
	// 	kprintf("%X", *p);
	// 	p++;
	// }
}
// 7   0111
// 0   0000 0000
// 1   0000 0001
// 5   0000 0101
// 34  0010 0010

void init_balls() {
	// u16 num_tiles;
	// ball_indexes = SPR_loadAllFrames(&spr_ball, ind, &num_tiles);
	u16 ball_ind = ind;
	ind += ENEMY_load_tiles(ind);
	
	GameObject* ball = &balls_list[0];
	for (u8 i = 0; i < MAX_OBJ; ++i, ++ball) {
		ENEMY_init(ball, (SCREEN_W-8)/2, (SCREEN_H-8)/2, ball_ind);
	}
}

void game_init() {
	VDP_setScreenWidth320();
	SPR_init();
	
	// init BACKGROUND, LEVEL AND HUD ///////////////////////////////

	#ifdef DEBUG
	VDP_setTextPlane(BG_BACKGROUND);
	#else	
	ind += BACKGROUND_init(ind);
	#endif

	#if GAME_MODE == MODE_SHOOTER
	ind += LEVEL_init(ind);
	#endif
	
	#if GAME_MODE == MODE_PLATFORMER
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

	init_balls();
	init_objects();
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

inline void update_enemies() {
	GameObject* ball = &balls_list[0];
	for (u8 i = 0; i < MAX_OBJ; ++i, ++ball) {
		ENEMY_update(ball);
	}
}

static inline void game_update() {
	update_input();

	PLAYER_update();
	update_enemies();

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
	}

	return 0;
}
