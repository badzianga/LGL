#include <assert.h>
#include <stddef.h>

#include "FillRect.h"
#include "Rect.h"

// TODO: optimize with loop unrolling
static inline void Memset4(void* ptr, uint32_t value, size_t n) {
    uint32_t* p = ptr;
    while (n--) {
        *p++ = value;
    }
}

static void FillRect4(uint8_t* target, int stride, int w, int h, uint32_t color) {
    while (h--) {
        Memset4(target, color, w);
        target += stride;
    }
}

void FillRect(Surface surface, const Rect* rect, uint32_t color) {
    const uint8_t bpp = surface.format->bytesPerPixel;
    const Rect surfaceRect = { 0, 0, surface.width, surface.height };
    Rect clipped;

    if (!RectIntersection(&surfaceRect, rect, &clipped)) return;

    uint8_t* start = (uint8_t*)surface.pixels + clipped.y * surface.stride + clipped.x * bpp;

    switch (bpp) {
        case 1: {
            color |= color << 8;
            color |= color << 16;
            assert(0 && "TODO: not implemented");
        } break;
        case 2: {
            color |= color << 16;
            assert(0 && "TODO: not implemented");
        } break;
        case 4: {
            FillRect4(start, surface.stride, clipped.width, clipped.height, color);
        } break;
        default: break;
    }
}
