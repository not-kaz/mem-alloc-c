#ifndef MEM_POOL_H
#define MEM_POOL_H

#include <stddef.h>

#define MEM_POOL_RESULT_OK 0
#define MEM_POOL_RESULT_INVALID_ARG 1
#define MEM_POOL_RESULT_BAD_ALIGNMENT 2

struct mem_pool_node {
	struct mem_pool_node *next;
};

struct mem_pool {
	unsigned char *buffer;
	size_t buffer_size;
	size_t block_size;
	size_t num_blocks;
	size_t num_used_blocks;
	struct mem_pool_node *free_list;
};

int mem_pool_init(struct mem_pool *pool, void *buf, size_t buf_size,
	size_t block_size);
void mem_pool_finish(struct mem_pool *pool);
void *mem_pool_alloc(struct mem_pool *pool);
int mem_pool_free(struct mem_pool *pool, void *ptr);
int mem_pool_reset(struct mem_pool *pool);
size_t mem_pool_num_unused_blocks(struct mem_pool *pool);

#endif
