#include "BlendFillRect.h"
#include "FillRect.h"
#include "internal/Inlines.h"

static void BlendFillRect1(uint8_t* target, int stride, int w, int h, Color color, const PixelFormat* format) {
    const uint8_t a = color.a;
    const uint8_t invA = 255 - a;

    while (h--) {
        uint8_t* pixel = target;
        int n = w;
        while (n--) {
            Color c = PixelToColor(format, *pixel);
            c = BlendColors(color, c, a, invA);
            *pixel++ = (uint8_t)ColorToPixel(format, c);
        }
        target += stride;
    }
}

static void BlendFillRect2(uint8_t* target, int stride, int w, int h, Color color, const PixelFormat* format) {
    const uint8_t a = color.a;
    const uint8_t invA = 255 - a;

    while (h--) {
        uint16_t* pixel = (uint16_t*)target;
        int n = w;
        while (n--) {
            Color c = PixelToColor(format, *pixel);
            c = BlendColors(color, c, a, invA);
            *pixel++ = (uint16_t)ColorToPixel(format, c);
        }
        target += stride;
    }
}

static void BlendFillRect4(uint8_t* target, int stride, int w, int h, Color color, const PixelFormat* format) {
    const uint8_t a = color.a;
    const uint8_t invA = 255 - a;

    while (h--) {
        uint32_t* pixel = (uint32_t*)target;
        int n = w;
        while (n--) {
            Color c = PixelToColor(format, *pixel);
            c = BlendColors(color, c, a, invA);
            *pixel++ = ColorToPixel(format, c);
        }
        target += stride;
    }
}

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
