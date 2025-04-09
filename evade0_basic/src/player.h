#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <genesis.h>
#include "gameobject.h"
#include "resources.h"

#define ANIM_VERTICAL    0
#define ANIM_HORIZONTAL  1

#define PLAYER_SPEED  2

extern GameObject player;

////////////////////////////////////////////////////////////////////////////
// INITIALIZATION

u16 PLAYER_init();

////////////////////////////////////////////////////////////////////////////
// GAME LOOP/LOGIC

void PLAYER_animate();
void PLAYER_get_input();
void PLAYER_update();

#endif // _PLAYER_H_