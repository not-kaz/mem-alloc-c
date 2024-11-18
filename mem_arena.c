#include <stdint.h>
#include <string.h>
#include "mem_arena.h"

#define DEFAULT_ALIGNMENT (sizeof(void *))

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
	uintptr_t offset, mod;
	void *ptr;

	if (!arena || !size || size > arena->capacity) {
		return NULL;
	}
	offset = (uintptr_t) arena->buf + arena->curr_offset;
	mod = offset & (DEFAULT_ALIGNMENT - 1);
	if (mod) {
		offset += DEFAULT_ALIGNMENT - mod;
	}
	offset -= (uintptr_t) arena->buf;
	if (offset + size <= arena->capacity) {
		ptr = &arena->buf[offset];
		arena->prev_offset = offset;
		arena->curr_offset = offset + size;
		memset(ptr, 0, size);
		return ptr;
	}
	return NULL;
}

void
mem_arena_reset(struct mem_arena *arena)
{
	if (arena) {
		arena->prev_offset = 0;
		arena->curr_offset = 0;
	}
}
