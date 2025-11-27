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
            *pixel++ = (uint8_t)color;
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

#define MAKE_BLEND_FILL_FUNCTION(TYPE, BYTES)                                                                         \
static void BlendFillRect##BYTES(uint8_t* target, int stride, int w, int h, Color color, const PixelFormat* format) { \
    const uint8_t a = color.a;                                                                                        \
    const uint8_t invA = 255 - a;                                                                                     \
    while (h--) {                                                                                                     \
        TYPE* pixel = (TYPE*)target;                                                                                  \
        int n = w;                                                                                                    \
        while (n--) {                                                                                                 \
            Color c = PixelToColor(format, *pixel);                                                                   \
            c = BlendColors(color, c, a, invA);                                                                       \
            *pixel++ = (TYPE)ColorToPixel(format, c);                                                                 \
        }                                                                                                             \
        target += stride;                                                                                             \
    }                                                                                                                 \
}

MAKE_BLEND_FILL_FUNCTION(uint8_t, 1)
MAKE_BLEND_FILL_FUNCTION(uint16_t, 2)
MAKE_BLEND_FILL_FUNCTION(uint32_t, 4)

void BlendFillRect(Surface surface, const Rect* rect, Color color) {
    const uint8_t bpp = surface.format->bytesPerPixel;
    const Rect surfaceRect = { 0, 0, surface.width, surface.height };
    Rect clipped;

    if (!RectIntersection(&surfaceRect, rect, &clipped)) return;

    uint8_t* row = (uint8_t*)surface.pixels + clipped.y * surface.stride + clipped.x * bpp;

    switch (bpp) {
        case 1: {
            BlendFillRect1(row, surface.stride, clipped.width, clipped.height, color, surface.format);
        } break;
        case 2: {
            BlendFillRect2(row, surface.stride, clipped.width, clipped.height, color, surface.format);
        } break;
        case 4: {
            BlendFillRect4(row, surface.stride, clipped.width, clipped.height, color, surface.format);
        } break;
        default: break;
    }
}

