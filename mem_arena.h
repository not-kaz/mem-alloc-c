#ifndef MEM_ARENA_H
#define MEM_ARENA_H

#include <stddef.h>

#define MEM_ARENA_RESULT_OK 0
#define MEM_ARENA_RESULT_INVALID_ARG 1

struct mem_arena {
	unsigned char *buffer;
	size_t buffer_size;
	size_t curr_offset;
};

int mem_arena_init(struct mem_arena *arena, void *buf, size_t buf_size);
void mem_arena_finish(struct mem_arena *arena);
void *mem_arena_alloc(struct mem_arena *arena, size_t size);
void mem_arena_reset(struct mem_arena *arena);

#endif
