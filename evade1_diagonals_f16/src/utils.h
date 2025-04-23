#ifndef _UTILS_H_
#define _UTILS_H_

#include <genesis.h>
#include <sprite_eng.h>
#include "globals.h"

////////////////////////////////////////////////////////////////////////////
// LOGIC

// #define wrap(X, L, H) ((X < L)? H : ((X > H)? : L: X))

#define WRAP(X, L, H) \
	if (X < L) \
		X = H; \
	else if (X > H) \
		X = L;

// #define clamp(X, L, H)   (min(max((X), (L)), (H)))

inline Box UTILS_get_box(int x, int y, int w, int h) {
	return (Box){x, x+w, y, y+h};
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