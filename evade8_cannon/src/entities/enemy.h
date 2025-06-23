#ifndef _ENEMY_H_
#define _ENEMY_H_

#include "resources.h"
#include "engine/gameobject.h"
#include "engine/mapobjects.h"

#define ENEMY_BOUNCER 0
#define ENEMY_WARPER  1
#define ENEMY_CANNON  2

#define BALL_MAX_SPEED 3

////////////////////////////////////////////////////////////////////////////
// INITIALIZATION

u16 ENEMY_load_tiles(u16 ind);
void ENEMY_init(GameObject* const ball, const MapObject* const mapobj, u16 ind);

////////////////////////////////////////////////////////////////////////////
// GAME LOOP/LOGIC

////////////////////////////////////////////////////////////////////////////
// PRIVATE MEMBERS



#endif