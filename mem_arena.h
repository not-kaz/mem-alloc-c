#ifndef MEM_ARENA_H
#define MEM_ARENA_H
#include <stddef.h>

struct mem_arena {
	unsigned char *buf;
	size_t size;
	size_t prev_offset;
	size_t curr_offset;
};

void mem_arena_init(struct mem_arena *arena, void *buf, size_t buf_size);
void mem_arena_finish(struct mem_arena *arena);
void *mem_arena_alloc(struct mem_arena *arena, size_t size);
void mem_arena_reset(struct mem_arena *arena);

#endif
