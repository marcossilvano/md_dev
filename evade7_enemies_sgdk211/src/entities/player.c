#include <genesis.h>
#include <maths.h>

#include "player.h"
#include "engine/level.h"
#include "engine/utils.h"
#include "hud.h"

GameObject player;

////////////////////////////////////////////////////////////////////////////
// PRIVATE MEMBERS

static inline void PLAYER_get_input_platformer();

static inline void PLAYER_get_input_dir4();
static inline void PLAYER_get_input_dir8();

static inline void PLAYER_get_input_dir4_2();
static inline void PLAYER_get_input_dir8_2();

static inline bool on_ground();

////////////////////////////////////////////////////////////////////////////
// INIT

u16 PLAYER_init(u16 ind) {
	ind += GAMEOBJECT_init(&player, &spr_ship, SCREEN_W/2-12, SCREEN_H/2-12, -16, -16, PAL_PLAYER, ind);
	player.health = PLAYER_MAX_HEALTH;
	return ind;
}

////////////////////////////////////////////////////////////////////////////
// UPDATE

void PLAYER_update() {
	// input
	// PLAYER_get_input_dir4();
	// PLAYER_get_input_dir8_2();
	PLAYER_get_input_dir4_2();
	// PLAYER_get_input_platformer();
	
	// project next position
	player.next_x = player.x + player.speed_x;
	player.next_y = player.y + player.speed_y;

	// check and resolve walls
	LEVEL_move_and_slide(&player);
	
	// update current position
	player.x = player.next_x;
	player.y = player.next_y;
	
	// limit do map boundaries
	LEVEL_check_map_boundaries(&player);

	if (LEVEL_collision_result() & COLLISION_VERT) {
		player.speed_y = 0;
	}
	if (LEVEL_collision_result() & COLLISION_HORIZ) {
		player.speed_x = 0;
	}

	// item check
	GAMEOBJECT_update_boundbox(player.x, player.y, &player);
	if (LEVEL_tileXY(player.box.left + player.w/2, player.box.top + player.h/2) == IDX_ITEM) {
		HUD_gem_collected(1);
		LEVEL_remove_tileXY(player.box.left + player.w/2, player.box.top + player.h/2, 0);
	}

	// GAMEOBJECT_wrap_screen(&player);
	// GAMEOBJECT_clamp_screen(&player);
	
	// update VDP/SGDK
	GAMEOBJECT_set_hwsprite_position(&player);
	SPR_setAnim(player.sprite, player.anim);
}

void PLAYER_on_hit(u8 amount) {
	KLog("Player hit!");
}

////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS

/**
 * Implements a platformer controlled character.
 * Current implementation contains the following features:
 * - Variable height jump (by button hold)
 * - Head bump on level map (makes the character fall instantly)
 * - Level blocks breaking by head bumping them from bellow
 */
static inline bool on_ground() {
	return LEVEL_collision_result() & COLLISION_BOTTOM;
}

static inline void PLAYER_get_input_platformer() {
	if (key_down(JOY_1, BUTTON_RIGHT)) {
		player.speed_x = PLAYER_SPEED;
		player.anim = 0;
	}
	else 
	if (key_down(JOY_1, BUTTON_LEFT)) {
		player.speed_x = -PLAYER_SPEED;
		player.anim = 4;
	} else {
		player.speed_x = 0;
	}

	// if is on ground
	if (on_ground()) {
		player.speed_y = FIX16(1);
	} 
	else
	// if head stomped a block above
	if (LEVEL_collision_result() & COLLISION_TOP) {
		LEVEL_remove_tileXY(player.box.left + player.w/2 - 4, player.box.top - 8, 0);
		LEVEL_remove_tileXY(player.box.left + player.w/2 + 4, player.box.top - 8, 0);
		player.speed_y = 0;
	}
	
	// falls when jump button is released
	if (key_released(JOY_1, BUTTON_A)) {
		// if isn't on ground and is going up
		if (!on_ground()) {
			if (player.speed_y < 0 && player.speed_y < FIX16(-2.4)) {
				player.speed_y = FIX16(-2.4);
			}
		}
	}

	// jump
	if (key_pressed(JOY_1, BUTTON_A)) {
		if (on_ground()) {
			player.speed_y = FIX16(-4);
		}
	}

	// limit gravity down speed
	player.speed_y += FIX16(0.15);
	if (player.speed_y > FIX16(4)) {
		player.speed_y = FIX16(4);
	}

// 	if (player.speed_y < 0) 
// 		player.anim = 2;
// 	else if (player.speed_y > 0) 
// 		player.anim = 6;
}

