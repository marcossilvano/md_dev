#include <genesis.h>
#include "enemy.h"
#include "engine/level.h"

static u16** ball_indexes;

////////////////////////////////////////////////////////////////////////////
// PRIVATE PROTOTYPES

static void frame_changed(Sprite* sprite);

////////////////////////////////////////////////////////////////////////////
// INITIALIZATION

u16 ENEMY_load_tiles(u16 ind) {
	u16 num_tiles;
	ball_indexes = SPR_loadAllFrames(&spr_ball, ind, &num_tiles);
	
    return num_tiles;
}

void ENEMY_init(GameObject* const obj, const MapObject* const mapobj, u16 ind) {
    // convert map coord to screen coord (and fix 1 tile offset y from Tiled)
	f32 x = F32_toInt(mapobj->x) % SCREEN_W;
	f32 y = F32_toInt(mapobj->y) % SCREEN_H - 16;

    GAMEOBJECT_init(obj, &spr_ball, x, y, -4, -4, PAL_ENEMY, ind);
    obj->speed_x = mapobj->speed_x;
    obj->speed_y = mapobj->speed_y;

    SPR_setAutoTileUpload(obj->sprite, FALSE);
    SPR_setFrameChangeCallback(obj->sprite, &frame_changed);
    
    SPR_setAnim(obj->sprite, 2);
    SPR_setAnimationLoop(obj->sprite, FALSE);
}

////////////////////////////////////////////////////////////////////////////
// GAME LOOP/LOGIC

void ENEMY_update1(GameObject* obj);
void ENEMY_update2(GameObject* obj);

void ENEMY_update(GameObject* obj) {
    ENEMY_update1(obj);
}

void ENEMY_update1(GameObject* obj) {
    obj->x += obj->speed_x;
    obj->y += obj->speed_y;

    GAMEOBJECT_update_boundbox(obj->x, obj->y, obj);

    if (obj->speed_x > 0) {				// moving right
		if (LEVEL_wallXY(obj->box.right, obj->box.top + obj->h/2)) {
            obj->speed_x = -obj->speed_x;
        }
    } 
    else if (obj->speed_x < 0) {		// moving left
        if (LEVEL_wallXY(obj->box.left, obj->box.top + obj->h/2)) {
                obj->speed_x = -obj->speed_x;
        }
    }
        
    if (obj->speed_y < 0) {             // moving up
        if (LEVEL_wallXY(obj->box.left + obj->w/2, obj->box.top)) {
            obj->speed_y = -obj->speed_y;
        }
    }
    else if (obj->speed_y > 0) {        // moving down
        if (LEVEL_wallXY(obj->box.left + obj->w/2, obj->box.bottom)) {
            obj->speed_y = -obj->speed_y;
        }
    }

    GAMEOBJECT_bounce_off_screen(obj);
    SPR_setPosition(obj->sprite, obj->box.left, obj->box.top);
}

void ENEMY_update2(GameObject* obj) {
    obj->next_x = obj->x + obj->speed_x;
    obj->next_y = obj->y + obj->speed_y;

    LEVEL_move_and_slide(obj);

    obj->x = obj->next_x;
    obj->y = obj->next_y;

    if (LEVEL_collision_result() & COLLISION_BOTTOM || LEVEL_collision_result() & COLLISION_TOP) {
        obj->speed_y = -obj->speed_y;
    }
    if (LEVEL_collision_result() & COLLISION_LEFT || LEVEL_collision_result() & COLLISION_RIGHT) {
        obj->speed_x = -obj->speed_x;
    }

    GAMEOBJECT_update_boundbox(obj->x, obj->y, obj);
    GAMEOBJECT_bounce_off_screen(obj);
    SPR_setPosition(obj->sprite, obj->box.left, obj->box.top);
}

////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS

static void frame_changed(Sprite* sprite) {
    // get VRAM tile index for this animation of this sprite
    u16 tileIndex = ball_indexes[sprite->animInd][sprite->frameInd];
	
    // manually set tile index for the current frame (preloaded in VRAM)
    SPR_setVRAMTileIndex(sprite, tileIndex);
}