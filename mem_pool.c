#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "mem_pool.h"
#define ALIGN_SIZE (sizeof(void *))
#define IS_POWER_OF_TWO(x) (((x) != 0) && (((x) & ((x) - 1)) == 0))

static void reset_pool_free_list(struct mem_pool *pool)
{
	for (size_t i = 0; i < pool->num_blocks; i++) {
		void *ptr = pool->buf + (pool->block_size * i);
		struct mem_pool_node *node = (struct mem_pool_node *) ptr;

		node->next = pool->free_list;
		pool->free_list = node;
	}
}

void mem_pool_init(struct mem_pool *pool, void *buf, size_t buf_size,
	size_t block_size)
{
	if (!pool || !buf || !buf_size) {
		return;
	}
	if (block_size < sizeof(void *)) {
		block_size = sizeof(void *);
	}
	block_size = (block_size + ALIGN_SIZE - 1) & ~(ALIGN_SIZE - 1);
	if (!IS_POWER_OF_TWO(block_size)) {
		return;
	}
	if (((uintptr_t) buf & (ALIGN_SIZE - 1)) != 0) {
		return;
	}
	pool->buf = buf;
	pool->buf_size = buf_size;
	pool->block_size = block_size;
	pool->num_blocks = buf_size / block_size;
	pool->num_used_blocks = 0;
	pool->free_list = NULL;
	reset_pool_free_list(pool);
}

void mem_pool_finish(struct mem_pool *pool)
{
	if (!pool) {
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

void mem_pool_free(struct mem_pool *pool, void *ptr)
{
	struct mem_pool_node *node;
	char *start, *end;

	if (pool == NULL || pool->buf == NULL || ptr == NULL) {
		return;
	}
	if (!IS_POWER_OF_TWO(pool->block_size)) {
		return;
	}
	start = (char *) pool->buf;
	end = (char *) pool->buf + pool->buf_size;
	if ((char *) ptr < start || (char *) ptr >= end) {
		return;
	}
	// TODO: Make sure block_size is power of two.
	if ((size_t) ((char *) ptr - start) & (pool->block_size - 1)) {
		return;
	}
	node = (struct mem_pool_node *) ptr;
	node->next = pool->free_list;
	pool->free_list = node;
	pool->num_used_blocks--;
}

size_t mem_pool_num_unused_blocks(struct mem_pool *pool)
{
	return pool && pool->buf ? pool->num_blocks - pool->num_used_blocks : 0;
}
