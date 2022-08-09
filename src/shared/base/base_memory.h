#ifndef BASE_MEMORY_H
#define BASE_MEMORY_H

#include <stdint.h>

#define PUSH_STRUCT(arena, type) (type *)MemoryArenaPushSize(arena, sizeof(type))
#define PUSH_ARRAY(arena, type, count) (type *)MemoryArenaPushSize(arena, (count)*sizeof(type))

// memory arenas
typedef struct MemoryArena
{
    uint8_t *base;
    size_t size;
    size_t used;
} MemoryArena;

void InitializeArena(MemoryArena *arena, uint8_t *base, size_t size);
void ClearArena(MemoryArena *arena);
bool MemoryArenaCanFit(MemoryArena *arena, size_t size);
void *MemoryArenaPushSize(MemoryArena *arena, size_t size);
void *MemoryArenaPopSize(MemoryArena *arena, size_t size);

#endif //BASE_MEMORY_H
