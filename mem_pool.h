#ifndef MEM_POOL_H
#define MEM_POOL_H

#include <stddef.h>

struct mem_pool_node {
	struct mem_pool_node *next;
};

struct mem_pool {
	unsigned char *buf;
	size_t buf_size;
	size_t block_size;
	size_t num_blocks;
	size_t num_used_blocks;
	struct mem_pool_node *free_list;
};

void mem_pool_init(struct mem_pool *pool, void *buf, size_t buf_size,
	size_t block_size);
void mem_pool_finish(struct mem_pool *pool);
void *mem_pool_alloc(struct mem_pool *pool);
void mem_pool_free(struct mem_pool *pool, void *ptr);
size_t mem_pool_num_unused_blocks(struct mem_pool *pool);

#endif
