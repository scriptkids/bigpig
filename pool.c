#include "bp.h"

void *create_pool(int size) 
{
    struct pool_node *pool;

    pool = (struct pool_node*)malloc(sizeof(struct pool_node));
    if (NULL == pool) {
        return NULL;
    }

    pool->last = malloc(size);

    if (NULL == pool->last) {
        return NULL;
    }

    pool->end  = pool->last + size;
    pool->size = size;
    pool->next = NULL;
    NOTICE("create a pool size is %d\n", size);
    return pool;
}

void *get_more_memory(struct pool_node *pool, int size)
{
    void *ptr;
    struct pool_node *new_pool;
 
    new_pool = create_pool(pool->size);
    if (NULL == new_pool) {
        return NULL;
    }
    ptr = new_pool->last;
    new_pool->last += size;
    pool->next = new_pool;
    NOTICE("get more memory");
    return ptr;
    /* 
    new_pool = (struct pool_node*)malloc(sizeof(struct pool_node));
    if (NULL == new_pool) {
        return NULL;
    }
    new_pool->last = malloc(pool_size);
    if (NULL == new_pool->last) {
        return NULL;
    }
    new_pool->end = new_pool->last + pool->size;
    new_pool->size = pool->size;
*/

}

void *get_memory(struct pool_node *pool, int size)
{
    void *ptr;
    struct pool_node *p;
    int free_size;

    if (size > pool->size) {
        NOTICE("request memory is larger than pool size\n");
        return NULL;
    }

    p = pool;
    while (1) {
        free_size = p->end - p->last;
        if (free_size >= size) {
            ptr = p->last;
            p->last += size;
            return ptr;
        }
        if (NULL == p->next)
            break;

        p = p->next;
    }
    ptr = get_more_memory(p, size);
    return ptr;
}


void destory_pool(struct pool_node *pool)
{
    struct pool_node *p, *q;
    
    p = pool;
    while (p) {
       
        free(p->end - p->size);
        p = p->next;
    }
    p = pool;
    while (p) {
        q = p->next;
        free(p);
        p = q; 
    }
    NOTICE("Destory the mem_pool");
}
