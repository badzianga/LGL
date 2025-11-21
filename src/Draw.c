#include <assert.h>

#include "Draw.h"
#include "FillRect.h"

void DrawRect(Surface surface, int x, int y, int w, int h, Color color) {
    FillRect(surface, x, y, w, h, color);
}

void DrawCircle(Surface surface, int x, int y, int r, Color color) {
    (void) surface;
    (void) x;
    (void) y;
    (void) r;
    (void) color;
    assert(0 && "TODO: not implemented");
}

// TODO: support alpha blending
static void FillFlatLine(Surface surface, int x1, int x2, int y, uint32_t color) {
    const uint8_t bpp = surface.format->bytesPerPixel;

    for (int x = x1; x < x2; ++x) {
        if (x < 0 || x >= surface.width) continue;
        uint8_t* pixel = surface.pixels + (y * surface.width + x) * bpp;
        switch (bpp) {
            case 1: {
                *pixel = (uint8_t)color;
            } break;
            case 2: {
                *(uint16_t*)pixel = (uint16_t)color;
            } break;
            case 4: {
                *(uint32_t*)pixel = color;
            } break;
            default: break;
        }
    }
}

static void FillBottomFlatTriangle(Surface surface, int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color) {
    const float invSlope1 = (float)(x2 - x1) / (float)(y2 - y1);
    const float invSlope2 = (float)(x3 - x1) / (float)(y3 - y1);

    float curX1 = (float)x1;
    float curX2 = (float)x1;

    for (int scanlineY = y1; scanlineY <= y2; ++scanlineY) {
        if (scanlineY < 0 || scanlineY >= surface.height) continue;
        FillFlatLine(surface, (int)curX1, (int)curX2, scanlineY, color);
        curX1 += invSlope1;
        curX2 += invSlope2;
    }
}

static void FillTopFlatTriangle(Surface surface, int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color) {
    const float invSlope1 = (float)(x3 - x1) / (float)(y3 - y1);
    const float invSlope2 = (float)(x3 - x2) / (float)(y3 - y2);

    float curX1 = (float)x3;
    float curX2 = (float)x3;

    for (int scanlineY = y3; scanlineY > y1; --scanlineY) {
        if (scanlineY < 0 || scanlineY >= surface.height) continue;
        FillFlatLine(surface, (int)curX1, (int)curX2, scanlineY, color);
        curX1 -= invSlope1;
        curX2 -= invSlope2;
    }
}

static void SortTrianglePointsAscendingByY(int* x1, int* y1, int* x2, int* y2, int* x3, int* y3) {
    if (*y1 > *y3) {
        const int tx = *x1, ty = *y1;
        *x1 = *x3, *y1 = *y3;
        *x3 = tx, *y3 = ty;
    }

    if (*y1 > *y2) {
        const int tx = *x1, ty = *y1;
        *x1 = *x2, *y1 = *y2;
        *x2 = tx, *y2 = ty;
    }

    if (*y2 > *y3) {
        const int tx = *x2, ty = *y2;
        *x2 = *x3, *y2 = *y3;
        *x3 = tx, *y3 = ty;
    }
}

static void SwapInts(int* a, int* b) {
    const int temp = *a;
    *a = *b;
    *b = temp;
}

void DrawTriangle(Surface surface, int x1, int y1, int x2, int y2, int x3, int y3, Color color) {
    SortTrianglePointsAscendingByY(&x1, &y1, &x2, &y2, &x3, &y3);

    const uint32_t pixelColor = ColorToPixel(surface.format, color);

    if (y2 == y3) {
        if (x2 > x3) SwapInts(&x2, &x3);
        FillBottomFlatTriangle(surface, x1, y1, x2, y2, x3, y3, pixelColor);
    }
    else if (y1 == y2) {
        if (x1 > x2) SwapInts(&x1, &x2);
        FillTopFlatTriangle(surface, x1, y1, x2, y2, x3, y3, pixelColor);
    }
    else {
        int x4 = x1 + (int)(((float)(y2 - y1) / (float)(y3 - y1)) * (float)(x3 - x1));
        const int y4 = y2;
        if (x2 > x4) SwapInts(&x2, &x4);
        FillBottomFlatTriangle(surface, x1, y1, x2, y2, x4, y4, pixelColor);
        FillTopFlatTriangle(surface, x2, y2, x4, y4, x3, y3, pixelColor);
    }
}

void DrawLine(Surface surface, int x1, int y1, int x2, int y2, Color color) {
    (void) surface;
    (void) x1;
    (void) y1;
    (void) x2;
    (void) y2;
    (void) color;
    assert(0 && "TODO: not implemented");
}
