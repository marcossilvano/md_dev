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
 * @todo melhorar colisao one-way
 * @todo Controle de DOWN, PRESSED e RELEASED para botoes
 * @todo Colisão com tiles sólidos (paredes)
 * @todo Fundo com parallax
 * @todo Adicionar obstaculos
 * @todo Adicionar inimigos
 * @todo verificar indice inicial de alocacao de sprites (?)
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
// u16 ind = TILE_USER_INDEX;
u8 color_delay = 5;

s16 map_scroll_x = 0;
s16 map_scroll_y = 0;
u16 bg_scroll = 0;

#define MAX_OBJ 1
GameObject objects[MAX_OBJ];

u16* spawn_map[15][7] = {
    { (u16[]){6, 1, 2, 3, 4, 5, 6}, (u16[]){4, 1, 2, 3, 4}, (u16[]){3, 1, 2, 3} },
    { (u16[]){6, 1, 2, 3, 4, 5, 6}, (u16[]){4, 1, 2, 3, 4}, (u16[]){3, 1, 2, 3} }
};


////////////////////////////////////////////////////////////////////////////
// DRAWING AND FX

inline void draw_position()  {
	intToStr(player.x, text, 6);
	VDP_drawText(text, 5, 3);

	intToStr(player.y, text, 6);
	VDP_drawText(text, 5, 4);
}

inline void draw_info() {
	VDP_drawText("PLATFORMER SAMPLE", 1, 1);
	VDP_drawText("Use DPAD and A", 1, 2);
	
	VDP_drawText("X: ", 2, 3);
	VDP_drawText("Y: ", 2, 4);
}

////////////////////////////////////////////////////////////////////////////
// GAME LOGIC

inline void CAMERA_follow(GameObject* obj) {
	map_scroll_x = obj->x - SCREEN_W/2 + obj->w/2;
	map_scroll_y = obj->y - SCREEN_H/2 + obj->h/2;
	
	map_scroll_x = clamp(map_scroll_x, 0, MAP_W - SCREEN_W);
	map_scroll_y = clamp(map_scroll_y, 0, MAP_H - SCREEN_H);
	
	MAP_scrollTo(map, map_scroll_x, map_scroll_y); 
	// VDP_setHorizontalScroll(BG_B, -map_scroll_x>>2);
	MAP_scrollTo(map_bg, map_scroll_x>>2, map_scroll_y>>2); 	
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

inline void game_update() {
	// PLAYER_update_nocollision();
	PLAYER_update();
	
	//CAMERA_move();
	CAMERA_follow(&player);
}

u16** robot_tile_indexes;

static void frame_changed(Sprite* sprite) {
	// get enemy index (stored in data field)
    // u16 enemyIndex = sprite->data;
	
    // get VRAM tile index for this animation of this sprite
    // u16 tileIndex = sprTileIndexes[enemyIndex][sprite->animInd][sprite->frameInd];
    u16 tileIndex = robot_tile_indexes[sprite->animInd][sprite->frameInd];
	
    // manually set tile index for the current frame (preloaded in VRAM)
    SPR_setVRAMTileIndex(sprite, tileIndex);
}

////////////////////////////////////////////////////////////////////////////
// GAME INIT

void game_init() {
	VDP_setScreenWidth320();
	
	SPR_init();
	ind += LEVEL_init(ind);

	PLAYER_init();
	
	u16 num_tiles;
	for (u8 i = 0; i < MAX_OBJ; ++i) {
		GAMEOBJECT_init(&objects[i], &spr_robot, (i+1)*24, 20*16);
		
		SPR_setAutoTileUpload(objects[i].sprite, FALSE);
		SPR_setFrameChangeCallback(objects[i].sprite, &frame_changed);

		robot_tile_indexes = SPR_loadAllFrames(&spr_robot, ind, &num_tiles);
		ind += num_tiles;

		SPR_setAnim(objects[i].sprite, 1);
	}
}

void hud_init() {
	// Initilizes text UI
	// VDP_setTextPalette(PAL_MAP);
	// VDP_setBackgroundColor(PAL_MAP*16 + 12);
	
	// VDP_setTextPlane(WINDOW);
	// VDP_setWindowHPos(FALSE, 0);
	// VDP_setWindowVPos(FALSE, 5);

	// draw_info();
	// LEVEL_draw_collision_map();
}

u8 tile_frame = 0;

int main(bool resetType)
{
	// Soft reset doesn't clear RAM. Can lead to bugs.
	if (!resetType) 
		SYS_hardReset();

	// VDP_setPlaneSize(64, 64, TRUE);
	game_init();
	hud_init();

	SYS_doVBlankProcess();

	while (1)
	{
		game_update();

		// draw_position();
		
		if (!--color_delay) {
			rotate_colors_left(PAL_MAP*16 + 5, PAL_MAP*16 + 7);
			rotate_colors_left(PAL_BACKGROUND*16 + 5, PAL_BACKGROUND*16 + 7);
			color_delay = 5;

			tile_frame = !tile_frame;

			if (tile_frame) {
				VDP_loadTileData( (const u32 *)animated_tiles2.tiles, 0x0F, 2, DMA_QUEUE);
				VDP_loadTileData( (const u32 *)animated_tiles2.tiles+32, 0x1B, 2, DMA_QUEUE);
			} else {
				VDP_loadTileData( (const u32 *)animated_tiles2.tiles+16, 0x0F, 2, DMA_QUEUE);
				VDP_loadTileData( (const u32 *)animated_tiles2.tiles+48, 0x1B, 2, DMA_QUEUE);
			}
		}
		
		for (u8 i = 0; i < MAX_OBJ; ++i) {
			SPR_setPosition(objects[i].sprite, objects[i].x-map_scroll_x, objects[i].y-map_scroll_y);
		}
		SPR_setPosition(player.sprite, player.x-map_scroll_x, player.y-map_scroll_y);

		SPR_update();

		SYS_doVBlankProcess();
		// update VDP scroll
	}

	return 0;
}
