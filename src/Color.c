#include "Color.h"

uint32_t ColorToPixel(const PixelFormat* format, Color color) {
    uint32_t r = (color.r >> format->rLoss) << format->rShift;
    uint32_t g = (color.g >> format->gLoss) << format->gShift;
    uint32_t b = (color.b >> format->bLoss) << format->bShift;
    uint32_t a = (color.a >> format->aLoss) << format->aShift;
    return (r & format->rMask) | (g & format->gMask) | (b & format->bMask) | (a & format->aMask);
}

Color PixelToColor(const PixelFormat* format, uint32_t pixel) {
    Color color;
    color.r = ((pixel & format->rMask) >> format->rShift) << format->rLoss;
    color.g = ((pixel & format->gMask) >> format->gShift) << format->gLoss;
    color.b = ((pixel & format->bMask) >> format->bShift) << format->bLoss;
    color.a = ((pixel & format->aMask) >> format->aShift) << format->aLoss;
    return color;
}
