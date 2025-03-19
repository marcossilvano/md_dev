#include "player.h"
#include "maths.h"

GameObject player;

bool on_floor = FALSE;

////////////////////////////////////////////////////////////////////////////
// INITIALIZATION

void PLAYER_init(u16* ind) {
	player.x = 5*16;
	player.y = 17*16;
	player.speed_x = 0;
	player.speed_y = 0;
	player.flip = FALSE;
	PAL_setPalette(PAL_PLAYER, spr_boy.palette->data, CPU);
	
	/*
	//		megaman[i].sprite = SPR_addSprite(&spr_megaman, megaman[i].x, megaman[i].y, TILE_ATTR(PAL2, priority, FALSE, FALSE));	
	//        // disable auto tile upload for enemies sprites as we will pre-load all animation frams in VRAM for them
	
	megaman[i].sprite = SPR_addSpriteEx(&spr_megaman, megaman[i].x, megaman[i].y, 
	TILE_ATTR_FULL(PAL2, priority, FALSE, FALSE, ind), -1, // sprite table index (ignored) 
	SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC);
	//SPR_FLAG_AUTO_SPRITE_ALLOC);
	
	*/

// player.sprite = SPR_addSprite(&spr_boy, player.x, player.y, TILE_ATTR(PAL_PLAYER, FALSE, FALSE, FALSE));
// SPR_setAutoTileUpload(player.sprite, FALSE);
player.sprite = SPR_addSprite(&spr_boy, player.x, player.y, TILE_ATTR_FULL(PAL_PLAYER, FALSE, FALSE, player.flip, 1));
player.w = player.sprite->definition->w;
player.h = player.sprite->definition->h;
*ind += player.sprite->definition->maxNumTile;
}

////////////////////////////////////////////////////////////////////////////
// GAME LOOP/LOGIC

void PLAYER_update_pos(GameObject* obj) {
	obj->right  = obj->x + obj->w;
	obj->bottom = obj->y + obj->h;
}

void PLAYER_check_floor(GameObject* obj) {
	on_floor = FALSE;
	if (LEVEL_wall_at(obj->x + 8, obj->bottom) || LEVEL_wall_at(obj->right - 8, obj->bottom)) {
		s16 last_y = obj->bottom - fix16ToInt(obj->speed_y);
		s16 tile_y = ((obj->bottom >> 4) << 4) +8;

		// if (obj->speed_y < 0) {
		// 	KLog_U1("y: ", obj->bottom);
		// 	KLog_U1("last_y: ", last_y);
		// 	KLog_U1("tile_y: ", tile_y);
		// }

		if (obj->speed_y == 0 || (obj->speed_y > 0 && last_y < tile_y)) {
			obj->y = obj->y - (obj->bottom % TILE_W);
			on_floor = TRUE;
		}
	}
}

bool PLAYER_on_floor(GameObject* obj) {
	if (obj->y >= FLOOR_Y) {
		obj->y = FLOOR_Y;
		return TRUE;
	} else {
		return FALSE;
	}
}

void PLAYER_map_clamp(GameObject* obj) {
	// horizontal
	if (obj->x < 0) {
		obj->x = 0;	
	} else 
	if (obj->x > MAP_W - obj->w) {
		obj->x = MAP_W - obj->w;
	}

	if (obj->y < 0) {
		obj->y = 0;	
	} else 
	if (obj->y > MAP_H - obj->h) {
		obj->y = MAP_H - obj->h;
	}
}

void PLAYER_wrap_bounds(GameObject* obj) {
	// horizontal
	if (obj->x < -(obj->w >> 1)) {
		obj->x = SCREEN_W - (obj->w >> 1);	
	}
	else if (obj->x > SCREEN_W - (obj->w >> 1)) {
		obj->x = -(obj->w >> 1);
	}
}

void PLAYER_animate() {
	// animation
	if (!on_floor) {
		if (player.speed_y < 0)
			player.anim = ANIM_JUMP;
		else 
		if (player.speed_y > 0)
			player.anim = ANIM_FALL;
	}
	else {
		if (player.speed_x)
			player.anim = ANIM_RUN;
		else
			player.anim = ANIM_STAND;
	}

	//sonic.anim = BLOCK_BLUE;
}

void PLAYER_get_input() {
	u16 value = JOY_readJoypad(JOY_1);

	if (value & BUTTON_LEFT) {
		player.speed_x = FIX16(-PLAYER_SPEED);
		player.flip = TRUE;
	}
	else
	if (value & BUTTON_RIGHT) {
		player.speed_x = FIX16(PLAYER_SPEED);
		player.flip = FALSE;
	} 
	else {
		player.speed_x = 0;
	}
/*
	if (value & BUTTON_UP) {
		sonic.speed_y = FIX16(-1);
		sonic.flip = TRUE;
	}
	else
	if (value & BUTTON_DOWN) {
		sonic.speed_y = FIX16(1);
		sonic.flip = FALSE;
	} 
	else {
		sonic.speed_y = 0;
	}
*/
	if (value & BUTTON_A && on_floor)
		player.speed_y = FIX16(-5);
}

void PLAYER_update()
{
	if (LEVEL_check_wall(&player)) {
		player.anim = BLOCK_RED;
	} else {
		player.anim = BLOCK_BLUE;
	}

	PLAYER_check_floor(&player);
	if (on_floor) {
		player.speed_y = 0;
	} else 
	{
		// gravity
		player.speed_y += FIX16(0.3); 

		// limit falling speed
		if (player.speed_y > FIX16(5)) 
			player.speed_y = FIX16(5);
	}

	// input
	PLAYER_get_input();

	// movement
	player.x = player.x + fix16ToInt(player.speed_x);
	player.y = player.y + fix16ToInt(player.speed_y);
	PLAYER_update_pos(&player);

	// wrap at screen bounds
	//PLAYER_wrap_bounds(&player);
	PLAYER_map_clamp(&player);

	// animate
	PLAYER_animate();

	// update VDP/SGDK
	//SPR_setPosition(player.sprite, player.x, player.y);
	SPR_setHFlip(player.sprite, player.flip);
	SPR_setAnim(player.sprite, player.anim);
}

void PLAYER_update_nocollision()
{
	// input
	PLAYER_get_input();

	// movement
	player.x = player.x + fix16ToInt(player.speed_x);
	player.y = player.y + fix16ToInt(player.speed_y);
	PLAYER_update_pos(&player);

	// animate
	PLAYER_animate();

	PLAYER_map_clamp(&player);

	// update VDP/SGDK
	//SPR_setPosition(player.sprite, player.x, player.y);
	SPR_setHFlip(player.sprite, player.flip);
	SPR_setAnim(player.sprite, player.anim);
}