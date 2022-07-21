#include "base_memory.h"

void InitializeArena(MemoryArena *arena, uint8_t *base, size_t size)
{
    arena->base = base;
    arena->size = size;
    arena->used = 0;
}

void ClearArena(MemoryArena *arena)
{
    arena->used = 0;
}

bool MemoryArenaCanFit(MemoryArena *arena, size_t size)
{
    bool result = ((arena->used + size) <= arena->size);
    return result;
}

void *MemoryArenaPushSize(MemoryArena *arena, size_t size)
{
    void *result = 0;
    bool can_fit = MemoryArenaCanFit(arena, size);
    ASSERT(can_fit);
    if(can_fit)
    {
        result = arena->base + arena->used;
        arena->used += size;
    }
    return result;
}

void *MemoryArenaPopSize(MemoryArena *arena, size_t size)
{
    ASSERT(size > 0);
    ASSERT(arena->used >= size);
    size_t new_used = arena->used - size;
    arena->used = new_used;
    void *result = arena->base + new_used;
    return result;
}
