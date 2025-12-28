#ifndef LGL_ALLOCATOR_H
#define LGL_ALLOCATOR_H
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef void* (*FnAlloc)(size_t bytes, void* user);
typedef void (*FnFree)(void* ptr, void* user);

typedef struct Allocator {
    FnAlloc alloc;
    FnFree free;
    void* user;
} Allocator;

void AllocatorSetGlobal(Allocator* allocator);
Allocator* AllocatorGet();

#define AllocatorAlloc(bytes) AllocatorGet()->alloc(bytes, AllocatorGet()->user)
#define AllocatorFree(ptr) AllocatorGet()->free(ptr, AllocatorGet()->user)

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // LGL_ALLOCATOR_H
