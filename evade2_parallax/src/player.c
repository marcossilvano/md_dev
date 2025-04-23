#include <genesis.h>
#include <maths.h>

#include "player.h"
#include "utils.h"

GameObject player;
u8 move_type = 0;

////////////////////////////////////////////////////////////////////////////
// INITIALIZATION

u16 PLAYER_init(u16 ind) {
	ind += GAMEOBJECT_init(&player, &spr_ship, FIX16(SCREEN_W/2-12), FIX16(SCREEN_H/2-12), ind);
	// SPR_setAnim(player.sprite, 0);
	return ind;
}

////////////////////////////////////////////////////////////////////////////
// GAME LOOP/LOGIC

/**
 * Get player input and set ship speed with:
 * - Constant speed
 * - Always moving (doesn't stop when dpad released)
 * - Four directions, mutually exclusive
 */
void PLAYER_get_input_dir4() {
	if (key_down(JOY_1, BUTTON_RIGHT)) {
		player.speed_x = PLAYER_SPEED;
		player.speed_y = 0;
		player.anim = 0;
	}
	else 
	if (key_down(JOY_1, BUTTON_LEFT)) {
		player.speed_x = -PLAYER_SPEED;
		player.speed_y = 0;
		player.anim = 4;
	}
	else
	if (key_down(JOY_1, BUTTON_UP)) {
		player.speed_x = 0;
		player.speed_y = -PLAYER_SPEED;
		player.anim = 2;
	}
	else
	if (key_down(JOY_1, BUTTON_DOWN)) {
		player.speed_x = 0;
		player.speed_y = PLAYER_SPEED;
		player.anim = 6;
	} 
}

/**
 * Get player input and set ship speed with:
 * - Constant speed
 * - Stop when dpad released
 * - Eight directions + fix for diagonals
 */
void PLAYER_get_input_dir8() {
	player.speed_x = 0;
	player.speed_y = 0;

	/* ANIM DIRECTIONS
		      2
		   3     1
		4           0
		   5     7
		      6
	*/

	if (key_down(JOY_1, BUTTON_RIGHT)) {
		player.speed_x = PLAYER_SPEED;
		player.anim = 0;

		if (key_down(JOY_1, BUTTON_UP)) {
			player.speed_x =  PLAYER_SPEED45;
			player.speed_y = -PLAYER_SPEED45;
			player.anim = 1;
			return;
		} 
		else
		if (key_down(JOY_1, BUTTON_DOWN)) {
			player.speed_x =  PLAYER_SPEED45;
			player.speed_y =  PLAYER_SPEED45;
			player.anim = 7;
			return;
		}
	} 
	else 
	if (key_down(JOY_1, BUTTON_LEFT)) {
		player.speed_x = -PLAYER_SPEED;
		player.anim = 4;

		if (key_down(JOY_1, BUTTON_UP)) {
			player.speed_x = -PLAYER_SPEED45;
			player.speed_y = -PLAYER_SPEED45;
			player.anim = 3;
			return;
		}
		else
		if (key_down(JOY_1, BUTTON_DOWN)) {
			player.speed_x = -PLAYER_SPEED45;
			player.speed_y =  PLAYER_SPEED45;
			player.anim = 5;
			return;
		}
	}

	if (key_down(JOY_1, BUTTON_UP)) {
		player.speed_y = -PLAYER_SPEED;
		player.anim = 2;
	}
	else
	if (key_down(JOY_1, BUTTON_DOWN)) {
		player.speed_y = PLAYER_SPEED;
		player.anim = 6;
	} 
}

// void PLAYER_get_input_dir8_2() {
// 	Vect2D_s16 input;
// 	input.x = key_down(JOY_1, BUTTON_RIGHT) - key_down(JOY_1, BUTTON_LEFT);
// 	input.y = key_down(JOY_1, BUTTON_DOWN)  - key_down(JOY_1, BUTTON_UP);

