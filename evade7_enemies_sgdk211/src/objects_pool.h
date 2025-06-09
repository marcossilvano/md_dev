#ifndef _OBJECTS_POOL_
#define _OBJECTS_POOL_

#include <genesis.h>
#include "engine/gameobject.h"

typedef struct PoolableObj {
	GameObject* obj;
	struct PoolableObj* next;
} PoolableObj;

typedef struct {
    PoolableObj* active;	    // head of active objects (used) linked list
	PoolableObj* free;		// head of available objects linked list
} ObjectsPool;


void print_array(PoolableObj* array, u8 n);
void OBJPOOL_init(ObjectsPool* pool, PoolableObj* array, u8 n);
GameObject* OBJPOOL_get_available(ObjectsPool* pool);

#endif