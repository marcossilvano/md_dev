/**
 * Upgrade para SGDK 2.11
 * ----------------------
 * - Atualizar pasta do SGDK
 * - Abrir C:\sgdk211\sample\basics\hello-world\src
 *   > Copiar pasta "boot" para "src" deste projeto
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
// #include "resources.h"

#include "hud.h"
#include "globals.h"
#include "engine/gameobject.h"
#include "engine/background.h"
#include "engine/utils.h"
#include "engine/level.h"
#include "entities/player.h"
#include "entities/enemy.h"

// index for tiles in VRAM (first tile reserved for SGDK)
// u16 ind = 1;
u16 ind = TILE_USER_INDEX;

// glow color effect
u8 bg_colors_delay = 5;
const u16 bg_color_glow[] = {0x0, 0x222, 0x444, 0x666, 0x888};

// enemies room table
typedef struct {
	u16 first;
	u16 last;
} RoomEnemies;

RoomEnemies enemies_table[NUMBER_OF_ROOMS];

// enemies pool
#define MAX_OBJ 30
GameObject enemy_pool[MAX_OBJ];
u16 enemy_tiles_ind;

////////////////////////////////////////////////////////////////////////////
// GAME INIT

// void init_objects() {
// 	for (u16 i = 0; i < LEN(level1_objects); ++i) {
// 		MapObject* p = (MapObject*) level1_objects[i];
// 		kprintf("room: %d, pos %ld %ld, spd: %d %d", p->room, F32_toInt(p->x), F32_toInt(p->y), F16_toInt(p->speed_x), F16_toInt(p->speed_y));
// 	}
// }

/**
 * Build enemy lookup table for indexed access when 
 * spawning enemies, when the player enters a new room.
 */
void init_enemy_data_from_map() {
	// get first and last index of enemies in each room
	u8 room = -1;
	u8 eny = -1;
	u16 idx = 0;
	MapObject* obj;
	for (; idx < LEN(level1_objects); ++idx) {
		obj = (MapObject*) level1_objects[idx];
		#ifdef DEBUG_OBJ
		text_add_int(obj->room);
		#endif
		
		if (obj->room != room) {		 // reached new room
			room = obj->room;
			++eny;
			enemies_table[eny].first = idx; // store the first enemy in room
			
			if (eny > 0) {				 // store the last enemy in previous room
				enemies_table[eny-1].last = idx-1;
			}
			
		}
		// room: 1
		// level1_objects:  00,00,00,01,01,01,01,
		//                  0  1  2  3  4  5  6
		//                                    idx
		// room_enemies = { {0,2}, {3,?}, ...
		//                          eny
	}
	enemies_table[eny].last = idx-1;

	#ifdef DEBUG_OBJ
	KLog("All enemy rooms from map:");
	text_print_and_clear();
	#endif

	#ifdef DEBUG_OBJ
	KLog("Enemy lookup table:");
	for (u8 i = 0; i < LEN(enemies_table); ++i) {
		text_add_int(enemies_table[i].first);
		text_add_int(enemies_table[i].last);
	}
	text_print_and_clear();
	#endif
}

void init_enemy_pool() {
	// prepare enemies pool
	for (u8 i = 0; i < LEN(enemy_pool); ++i) {
		enemy_pool[i].active = FALSE;
	}

	// load enemy tiles
	enemy_tiles_ind = ind;
	ind += ENEMY_load_tiles(ind);
	
	// GameObject* ball = enemy_pool;
	// for (u8 i = 0; i < MAX_OBJ; ++i, ++ball) {
	// 	ENEMY_init(ball, (SCREEN_W-8)/2, (SCREEN_H-8)/2, ball_ind);
	// }
}

/**
 * Returns the next available index from enemies_list or -1 if there is none.
 */
s16 get_enemy_available() {
	for (u8 i = 0; i < LEN(enemy_pool); ++i) {
		if (!enemy_pool[i].active)
			return i;
	}
	return -1;
}

//u16 screen_x, u16 screen_y
void spawn_enemies() {
	// spawn enemies in current room
	u8 room = LEVEL_current_room();

	#ifdef DEBUG_OBJ
	KLog("Enemies in room according to EnemyTable:");
	#endif

	for (u16 i = enemies_table[room].first; i <= enemies_table[room].last; ++i) {
// 		MapObject* p = (MapObject*) level1_objects[i];

		MapObject* mapobj = (MapObject*)level1_objects[i];
		#ifdef DEBUG_OBJ
		kprintf("room: %d, pos %ld %ld, spd: %d %d", 
			mapobj->room, F32_toInt(mapobj->x), F32_toInt(mapobj->y), 
			F16_toInt(mapobj->speed_x), F16_toInt(mapobj->speed_y));
		#endif
		
		s16 available = get_enemy_available();
		if (available == -1) return;

		GameObject* enemy = &enemy_pool[available];
		ENEMY_init(enemy, mapobj, enemy_tiles_ind);
	}
}

void clear_enemy_pool() {
	GameObject* ball = &enemy_pool[0];
	for (u8 i = 0; i < MAX_OBJ; ++i, ++ball) {
		if (ball->active) {
			SPR_releaseSprite(ball->sprite);
			ball->active = FALSE;
		}
	}
}

void game_init() {
	VDP_setScreenWidth320();
	SPR_init();
	
	// init BACKGROUND, LEVEL AND HUD ///////////////////////////////

	#ifdef DEBUG_LEVEL
	VDP_setTextPlane(BG_BACKGROUND);
	#else	
	ind += BACKGROUND_init(ind);
	#endif

	ind += LEVEL_init(ind);
	
	#ifdef DEBUG_LEVEL
	LEVEL_draw_map();
	#endif
	
	#ifndef DEBUG_LEVEL
	ind += HUD_init(ind);
	#endif
	
	// init GAME OBJECTS ////////////////////////////////////////////

	PLAYER_init(ind);

	init_enemy_data_from_map();
	init_enemy_pool();
	spawn_enemies(); // spawn enemies in first screen
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
	GameObject* ball = &enemy_pool[0];
	for (u8 i = 0; i < MAX_OBJ; ++i, ++ball) {
		if (ball->active) {
			ENEMY_update(ball);
		}
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
	if (LEVEL_update_camera(&player)) {
		clear_enemy_pool();
		spawn_enemies();
	}
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
	
	// print map indexes
	// LEVEL_print_tilemap_buff();

	SYS_doVBlankProcess();
	
	kprintf("Free RAM after Game Init: %d", MEM_getFree());

	while (true) {
		game_update();

		SPR_update();
		SYS_doVBlankProcess();
	}

	return 0;
}
