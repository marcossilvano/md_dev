#ifndef _ENEMY_H_
#define _ENEMY_H_

#include "resources.h"
#include "engine/gameobject.h"

#define ENEMY_BOUNCER 0
#define ENEMY_WARPER  1

typedef struct {
	u16 room;
    f32 x;
	f32 y;
    f16 speed_x;
    f16 speed_y;
    u8 type;
} MapObject;

#define BALL_MAX_SPEED 3

////////////////////////////////////////////////////////////////////////////
// INITIALIZATION

u16 ENEMY_load_tiles(u16 ind);
void ENEMY_init(GameObject* const ball, const MapObject* const mapobj, u16 ind);

////////////////////////////////////////////////////////////////////////////
// GAME LOOP/LOGIC

void ENEMY_bouncer_update(GameObject* obj);
void ENEMY_warper_update(GameObject* obj);

////////////////////////////////////////////////////////////////////////////
// PRIVATE MEMBERS



#endif