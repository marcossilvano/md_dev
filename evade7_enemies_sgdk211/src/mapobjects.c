// #include "mapobjects.h"
// #include "engine/utils.h"

// RoomEnemies enemies_table[NUMBER_OF_ROOMS];
// int curr_mapobj;

// ///////////////////////////////////////////////////////////////////////////
// // MAP OBJECTS EXTRACTION

// /**
//  * Build enemy lookup table for indexed access when 
//  * spawning enemies, when the player enters a new room.
//  * The lookup table stores the first and last level_objects indexes
//  * from each room.
//  */
// void MAPOBJ_init_mapobjects(const void* level_objects[], u16 n) {
// 	// set all rooms up with zero enemies
// 	for (u16 i = 0; i < LEN(enemies_table); i++) {
// 		enemies_table[i].first = 1;
// 		enemies_table[i].last  = 0;
// 	}

// 	// gets first and last index of enemies in each room
// 	u8 room = -1;
// 	u16 idx = 0;
// 	MapObject* obj;
// 	for (; idx < n; ++idx) {
// 		obj = (MapObject*)level_objects[idx];
// 		#ifdef DEBUG_OBJ
// 		text_add_int(obj->room);
// 		#endif
		
// 		if (obj->room != room) {		 			// reached new room
// 			if (idx > 0) {				 
// 				enemies_table[room].last = idx-1;	// store the last enemy in previous room
// 			}
// 			room = obj->room;
// 			enemies_table[room].first = idx; 		// store the first enemy in room
// 		}
// 	}
// 	enemies_table[room].last = idx-1;

// 	#ifdef DEBUG_OBJ
// 	KLog("All enemy rooms from map:");
// 	text_print_and_clear();
// 	#endif

// 	#ifdef DEBUG_OBJ
// 	KLog("Enemy lookup table:");
// 	for (u8 i = 0; i < LEN(enemies_table); ++i) {
// 		text_add_int(enemies_table[i].first);
// 		text_add_int(enemies_table[i].last);
// 	}
// 	text_print_and_clear();
// 	#endif
// }

// /**
//  * Inits the loop to get the MapObjects from a room.
//  * Returns the first MapObject in room.
//  * To loop through the remaining, use LEVEL_loop_next().
//  */
// MapObject* MAPOBJ_loop_init(void* level_objects[], u16 n, u8 room) {
//     curr_mapobj = enemies_table[room].first;

//     // empy rooms have crossed indexes
//     if (enemies_table[room].first > enemies_table[room].last) {
//         return NULL;
//     }
//     return (MapObject*)level_objects[curr_mapobj];
// }

// /**
//  * Loops through room's MapObjects.
//  * Returns the next MapObject in room.
//  * To init the loop, use LEVEL_loop_init().
//  */
// MapObject* MAPOBJ_loop_next(void* level_objects[], u16 n, u8 room) {
//     curr_mapobj++;
//     if (curr_mapobj > enemies_table[room].last) {
//         return NULL;
//     }
//     if (curr_mapobj >= n) {
//         return NULL;
//     }
//     return (MapObject*)level_objects[curr_mapobj];
// }
