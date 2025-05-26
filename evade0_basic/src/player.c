#include <genesis.h>
#include <maths.h>

#include "player.h"
#include "utils.h"

GameObject player;

bool on_floor = FALSE;

////////////////////////////////////////////////////////////////////////////
// INITIALIZATION

u16 PLAYER_init() {
	return GAMEOBJECT_init(&player, &spr_ship, SCREEN_W/2-8, SCREEN_H/2-8);
}

////////////////////////////////////////////////////////////////////////////
// GAME LOOP/LOGIC

void PLAYER_get_input() {
	u16 value = JOY_readJoypad(JOY_1);

	if (value & BUTTON_LEFT) {
		player.speed_x = FIX16(-PLAYER_SPEED);
	}
	else 
	if (value & BUTTON_RIGHT) {
		player.speed_x = FIX16(PLAYER_SPEED);
	} 
	else {
		player.speed_x = 0;
	}

	if (value & BUTTON_UP) {
		player.speed_y = FIX16(-PLAYER_SPEED);
	}
	else
	if (value & BUTTON_DOWN) {
		player.speed_y = FIX16(PLAYER_SPEED);
	} 
	else {
		player.speed_y = 0;
	}
}

void PLAYER_animate() {
	if (player.speed_x != 0) {
		player.anim = ANIM_HORIZONTAL;
		if (player.speed_x > 0) {
			SPR_setHFlip(player.sprite, 0);
		} else {
			SPR_setHFlip(player.sprite, 1);
		}	
	}
	
	if (player.speed_y != 0) {
		player.anim = ANIM_VERTICAL;
		if (player.speed_y < 0) {
			SPR_setVFlip(player.sprite, 0);
		} else {
			SPR_setVFlip(player.sprite, 1);
		}	
	}
	SPR_setAnim(player.sprite, player.anim);
}

void PLAYER_update() {
	// input
	PLAYER_get_input();

	// movement
	player.x = player.x + F16_toInt(player.speed_x);
	player.y = player.y + F16_toInt(player.speed_y);
	GAMEOBJECT_update_box(&player);

	// wrap at screen bounds
	GAMEOBJECT_wrap_screen(&player);
	// GAMEOBJECT_clamp_screen(&player);

	// animate
	PLAYER_animate();

	// update VDP/SGDK
	SPR_setPosition(player.sprite, player.x, player.y);
	SPR_setAnim(player.sprite, player.anim);
}