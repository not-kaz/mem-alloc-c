#ifndef MEM_POOL_H
#define MEM_POOL_H
#include <stddef.h>

struct mem_pool_list_node {
	struct mem_pool_list_node *next;
};

struct mem_pool {
	unsigned char *buf;
	size_t capacity;
	size_t chunk_size;
	size_t free_chunks;
	struct mem_pool_list_node *free_list_head;
};

void mem_pool_init(struct mem_pool *pool, void *buf, size_t buf_size,
	size_t chunk_size);
void *mem_pool_alloc(struct mem_pool *pool);
void mem_pool_reset(struct mem_pool *pool);
size_t mem_pool_calc_free_chunks(struct mem_pool *pool);

#endif
