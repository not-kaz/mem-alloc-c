#include <stdint.h>
#include <string.h>
#include "mem_arena.h"

#define DEFAULT_ALIGNMENT (sizeof(void *))

int mem_arena_init(struct mem_arena *arena, void *buf, size_t buf_size)
{
	if (arena == NULL || buf == NULL || buf_size == 0u) {
		return MEM_ARENA_RESULT_INVALID_ARG;
	}
	arena->buffer = (unsigned char *) buf;
	arena->buffer_size = buf_size;
	arena->curr_offset = 0;
	return MEM_ARENA_RESULT_OK;
}

void mem_arena_finish(struct mem_arena *arena)
{
	if (arena != NULL) {
		memset(arena, 0, sizeof(struct mem_arena));
	}
}

void *mem_arena_alloc(struct mem_arena *arena, size_t size)
{
	uintptr_t offset, mod;

	if (arena == NULL || size == 0u || size > arena->buffer_size) {
		return NULL;
	}
	offset = (uintptr_t) arena->buffer + arena->curr_offset;
	mod = offset & (DEFAULT_ALIGNMENT - 1);
	if (mod) {
		offset += DEFAULT_ALIGNMENT - mod;
	}
	offset -= (uintptr_t) arena->buffer;
	if (offset + size <= arena->buffer_size) {
		void *ptr = &arena->buffer[offset];

		arena->curr_offset = offset + size;
		memset(ptr, 0, size);
		return ptr;
	}
	return NULL;
}

void mem_arena_reset(struct mem_arena *arena)
{
	if (arena != NULL) {
		arena->curr_offset = 0;
	}
}
