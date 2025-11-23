#include <stdlib.h>

#include "Allocator.h"

static void* MallocWrapper(size_t bytes, void* user) {
    (void)user;
    return malloc(bytes);
}

static void FreeWrapper(void* ptr, void* user) {
    (void)user;
    free(ptr);
}

static Allocator defaultAlloc = {
    .alloc = MallocWrapper,
    .free = FreeWrapper,
    .user = NULL
};

static Allocator* globalAlloc = &defaultAlloc;

void AllocatorSetGlobal(Allocator* allocator) {
    globalAlloc = allocator;
}

Allocator* AllocatorGet() {
    return globalAlloc;
}
