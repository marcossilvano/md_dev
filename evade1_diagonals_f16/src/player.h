#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <genesis.h>
#include "gameobject.h"
#include "resources.h"

#define ANIM_VERTICAL    0
#define ANIM_HORIZONTAL  1

#define PLAYER_SPEED   FIX16(2)
// #define PLAYER_SPEED45 FIX16(0.707 * PLAYER_SPEED)
#define PLAYER_SPEED45 FIX16(1.414)

extern GameObject player;

////////////////////////////////////////////////////////////////////////////
// INITIALIZATION

u16 PLAYER_init(u16 ind);

////////////////////////////////////////////////////////////////////////////
// GAME LOOP/LOGIC

void PLAYER_animate();
void PLAYER_get_input();
void PLAYER_update();

#endif // _PLAYER_H_