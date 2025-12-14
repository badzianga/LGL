#include "Arena.h"
#include "Error.h"

Arena ArenaInit(void* buffer, size_t size) {
    if (buffer == NULL || size == 0) {
        THROW_ERROR(ERR_INVALID_PARAMS);
        return (Arena){ 0 };
    }
    return (Arena){ buffer, size, 0 };
}

void* ArenaAlloc(Arena* arena, size_t bytes) {
    if (arena == NULL || bytes == 0) {
        THROW_ERROR(ERR_INVALID_PARAMS);
        return NULL;
    }
    if (arena->offset + bytes > arena->size) {
        THROW_ERROR(ERR_OUT_OF_MEMORY);
        return NULL;
    }

    void* memory = arena->base + arena->offset;
    arena->offset += bytes;
    return memory;
}

void ArenaReset(Arena* arena) {
    if (arena == NULL) {
        THROW_ERROR(ERR_INVALID_PARAMS);
        return;
    }
    arena->offset = 0;
}
