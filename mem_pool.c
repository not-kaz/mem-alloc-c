#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "mem_pool.h"

#define DEFAULT_ALIGNMENT (sizeof(void *))
#define ALIGN_FORWARD(x, alignment) \
	if ((x) & ((alignment) - 1)) { \
		(x) += (alignment) - ((x) & ((alignment) - 1)); \
	}
#define IS_POWER_OF_TWO(x) ((x) > 0 && (((x) & ((x) - 1)) == 0))
#define IS_VALID_MEM_POOL(pool) \
	((pool) && (pool->buf) && (pool->capacity) && (pool->chunk_size))

void
mem_pool_init(struct mem_pool *pool, void *buf, size_t buf_size,
	size_t chunk_size)
{
	uintptr_t start;

	if (!pool || !buf_size || chunk_size > buf_size) {
		return;
	}
	start = (uintptr_t) buf;
	if (!IS_POWER_OF_TWO(DEFAULT_ALIGNMENT)) {
		return;
	}
	ALIGN_FORWARD(start, DEFAULT_ALIGNMENT);
	buf_size -= (size_t) (start - (uintptr_t) buf);
	ALIGN_FORWARD(chunk_size, DEFAULT_ALIGNMENT);
	if (chunk_size > buf_size) {
		return;
	}
	pool->buf = (unsigned char *) buf;
	pool->capacity = buf_size;
	pool->chunk_size = chunk_size;
	mem_pool_reset(pool);
}

void *
mem_pool_alloc(struct mem_pool *pool)
{
	struct mem_pool_list_node *node;

	if (!IS_VALID_MEM_POOL(pool) || !pool->free_list_head) {
		return NULL;
	}
	node = pool->free_list_head;
	pool->free_list_head = pool->free_list_head->next;
	memset(node, 0, pool->chunk_size);
	return node;
}

void
mem_pool_reset(struct mem_pool *pool)
{
	if (!IS_VALID_MEM_POOL(pool)) {
		return;
	}
	pool->free_list_head = NULL;
	for (size_t i = 0; i < pool->capacity / pool->chunk_size; i++) {
		void *ptr;
		struct mem_pool_list_node *node;

		ptr = &pool->buf[i * pool->chunk_size];
		node = (struct mem_pool_list_node *) ptr;
		node->next = pool->free_list_head;
		pool->free_list_head = node;
	}
}

size_t
mem_pool_calc_free_chunks(struct mem_pool *pool)
{
	struct mem_pool_list_node *curr = NULL;
	size_t count = 0;

	if (!IS_VALID_MEM_POOL(pool)) {
		return 0;
	}
	curr = pool->free_list_head;
	while (curr) {
		count++;
		curr = curr->next;
	}
	return count;
}
