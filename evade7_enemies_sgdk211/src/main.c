/**
 * Upgrade para SGDK 2.11
 * ----------------------
 * - Atualizar pasta do SGDK (ou colocar em nova pasta, ex: sgdk211)
 * - Se Windows: atualizar a variável de ambiente GDK (ex: c:\sgdk211)
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
 * [OK] Inimigos bolas que ricocheteiam
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
#include "engine/objects_pool.h"

// index for tiles in VRAM (first tile reserved for SGDK)
// u16 ind = 1;
u16 ind = TILE_USER_INDEX;

// glow color effect
u8 bg_colors_delay = 5;
const u16 bg_color_glow[] = {0x0, 0x222, 0x444, 0x666, 0x888};

// enemies pool
#define MAX_BALLS 30
GameObject enemy_array[MAX_BALLS];
u16 enemy_tiles_ind;
ObjectsPool enemy_pool;

////////////////////////////////////////////////////////////////////////////
// GAME INIT

void init_enemies() {
	// init_enemy_lookup_table(level1_objects, LEN(level1_objects));
	MAPOBJ_init_mapobjects(level1_objects, LEN(level1_objects));
	OBJPOOL_init(&enemy_pool, enemy_array, LEN(enemy_array));

	// load enemy tiles
	enemy_tiles_ind = ind;
	ind += ENEMY_load_tiles(ind);
}

void spawn_enemies() {
	// spawn enemies in current room
	u8 room = LEVEL_current_room();

	// Get Map Data Object from level_objects for the current room
	MapObject* mapobj = MAPOBJ_loop_init(level1_objects, LEN(level1_objects), room);
	while(mapobj) {
		GameObject* enemy = OBJPOOL_get_available(&enemy_pool);
		if (!enemy) return;

		// Enemy factory function: It gets the needed data from MapObject
		ENEMY_init(enemy, mapobj, enemy_tiles_ind);
		mapobj = MAPOBJ_loop_next(level1_objects, LEN(level1_objects), room);
	}

}

void clear_enemies() {
	OBJPOOL_clear(&enemy_pool);
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

	init_enemies();
	spawn_enemies(); // spawn enemies in first screen
}

////////////////////////////////////////////////////////////////////////////
// GAME LOGIC

static inline void color_effects() {
	--bg_colors_delay;
	if (bg_colors_delay == 0) {
		// rotate_colors_left(PAL_BACKGROUND*16, PAL_BACKGROUND*16+15);
		glow_color(PAL_BACKGROUND*16+8, bg_color_glow, 5);

		bg_colors_delay = 10;
	}
}

inline void update_enemies() {
	GameObject* obj = OBJPOOL_loop_init(&enemy_pool);
	while (obj) {
		obj->update(obj);
		GameObject* obj_to_release = NULL;

		if (GAMEOBJECT_check_collision(&player, obj)) {
			PLAYER_on_hit(1);
			//obj->on_hit(obj, 1);
			obj_to_release = obj;
		}
		
		obj = OBJPOOL_loop_next(&enemy_pool);
		if (obj_to_release) {
			OBJPOOL_release(&enemy_pool, obj_to_release);
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
		clear_enemies();
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
