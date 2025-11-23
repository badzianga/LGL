#include "Arena.h"

Arena ArenaInit(void* buffer, size_t size) {
    if (buffer == NULL || size == 0) {
        return (Arena){ 0 };
    }
    return (Arena){ buffer, size, 0 };
}

void* ArenaAlloc(Arena* arena, size_t bytes) {
    if (arena == NULL || bytes == 0) return NULL;
    if (arena->offset + bytes > arena->size) return NULL;

    void* memory = arena->base + arena->offset;
    arena->offset += bytes;
    return memory;
}

void ArenaReset(Arena* arena) {
    arena->offset = 0;
}
