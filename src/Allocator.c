#include "Allocator.h"

#define ALIGN_UP(x, a) (((x) + ((a) - 1)) & ~((a) - 1))

#ifdef ESP_PLATFORM
#include <esp_heap_caps.h>

static void* MallocWrapper(size_t bytes, void* user) {
    (void)user;
    const int alignment = 8;
    bytes = ALIGN_UP(bytes, alignment);
    return heap_caps_aligned_alloc(alignment, bytes, MALLOC_CAP_8BIT);
}

static void FreeWrapper(void* ptr, void* user) {
    (void)user;
    heap_caps_free(ptr);
}
#else
#include <stdlib.h>

static void* MallocWrapper(size_t bytes, void* user) {
    (void)user;
    const int alignment = 16;
    bytes = ALIGN_UP(bytes, alignment);
    return aligned_alloc(alignment, bytes);
}

static void FreeWrapper(void* ptr, void* user) {
    (void)user;
    free(ptr);
}
#endif

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
