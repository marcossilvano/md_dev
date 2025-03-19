#include "structs.h"

int hscroll_offset = 0;
char text[5];

u16 GAMEOBJECT_init(GameObject* const obj, const SpriteDefinition* const sprite, s16 x, s16 y) {
	obj->x = x;
	obj->y = y;
	obj->speed_x = 0;
	obj->speed_y = 0;
	obj->flip = FALSE;
	PAL_setPalette(PAL_PLAYER, sprite->palette->data, DMA);
	
	obj->sprite = SPR_addSprite(sprite, obj->x, obj->y, TILE_ATTR_FULL(PAL_PLAYER, FALSE, FALSE, obj->flip, 1));
	obj->w = obj->sprite->definition->w;
	obj->h = obj->sprite->definition->h;
	
	return obj->sprite->definition->maxNumTile;
}