// 	// define player x,y speed by scaling the direction vector
// 	player.speed_x += fix16Mul(  cosFix16(player.dir), player.speed );
// 	player.speed_y += fix16Mul( -sinFix16(player.dir), player.speed );

// 	// frame?
// }

/**
 * Get player input and set ship speed with:
 * - Non constant speed + acceleration
 * - Slows down until stop, when dpad is released
 * - Left/Right = Rotate
 * - Up = Accelerate
 */
void PLAYER_get_input_turn_move() {
	
	// rotate ship with delay while holding the key
	if (key_down(JOY_1, BUTTON_RIGHT)) {
		player.dir -= 16;
		if (player.dir < 0)
			player.dir += 1024;
	} 
	else
	if (key_down(JOY_1, BUTTON_LEFT)) {
		player.dir += 16;
		if (player.dir > 1023)
			player.dir -= 1024;
	}
					   
	// rotate the ship instantaneously when key is pressed or released
	if (key_released(JOY_1, BUTTON_RIGHT)) {
		// truncate to a multiple of 128
		player.dir = player.dir / 128 * 128; 
	} else
	if (key_released(JOY_1, BUTTON_LEFT)) {
	 	// round to a multiple of 128, but don't let it go to 1024 or higher, since there is no 8th frame (1024/128 = 8)
	 	player.dir = (player.dir + 64)/ 128 * 128;
		if (player.dir >= 1024)
			player.dir = 0;
	}
	
	// accelerate ship
	if (key_down(JOY_1, BUTTON_UP)) {
		player.speed = FIX16(0.05);
	} else {
		player.speed = 0;
	}

	// define player x,y speed by scaling the direction vector
	player.speed_x += fix16Mul(  cosFix16(player.dir), player.speed );
	player.speed_y += fix16Mul( -sinFix16(player.dir), player.speed );
	
	PLAYER_limit_speed();

	/* ANIM DIRECTIONS
		      2
		   3     1
		4           0
		   5     7
		      6
	*/
	// 1024 angles / 8 frames -> each frame = 128 units
	player.anim = player.dir / 128; 
}

void PLAYER_limit_speed() {
	// limit max speed
	f16 length = FIX16(getApproximatedDistance(fix16ToInt(player.speed_x), fix16ToInt(player.speed_y)));
	if (length > PLAYER_SPEED) {
			// normalize speed vector
			player.speed_x = fix16Mul(fix16Div(player.speed_x, length), PLAYER_SPEED);
			player.speed_y = fix16Mul(fix16Div(player.speed_y, length), PLAYER_SPEED);
		}
}

// void PLAYER_limit_speed() {
// 	// limit max speed
// 	switch (player.anim) {
// 		case 0: player.speed_x = min(player.speed_x,  PLAYER_SPEED);   break;
		
// 		case 1: player.speed_x = min(player.speed_x,  PLAYER_SPEED45);
// 				player.speed_y = max(player.speed_y, -PLAYER_SPEED45); break;
		
// 		case 2:	player.speed_y = max(player.speed_y, -PLAYER_SPEED);   break;
		
// 		case 3:	player.speed_y = max(player.speed_y, -PLAYER_SPEED45);
// 				player.speed_x = max(player.speed_x, -PLAYER_SPEED45); break;
		
// 		case 4: player.speed_x = max(player.speed_x, -PLAYER_SPEED);   break;
		
// 		case 5: player.speed_x = max(player.speed_x, -PLAYER_SPEED45);
// 				player.speed_y = min(player.speed_y,  PLAYER_SPEED45); break;
		
// 		case 6:	player.speed_y = min(player.speed_y,  PLAYER_SPEED);   break;
		
// 		case 7: player.speed_x = min(player.speed_x,  PLAYER_SPEED45);
// 				player.speed_y = min(player.speed_y,  PLAYER_SPEED45); break;
// 	}		
// }
	
void PLAYER_update() {	
	// get input
	PLAYER_get_input_dir4();
		
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