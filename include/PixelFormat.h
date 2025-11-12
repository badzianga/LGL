#pragma once
#include <stdint.h>

#include "Color.h"

typedef struct PixelFormat {
    uint32_t rMask;
    uint32_t gMask;
    uint32_t bMask;
    uint32_t aMask;
    
    uint8_t rShift;
    uint8_t gShift;
    uint8_t bShift;
    uint8_t aShift;

    uint8_t rLoss;
    uint8_t gLoss;
    uint8_t bLoss;
    uint8_t aLoss;

    uint8_t bytesPerPixel;
} PixelFormat;

extern const PixelFormat FORMAT_RGBA8888;
extern const PixelFormat FORMAT_ABGR8888;
extern const PixelFormat FORMAT_ARGB8888;
extern const PixelFormat FORMAT_BGRA8888;
extern const PixelFormat FORMAT_RGB565;
extern const PixelFormat FORMAT_BGR565;
extern const PixelFormat FORMAT_RGB332;
extern const PixelFormat FORMAT_BGR233;

uint32_t ColorToPixel(const PixelFormat* format, Color color);
Color PixelToColor(const PixelFormat* format, uint32_t pixel);
