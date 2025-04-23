#include "gameobject.h"
#include "utils.h"

////////////////////////////////////////////////////////////////////////////
// INITIALIZATION

u16 GAMEOBJECT_init(GameObject* const obj, const SpriteDefinition* const sprite, s16 x, s16 y, u16 ind) {
	obj->x = x;
	obj->y = y;
	obj->speed_x = 0;
	obj->speed_y = 0;
	obj->speed = 0;
	obj->anim = 0;
	obj->dir = 0;
	PAL_setPalette(PAL_PLAYER, sprite->palette->data, DMA);
	
	obj->sprite = SPR_addSprite(sprite, obj->x, obj->y, TILE_ATTR_FULL(PAL_PLAYER, FALSE, FALSE, 0, ind));
	obj->w = obj->sprite->definition->w;
	obj->h = obj->sprite->definition->h;
	
	return obj->sprite->definition->maxNumTile;
}

////////////////////////////////////////////////////////////////////////////
// GAME LOOP/LOGIC

/*
inline void update_rect(GameObject* obj) {
	obj->rect.left  = obj->x;
	obj->rect.right = obj->x + obj->sprite->definition->w;
	obj->rect.top   = obj->y;
	obj->rect.bottom= obj->y + obj->sprite->definition->h;
}
*/

void GAMEOBJECT_update_box(GameObject* obj) {
	obj->right  = obj->x + obj->w;
	obj->bottom = obj->y + obj->h;
}

void GAMEOBJECT_clamp_screen(GameObject* obj) {
	obj->x = clamp(obj->x, 0, FIX16(SCREEN_W));
	obj->y = clamp(obj->y, 0, FIX16(SCREEN_H));
}

void GAMEOBJECT_wrap_screen(GameObject* obj) {
	WRAP(obj->x, -fix16Div(obj->w, 2), FIX16(SCREEN_W) - obj->w/2)
	WRAP(obj->y, -fix16Div(obj->h, 2), FIX16(SCREEN_H) - obj->h/2)
}

void GAMEOBJECT_bounce_off_screen(GameObject* obj) {
	// bounce off screen bounds
	if (obj->x < 0 || (obj->x + obj->sprite->definition->w) > SCREEN_W) {
		obj->speed_x = -obj->speed_x;
	}	

	if (obj->y < 0 || (obj->y + obj->sprite->definition->h) > SCREEN_H) {
		obj->speed_y = -obj->speed_y;
	}	
}
