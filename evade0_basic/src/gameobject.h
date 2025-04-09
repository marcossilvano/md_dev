#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include <genesis.h>
#include "sprite_eng.h"
#include "globals.h"

typedef struct GameObject {
	Sprite* sprite;
	s16 x;
	s16 y;
	s16 screen_x;
	s16 screen_y;
	s16 right;
	s16 bottom;
	f16 speed_x;
	f16 speed_y;
	u8 w;
	u8 h;
	u8 flip;
	u8 anim;
} GameObject;

////////////////////////////////////////////////////////////////////////////
// INITIALIZATION

u16 GAMEOBJECT_init(GameObject* const obj, const SpriteDefinition* const sprite, s16 x, s16 y);

////////////////////////////////////////////////////////////////////////////
// GAME LOOP/LOGIC

void GAMEOBJECT_update_box(GameObject* obj);
void GAMEOBJECT_clamp_screen(GameObject* obj);
void GAMEOBJECT_wrap_screen(GameObject* obj);
void GAMEOBJECT_bounce_off_screen(GameObject* obj);

#endif // _STRUCTS_H_