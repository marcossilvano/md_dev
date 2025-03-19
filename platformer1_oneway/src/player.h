#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <genesis.h>
#include "structs.h"
#include "resources.h"
#include "level.h"

#define ANIM_STAND  0
#define ANIM_RUN    1
#define ANIM_JUMP   2
#define ANIM_FALL   2
#define BLOCK_BLUE	4
#define BLOCK_RED	5

#define FLOOR_Y       146
#define PLAYER_SPEED  2

extern GameObject player;

extern bool on_floor;

////////////////////////////////////////////////////////////////////////////
// INITIALIZATION

void PLAYER_init(u16* ind);

////////////////////////////////////////////////////////////////////////////
// GAME LOOP/LOGIC

void PLAYER_update_pos(GameObject* obj);
void PLAYER_check_floor(GameObject* obj);
bool PLAYER_on_floor(GameObject* obj);
void PLAYER_map_clamp(GameObject* obj);
void PLAYER_wrap_bounds(GameObject* obj);
void PLAYER_animate();
void PLAYER_get_input();
void PLAYER_update();
void PLAYER_update_nocollision();

#endif // _PLAYER_H_