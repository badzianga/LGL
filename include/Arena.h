#ifndef LGL_ARENA_H
#define LGL_ARENA_H
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct Arena {
    uint8_t* base;
    size_t size;
    size_t offset;
} Arena;

Arena ArenaInit(void* buffer, size_t size);
void* ArenaAlloc(Arena* arena, size_t bytes);
void ArenaReset(Arena* arena);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // LGL_ARENA_H
