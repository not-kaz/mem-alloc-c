#include <stdint.h>
#include <string.h>
#include "mem_arena.h"

#define DEFAULT_ALIGNMENT (2 * sizeof(void *))

void
mem_arena_init(struct mem_arena *arena, void *buf, size_t buf_size)
{
	if (!arena || !buf || !buf_size) {
		return;
	}
	arena->buf = (unsigned char *) buf;
	arena->capacity = buf_size;
	arena->prev_offset = 0;
	arena->curr_offset = 0;
}

void
mem_arena_finish(struct mem_arena *arena)
{
	if (arena) {
		memset(arena, 0, sizeof(struct mem_arena));
	}
}

void *
mem_arena_alloc(struct mem_arena *arena, size_t size)
{
	uintptr_t base, offset, aligned_offset;
	void *ptr;

	if (!arena || !size || size > arena->capacity) {
		return NULL;
	}
	base = (uintptr_t) arena->buf;
	offset = (uintptr_t) arena->curr_offset;
	if (offset > UINTPTR_MAX - base) {
		return NULL;
	}
	offset = base + offset;
	aligned_offset = offset + (DEFAULT_ALIGNMENT - 1);
	if (aligned_offset < offset) {
		return NULL;
	}
	aligned_offset &= ~(DEFAULT_ALIGNMENT - 1);
	aligned_offset -= base;
	if (size > arena->capacity - aligned_offset) {
		return NULL;
	}
	ptr = &arena->buf[aligned_offset];
	arena->prev_offset = aligned_offset;
	arena->curr_offset = aligned_offset + size;
	memset(ptr, 0, size);
	return ptr;
}

void
mem_arena_reset(struct mem_arena *arena)
{
	if (arena) {
		arena->prev_offset = 0;
		arena->curr_offset = 0;
	}
}
