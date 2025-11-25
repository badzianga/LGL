#include "BlendFillRect.h"
#include "FillRect.h"
#include "internal/Inlines.h"

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
