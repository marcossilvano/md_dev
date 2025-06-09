#include "objects_pool.h"
#include "engine/utils.h"

/////////////////////////////////////////////////////////////////////
// LINKED LIST OP

static void LINKEDLIST_add(PoolableObj** list, PoolableObj* obj) {
    obj->next = *list;
    *list = obj;
}

static PoolableObj* LINKEDLIST_remove(PoolableObj** list) {
    if (*list) {                   // if not empty
        PoolableObj* pobj = *list; // first list element
        *list = pobj->next;
        return pobj;
    }
    return NULL;
}

static void LINKEDLIST_print(PoolableObj* list) {
    kprintf("LINKED LIST: ");
    while (list) {
        text_add_int(list->obj->dir);
        list = list->next;
    }
    text_print_and_clear();
}

/////////////////////////////////////////////////////////////////////
// OBJECTS POOL OP

void print_array(PoolableObj* array, u8 n) {
    KLog("POOLABLE OBJ ARRAY:");
    for (u8 i = 0; i < n; ++i) {
        text_add_int(array[i].obj->dir);
    }
    text_print_and_clear();
}

void OBJPOOL_init(ObjectsPool* pool, PoolableObj* array, u8 n) {
	// // set all objects as inactive
	// for (u8 i = 0; i < n; ++i) {
	// 	pool_array->obj->active = FALSE;
	// }
    print_array(array, n);

    pool->active = NULL;    // empty list
    pool->free = array;     // first array object

    // init array's linked list
    for (u8 i = 0; i < n-1; ++i) {
        array->obj->dir = i;    // ID for testing
        array->next = (array+1);
        array++;
    }
    array->obj->dir = n-1;      // ID for testing
    array->next = NULL;

    // active -> 
    // free -> [0] -> [1] -> [2] -> [3] -> [4]

    // active -> [0]
    // free -> [1] -> [2] -> [3] -> [4]

    print_array(pool->free, n);

    LINKEDLIST_print(pool->free);
    LINKEDLIST_print(pool->active);
}

/**
 * Returns the next available from pool or NULL if there is none.
 */
GameObject* OBJPOOL_get_available(ObjectsPool* pool) {
	// for (u8 i = 0; i < n; ++i) {
	// 	if (!pool[i].active)
	// 		return &pool[i];
	// }
	// return NULL;

    // if (pool->free) {
    //     PoolableObj* pobj = LINKEDLIST_remove(&pool->free);
    //     LINKEDLIST_add(&pool->active, pobj);
    //     return pobj->obj;
    // }

    return NULL;
}