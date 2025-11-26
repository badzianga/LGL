#ifdef __SSE2__
#include <emmintrin.h>
#endif  // __SSE2__

#include "FillRect.h"
#include "internal/Inlines.h"
#include "Rect.h"

#ifdef __SSE2__
static void FillRect1SSE(uint8_t* target, int stride, int w, int h, uint32_t color) {
    const __m128i v = _mm_set1_epi32((int)color);

    while (h--) {
        uint8_t* row = target;
        int n = w;
        while (n >= 16) {
            _mm_storeu_si128((__m128i*)row, v);
            row += 16;
            n -= 16;
        }

        const uint8_t c = (uint8_t)color;
        while (n--) {
            *row++ = c;
        }

        target += stride;
    }
}

static void FillRect2SSE(uint8_t* target, int stride, int w, int h, uint32_t color) {
    const __m128i v = _mm_set1_epi32((int)color);

    while (h--) {
        uint8_t* row = target;
        int n = w;
        while (n >= 8) {
            _mm_storeu_si128((__m128i*)row, v);
            row += 16;
            n -= 8;
        }

        const uint16_t c = (uint16_t)color;
        uint16_t* ptr = (uint16_t*)row;
        while (n--) {
            *ptr++ = c;
        }

        target += stride;
    }
}

static void FillRect4SSE(uint8_t* target, int stride, int w, int h, uint32_t color) {
    const __m128i v = _mm_set1_epi32((int)color);
    while (h--) {
        uint8_t* row = target;
        int n = w;
        while (n >= 4) {
            _mm_storeu_si128((__m128i*)row, v);
            row += 16;
            n -= 4;
        }

        Memset4(row, color, n);

        target += stride;
    }
}
#else
static void FillRect1(uint8_t* target, int stride, int w, int h, uint32_t color) {
    const int quads = w >> 2;
    const int remaining = w & 3;
    const int offset = quads << 2;

    while (h--) {
        Memset4(target, color, quads);
        uint8_t* pixel = target + offset;
        int n = remaining;
        while (n--) {
            *pixel++ = color;
        }
        target += stride;
    }
}

static void FillRect2(uint8_t* target, int stride, int w, int h, uint32_t color) {
    const int pairs = w >> 1;
    const int odd = w & 1;
    const int offset = pairs << 2;

    while (h--) {
        Memset4(target, color, pairs);
        if (odd) {
            *(uint16_t*)(target + offset) = (uint16_t)color;
        }
        target += stride;
    }
}

static void FillRect4(uint8_t* target, int stride, int w, int h, uint32_t color) {
    while (h--) {
        Memset4(target, color, w);
        target += stride;
    }
}
#endif  // __SSE2__

void FillRect(Surface surface, const Rect* rect, uint32_t color) {
    const uint8_t bpp = surface.format->bytesPerPixel;
    const Rect surfaceRect = { 0, 0, surface.width, surface.height };
    Rect clipped;

    if (!RectIntersection(&surfaceRect, rect, &clipped)) return;

    uint8_t* row = (uint8_t*)surface.pixels + clipped.y * surface.stride + clipped.x * bpp;

    switch (bpp) {
        case 1: {
            color |= color << 8;
            color |= color << 16;
#ifdef __SSE2__
            FillRect1SSE(row, surface.stride, clipped.width, clipped.height, color);
#else
            FillRect1(row, surface.stride, clipped.width, clipped.height, color);
#endif  // __SSE2__
        } break;
        case 2: {
            color |= color << 16;
#ifdef __SSE2__
            FillRect2SSE(row, surface.stride, clipped.width, clipped.height, color);
#else
            FillRect2(row, surface.stride, clipped.width, clipped.height, color);
#endif  // __SSE2__
        } break;
        case 4: {
#ifdef __SSE2__
            FillRect4SSE(row, surface.stride, clipped.width, clipped.height, color);
#else
            FillRect4(row, surface.stride, clipped.width, clipped.height, color);
#endif  // __SSE2__
        } break;
        default: break;
    }
}
