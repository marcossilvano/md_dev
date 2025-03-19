#ifndef _STRUCTS_H_
#define _STRUCTS_H_

#include <genesis.h>
#include "sprite_eng.h"

#define TILE_W 16
#define SCREEN_W 320
#define SCREEN_H 224

#define MAP_TILES_W 120
#define MAP_TILES_H 50

#define MAP_W MAP_TILES_W * TILE_W
#define MAP_H MAP_TILES_H * TILE_W

#define PAL_PLAYER 		PAL0
#define PAL_MAP 		PAL1
#define PAL_BACKGROUND 	PAL2

extern int hscroll_offset;
extern char text[5];

typedef struct GameObject {
	Sprite* sprite;
	s16 x;
	s16 y;
	s16 screen_x;
	s16 screen_y;
	s16 w;
	s16 h;
	s16 right;
	s16 bottom;
	f16 speed_x;
	f16 speed_y;
	u8 flip;
	u8 anim;
} GameObject;

typedef struct Rect {
	u16 left;
	u16 right;
	u16 bottom;
	u16 top;
} Rect;

extern int hscroll_offset;
extern char text[5];

/*
inline void update_rect(GameObject* obj) {
	obj->rect.left  = obj->x;
	obj->rect.right = obj->x + obj->sprite->definition->w;
	obj->rect.top   = obj->y;
	obj->rect.bottom= obj->y + obj->sprite->definition->h;
}
*/
u16 GAMEOBJECT_init(GameObject* const obj, const SpriteDefinition* const sprite, s16 x, s16 y);

inline Rect get_rect(int x, int y, int w, int h) {
	return (Rect){x, x+w, y, y+h};
}

////////////////////////////////////////////////////////////////////////////
// DRAWING AND FX

inline void rotate_colors(u8 first_index, u8 last_index, s8 direction) {
	u16 last_color = PAL_getColor(first_index);
	for (u8 i = first_index; i != last_index; i += direction) {
		PAL_setColor(i, PAL_getColor(i + direction));
	}
	PAL_setColor(last_index, last_color);
}

inline void rotate_colors_left(u8 left_index, u8 right_index) {
	rotate_colors(left_index, right_index, 1);
}

inline void rotate_colors_right(u8 left_index, u8 right_index) {
	rotate_colors(right_index, left_index, -1);
}

#endif // _STRUCTS_H_