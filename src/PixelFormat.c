#include <stddef.h>

#include "Color.h"
#include "PixelFormat.h"

const PixelFormat FORMAT_RGBA8888 = {
    .rMask = 0xFF000000,
    .gMask = 0x00FF0000,
    .bMask = 0x0000FF00,
    .aMask = 0x000000FF,

    .rShift = 24,
    .gShift = 16,
    .bShift = 8,
    .aShift = 0,

    .rLoss = 0,
    .gLoss = 0,
    .bLoss = 0,
    .aLoss = 0,

    .bytesPerPixel = 4,
};

const PixelFormat FORMAT_ABGR8888 = {
    .rMask = 0x000000FF,
    .gMask = 0x0000FF00,
    .bMask = 0x00FF0000,
    .aMask = 0xFF000000,

    .rShift = 0,
    .gShift = 8,
    .bShift = 16,
    .aShift = 24,

    .rLoss = 0,
    .gLoss = 0,
    .bLoss = 0,
    .aLoss = 0,

    .bytesPerPixel = 4,
};

const PixelFormat FORMAT_ARGB8888 = {
    .rMask = 0x00FF0000,
    .gMask = 0x0000FF00,
    .bMask = 0x000000FF,
    .aMask = 0xFF000000,

    .rShift = 16,
    .gShift = 8,
    .bShift = 0,
    .aShift = 24,

    .rLoss = 0,
    .gLoss = 0,
    .bLoss = 0,
    .aLoss = 0,
    
    .bytesPerPixel = 4,
};

const PixelFormat FORMAT_BGRA8888 = {
    .rMask = 0x0000FF00,
    .gMask = 0x00FF0000,
    .bMask = 0xFF000000,
    .aMask = 0x000000FF,

    .rShift = 8,
    .gShift = 16,
    .bShift = 24,
    .aShift = 0,

    .rLoss = 0,
    .gLoss = 0,
    .bLoss = 0,
    .aLoss = 0,

    .bytesPerPixel = 4,
};

const PixelFormat FORMAT_RGB565 = {
    .rMask = 0b1111100000000000,
    .gMask = 0b0000011111100000,
    .bMask = 0b0000000000011111,
    .aMask = 0b0000000000000000,

    .rShift = 11,
    .gShift = 5,
    .bShift = 0,
    .aShift = 0,

    .rLoss = 3,
    .gLoss = 2,
    .bLoss = 3,
    .aLoss = 8,

    .bytesPerPixel = 2,
};

const PixelFormat FORMAT_BGR565 = {
    .rMask = 0b0000000000011111,
    .gMask = 0b0000011111100000,
    .bMask = 0b1111100000000000,
    .aMask = 0b0000000000000000,

    .rShift = 0,
    .gShift = 5,
    .bShift = 11,
    .aShift = 0,

    .rLoss = 3,
    .gLoss = 2,
    .bLoss = 3,
    .aLoss = 8,

    .bytesPerPixel = 2,
};

const PixelFormat FORMAT_RGB332 = {
    .rMask = 0b11100000,
    .gMask = 0b00011100,
    .bMask = 0b00000011,
    .aMask = 0b00000000,

    .rShift = 5,
    .gShift = 2,
    .bShift = 0,
    .aShift = 0,

    .rLoss = 5,
    .gLoss = 5,
    .bLoss = 6,
    .aLoss = 8,

    .bytesPerPixel = 1,
};

const PixelFormat FORMAT_BGR233 = {
    .rMask = 0b00000111,
    .gMask = 0b00111000,
    .bMask = 0b11000000,
    .aMask = 0b00000000,

    .rShift = 0,
    .gShift = 3,
    .bShift = 6,
    .aShift = 0,

    .rLoss = 5,
    .gLoss = 5,
    .bLoss = 6,
    .aLoss = 8,

    .bytesPerPixel = 1,
};

uint32_t ColorToPixel(const PixelFormat* format, Color color) {
    const uint32_t r = (color.r >> format->rLoss) << format->rShift;
    const uint32_t g = (color.g >> format->gLoss) << format->gShift;
    const uint32_t b = (color.b >> format->bLoss) << format->bShift;
    const uint32_t a = (color.a >> format->aLoss) << format->aShift;

    return r | g | b | a;
}

Color PixelToColor(const PixelFormat* format, uint32_t pixel) {
    Color color;
    color.r = ((pixel & format->rMask) >> format->rShift) << format->rLoss;
    color.g = ((pixel & format->gMask) >> format->gShift) << format->gLoss;
    color.b = ((pixel & format->bMask) >> format->bShift) << format->bLoss;
    color.a = (format->aMask == 0) ? 255 : ((pixel & format->aMask) >> format->aShift) << format->aLoss;

    return color;
}
