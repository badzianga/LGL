#include "FillRect.h"
#include "PixelFormat.h"

static void FillRect1(Surface surface, int x, int y, int w, int h, uint8_t color) {
    for (int hi = 0; hi < h; ++hi) {
        int yi = y + hi;
        if (yi < 0 || yi >= surface.height) continue;
        for (int wi = 0; wi < w; ++wi) {
            int xi = x + wi;
            if (xi < 0 || xi >= surface.width) continue;
            ((uint8_t*)surface.pixels)[yi * surface.width + xi] = color; 
        }
    }
}

static void FillRect1a(Surface surface, int x, int y, int w, int h, uint8_t color) {
    const Color srcColor = PixelToColor(surface.format, color);
    if (srcColor.a == 0) return;

    for (int hi = 0; hi < h; ++hi) {
        int yi = y + hi;
        if (yi < 0 || yi >= surface.height) continue;
        for (int wi = 0; wi < w; ++wi) {
            int xi = x + wi;
            if (xi < 0 || xi >= surface.width) continue;

            const uint8_t dstPixel = ((uint8_t*)surface.pixels)[yi * surface.width + xi];
            const Color dstColor = PixelToColor(surface.format, dstPixel);

            const uint8_t a = srcColor.a;
            const uint8_t invA = 255 - a;

            Color out = {
                .r = (srcColor.r * a + dstColor.r * invA) / 255,
                .g = (srcColor.g * a + dstColor.g * invA) / 255,
                .b = (srcColor.b * a + dstColor.b * invA) / 255,
                .a = 255
                // .a = (srcColor.a + dstColor.a * (255 - srcColor.a) / 255)
            };

            const uint8_t outPixel = ColorToPixel(surface.format, out);
            ((uint8_t*)surface.pixels)[yi * surface.width + xi] = outPixel;
        }
    }
}

static void FillRect2(Surface surface, int x, int y, int w, int h, uint16_t color) {
    for (int hi = 0; hi < h; ++hi) {
        int yi = y + hi;
        if (yi < 0 || yi >= surface.height) continue;
        for (int wi = 0; wi < w; ++wi) {
            int xi = x + wi;
            if (xi < 0 || xi >= surface.width) continue;
            ((uint16_t*)surface.pixels)[yi * surface.width + xi] = color; 
        }
    }
}

static void FillRect2a(Surface surface, int x, int y, int w, int h, uint16_t color) {
    const Color srcColor = PixelToColor(surface.format, color);
    if (srcColor.a == 0) return;

    for (int hi = 0; hi < h; ++hi) {
        int yi = y + hi;
        if (yi < 0 || yi >= surface.height) continue;
        for (int wi = 0; wi < w; ++wi) {
            int xi = x + wi;
            if (xi < 0 || xi >= surface.width) continue;

            const uint16_t dstPixel = ((uint16_t*)surface.pixels)[yi * surface.width + xi];
            const Color dstColor = PixelToColor(surface.format, dstPixel);

            const uint8_t a = srcColor.a;
            const uint8_t invA = 255 - a;

            Color out = {
                .r = (srcColor.r * a + dstColor.r * invA) / 255,
                .g = (srcColor.g * a + dstColor.g * invA) / 255,
                .b = (srcColor.b * a + dstColor.b * invA) / 255,
                .a = 255
                // .a = (srcColor.a + dstColor.a * (255 - srcColor.a) / 255)
            };

            const uint16_t outPixel = ColorToPixel(surface.format, out);
            ((uint16_t*)surface.pixels)[yi * surface.width + xi] = outPixel;
        }
    }
}

static void FillRect4(Surface surface, int x, int y, int w, int h, uint32_t color) {
    for (int hi = 0; hi < h; ++hi) {
        int yi = y + hi;
        if (yi < 0 || yi >= surface.height) continue;
        for (int wi = 0; wi < w; ++wi) {
            int xi = x + wi;
            if (xi < 0 || xi >= surface.width) continue;
            ((uint32_t*)surface.pixels)[yi * surface.width + xi] = color; 
        }
    }
}

static void FillRect4a(Surface surface, int x, int y, int w, int h, uint32_t color) {
    const Color srcColor = PixelToColor(surface.format, color);
    if (srcColor.a == 0) return;

    for (int hi = 0; hi < h; ++hi) {
        int yi = y + hi;
        if (yi < 0 || yi >= surface.height) continue;
        for (int wi = 0; wi < w; ++wi) {
            int xi = x + wi;
            if (xi < 0 || xi >= surface.width) continue;

            const uint32_t dstPixel = ((uint32_t*)surface.pixels)[yi * surface.width + xi];
            const Color dstColor = PixelToColor(surface.format, dstPixel);

            const uint8_t a = srcColor.a;
            const uint8_t invA = 255 - a;

            Color out = {
                .r = (srcColor.r * a + dstColor.r * invA) / 255,
                .g = (srcColor.g * a + dstColor.g * invA) / 255,
                .b = (srcColor.b * a + dstColor.b * invA) / 255,
                .a = 255
                // .a = (srcColor.a + dstColor.a * (255 - srcColor.a) / 255)
            };

            const uint32_t outPixel = ColorToPixel(surface.format, out);
            ((uint32_t*)surface.pixels)[yi * surface.width + xi] = outPixel;
        }
    }
}

void FillRect(Surface surface, int x, int y, int w, int h, Color color) {
    uint32_t pixelColor = ColorToPixel(surface.format, color);
    switch (surface.format->bytesPerPixel) {
        case 1: {
            if (surface.format->aMask == 0 || color.a == 255) {
                FillRect1(surface, x, y, w, h, (uint8_t)pixelColor);
            }
            else {
                if (color.a == 0) return;
                FillRect1a(surface, x, y, w, h, pixelColor);
            }
        } break;
        case 2: {
            if (surface.format->aMask == 0 || color.a == 255) {
                FillRect2(surface, x, y, w, h, (uint16_t)pixelColor);
            }
            else {
                if (color.a == 0) return;
                FillRect2a(surface, x, y, w, h, pixelColor);
            }
        } break;
        case 4: {
            if (surface.format->aMask == 0 || color.a == 255) {
                FillRect4(surface, x, y, w, h, pixelColor);
            }
            else {
                if (color.a == 0) return;
                FillRect4a(surface, x, y, w, h, pixelColor);
            }
        } break;
        default: break;
    }
}
