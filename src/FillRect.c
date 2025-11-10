#include "FillRect.h"

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

void FillRect(Surface surface, int x, int y, int w, int h, Color color) {
    uint32_t pixelColor = ColorToPixel(surface.format, color);
    switch (surface.format->bytesPerPixel) {
        case 1: {
            FillRect1(surface, x, y, w, h, (uint8_t)pixelColor);
        } break;
        case 2: {
            FillRect2(surface, x, y, w, h, (uint16_t)pixelColor);
        } break;
        case 4: {
            FillRect4(surface, x, y, w, h, pixelColor);
        } break;
        default: break;
    }
}
