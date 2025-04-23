#include <genesis.h>
#include <maths.h>

#include "player.h"
#include "utils.h"

GameObject player;
u8 old_anim = 0;

////////////////////////////////////////////////////////////////////////////
// INITIALIZATION

u16 PLAYER_init(u16 ind) {
	ind += GAMEOBJECT_init(&player, &spr_ship, FIX16(SCREEN_W/2-8), FIX16(SCREEN_H/2-8), ind);
	// SPR_setAnim(player.sprite, 0);
	return ind;
}

////////////////////////////////////////////////////////////////////////////
// GAME LOOP/LOGIC

void PLAYER_get_input() {
	u16 value = JOY_readJoypad(JOY_1);

	if ((value & BUTTON_RIGHT) && (value & BUTTON_UP)) {
		player.speed_x =  FIX16(1.414);
		player.speed_y = -FIX16(1.414);
		player.anim = 1;
		return;
	}
	if ((value & BUTTON_RIGHT) && (value & BUTTON_DOWN)) {
		player.speed_x =  FIX16(1.414);
		player.speed_y =  FIX16(1.414);
		player.anim = 7;
		return;
	}
	if ((value & BUTTON_LEFT) && (value & BUTTON_UP)) {
		player.speed_x = -FIX16(1.414);
		player.speed_y = -FIX16(1.414);
		player.anim = 3;
		return;
	}
	if ((value & BUTTON_LEFT) && (value & BUTTON_DOWN)) {
		player.speed_x = -FIX16(1.414);
		player.speed_y =  FIX16(1.414);
		player.anim = 5;
		return;
	}

	if (value & BUTTON_RIGHT) {
		player.speed_x = PLAYER_SPEED;
		player.anim = 0;
	}
	else 
	if (value & BUTTON_LEFT) {
		player.speed_x = -PLAYER_SPEED;
		player.anim = 4;
	}
	else {
		player.speed_x = 0;
	}

	if (value & BUTTON_UP) {
		player.speed_y = -PLAYER_SPEED;
		player.anim = 2;
	}
	else
	if (value & BUTTON_DOWN) {
		player.speed_y = PLAYER_SPEED;
		player.anim = 6;
	} 
	else {
		player.speed_y = 0;
	}

}

void PLAYER_update() {
	// input
	PLAYER_get_input();
	
	// movement
	player.x = player.x + player.speed_x;
	player.y = player.y + player.speed_y;
	
	// wrap at screen bounds
	GAMEOBJECT_wrap_screen(&player);
	// GAMEOBJECT_clamp_screen(&player);
	
	// update VDP/SGDK
	SPR_setPosition(player.sprite, fix16ToInt(player.x), fix16ToInt(player.y));
	SPR_setAnim(player.sprite, player.anim);
}