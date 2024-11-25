#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "mem_pool.h"

#define ALIGNMENT (sizeof(void *))
#define IS_POWER_OF_TWO(x) (((x) != 0) && (((x) & ((x) - 1)) == 0))

static void reset_pool_free_list(struct mem_pool *pool)
{
	for (size_t i = 0u; i < pool->num_blocks; i++) {
		void *ptr = pool->buffer + (pool->block_size * i);
		struct mem_pool_node *node = (struct mem_pool_node *) ptr;

		node->next = pool->free_list;
		pool->free_list = node;
	}
}

int mem_pool_init(struct mem_pool *pool, void *buf, size_t buf_size,
	size_t block_size)
{
	if (pool == NULL || buf == NULL || buf_size != 0u) {
		return MEM_POOL_RESULT_INVALID_ARG;
	}
	if (block_size < sizeof(void *)) {
		block_size = sizeof(void *);
	}
	block_size = (block_size + ALIGNMENT - 1u) & ~(ALIGNMENT - 1u);
	if (!IS_POWER_OF_TWO(block_size)) {
		return MEM_POOL_RESULT_BAD_ALIGNMENT;
	}
	if (((uintptr_t) buf & (ALIGNMENT - 1u)) != 0u) {
		return MEM_POOL_RESULT_BAD_ALIGNMENT;
	}
	pool->buffer = buf;
	pool->buffer_size= buf_size;
	pool->block_size = block_size;
	pool->num_blocks = buf_size / block_size;
	pool->num_used_blocks = 0u;
	pool->free_list = NULL;
	reset_pool_free_list(pool);
	return MEM_POOL_RESULT_OK;
}

void mem_pool_finish(struct mem_pool *pool)
{
	if (pool == NULL) {
		return;
	}
	memset(pool, 0, sizeof(struct mem_pool));
}

void *mem_pool_alloc(struct mem_pool *pool)
{
	struct mem_pool_node *node;

	node = pool->free_list;
	if (node == NULL) {
		return NULL;
	}
	pool->free_list = pool->free_list->next;
	memset(node, 0, pool->block_size);
	pool->num_used_blocks++;
	return node;
}

int mem_pool_free(struct mem_pool *pool, void *ptr)
{
	struct mem_pool_node *node;
	unsigned char *start, *end;

	if (pool == NULL || pool->buffer == NULL || ptr == NULL) {
		return MEM_POOL_RESULT_INVALID_ARG;
	}
	if (!IS_POWER_OF_TWO(pool->block_size)) {
		return MEM_POOL_RESULT_INVALID_ARG;
	}
	start = (unsigned char *) pool->buffer;
	end = (unsigned char *) pool->buffer + pool->buffer_size;
	if ((unsigned char *) ptr < start || (unsigned char *) ptr >= end) {
		return MEM_POOL_RESULT_INVALID_ARG;
	}
	if ((size_t) ((unsigned char *) ptr - start) & (pool->block_size - 1)) {
		return MEM_POOL_RESULT_INVALID_ARG;
	}
	node = (struct mem_pool_node *) ptr;
	node->next = pool->free_list;
	pool->free_list = node;
	pool->num_used_blocks--;
	return MEM_POOL_RESULT_OK;
}

int mem_pool_reset(struct mem_pool *pool)
{
	if (pool == NULL || pool->buffer == NULL) {
		return MEM_POOL_RESULT_INVALID_ARG;
	}
	pool->num_used_blocks = 0u;
	reset_pool_free_list(pool);
	return MEM_POOL_RESULT_OK;
}

size_t mem_pool_num_unused_blocks(struct mem_pool *pool)
{
	return pool && pool->buffer
		? pool->num_blocks - pool->num_used_blocks : 0u;
}