/**
 * Get player input and set ship speed with:
 * - Constant speed
 * - Always moving (doesn't stop when dpad released)
 * - Four directions, mutually exclusive
 */
static inline void PLAYER_get_input_dir4() {
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

// static void limit_speed_vector() {
// 	// limit max speed
// 	u32 length = FIX16(getApproximatedDistance(F16_toInt(player.speed_x), F16_toInt(player.speed_y)));
// 	if (length > PLAYER_SPEED) {
// 			// normalize speed vector
// 			player.speed_x = F16_mul(F16_div(player.speed_x, length), PLAYER_SPEED);
// 			player.speed_y = F16_mul(F16_div(player.speed_y, length), PLAYER_SPEED);
// 		}
// }

static void PLAYER_get_input_dir4_2() {
	if (key_down(JOY_1, BUTTON_RIGHT)) {
		player.dir = 0;
	} else 
	if (key_down(JOY_1, BUTTON_LEFT)) {
		player.dir = 128 * 4;
	} else	
	if (key_down(JOY_1, BUTTON_UP)) {
		player.dir = 128 * 2;
	} else
	if (key_down(JOY_1, BUTTON_DOWN)) {
		player.dir = 128 * 6;
	} 

	if (key_any(JOY_1)) {
		// define player x,y speed by scaling the direction vector
		player.speed += PLAYER_ACCEL;
		player.speed = min(player.speed, PLAYER_SPEED);
	} else {
		player.speed -= PLAYER_ACCEL;
		player.speed = max(player.speed, 0);
	}
	player.speed_x = F16_mul(  cosFix16(player.dir), player.speed );
	player.speed_y = F16_mul( -sinFix16(player.dir), player.speed );

	// limit_speed_vector();

	player.anim = player.dir / 128; 
}

static void PLAYER_get_input_dir8_2() {
	if (key_down(JOY_1, BUTTON_RIGHT)) {
		if (key_down(JOY_1, BUTTON_UP)) {
			player.dir = 128 * 1;
			goto END;
		} 
		else if (key_down(JOY_1, BUTTON_DOWN)) {
			player.dir = 128 * 7;
			goto END;
		} else {
			player.dir = 0;
		}
	} 
	else 
	if (key_down(JOY_1, BUTTON_LEFT)) {
		if (key_down(JOY_1, BUTTON_UP)) {
			player.dir = 128 * 3;
			goto END;
		}
		else if (key_down(JOY_1, BUTTON_DOWN)) {
			player.dir = 128 * 5;
			goto END;
		} else {
			player.dir = 128 * 4;
		}
	}
	
	if (key_down(JOY_1, BUTTON_UP)) {
		player.dir = 128 * 2;
	}
	else
	if (key_down(JOY_1, BUTTON_DOWN)) {
		player.dir = 128 * 6;
	} 

	END:

	if (key_any(JOY_1)) {
		// define player x,y speed by scaling the direction vector
		player.speed += PLAYER_ACCEL;
		player.speed = min(player.speed, PLAYER_SPEED);
	} else {
		player.speed -= PLAYER_ACCEL;
		player.speed = max(player.speed, 0);
	}
	player.speed_x = F16_mul(  cosFix16(player.dir), player.speed );
	player.speed_y = F16_mul( -sinFix16(player.dir), player.speed );

	// limit_speed_vector();

	player.anim = player.dir / 128; 
}

/**
 * Get player input and set ship speed with:
 * - Constant speed
 * - Stop when dpad released
 * - Eight directions + fix for diagonals
 */
static inline void PLAYER_get_input_dir8() {
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
