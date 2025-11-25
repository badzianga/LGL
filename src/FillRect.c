#include "FillRect.h"
#include "internal/Inlines.h"
#include "Rect.h"

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
            FillRect1(row, surface.stride, clipped.width, clipped.height, color);
        } break;
        case 2: {
            color |= color << 16;
            FillRect2(row, surface.stride, clipped.width, clipped.height, color);
        } break;
        case 4: {
            FillRect4(row, surface.stride, clipped.width, clipped.height, color);
        } break;
        default: break;
    }
}
