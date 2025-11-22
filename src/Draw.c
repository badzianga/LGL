#include <stdlib.h>

#include "Draw.h"
#include "FillRect.h"

static void SetPixel(uint8_t* pixel, uint32_t color, uint8_t bpp) {
    switch (bpp) {
        case 1: {
            *(uint8_t*)pixel = (uint8_t)color;
        } break;
        case 2: {
            *(uint16_t*)pixel = (uint16_t)color;
        } break;
        case 4: {
            *(uint32_t*)pixel = (uint32_t)color;
        } break;
        default: break;
    }
}

static uint32_t GetPixelValue(const uint8_t* pixel, uint8_t bpp) {
    switch (bpp) {
        case 1: return *(uint8_t*)pixel;
        case 2: return *(uint16_t*)pixel;
        case 4: return *(uint32_t*)pixel;
        default: return 0;
    }
}

static Color BlendColors(Color c1, Color c2) {
    const uint8_t a = c1.a;
    const uint8_t invA = 255 - a;

    return (Color){
        .r = (c1.r * a + c2.r * invA) / 255,
        .g = (c1.g * a + c2.g * invA) / 255,
        .b = (c1.b * a + c2.b * invA) / 255,
        .a = 255
    };
}

void DrawRect(Surface surface, int x, int y, int w, int h, Color color) {
    FillRect(surface, x, y, w, h, color);
}

void DrawCircle(Surface surface, int x, int y, int r, Color color) {
    if (color.a == 0) return;

    const uint8_t bpp = surface.format->bytesPerPixel;
    const uint32_t pixelColor = ColorToPixel(surface.format, color);

    for (int yi = y - r; yi <= y + r; ++yi) {
        if (yi < 0 || yi >= surface.height) continue;
        for (int xi = x - r; xi <= x + r; ++xi) {
            if (xi < 0 || xi >= surface.width) continue;
            const int dx = xi - x;
            const int dy = yi - y;
            if (dx * dx + dy * dy > r * r) continue;
            uint8_t* pixel = surface.pixels + (yi * surface.width + xi) * bpp;

            if (color.a == 255) {
                SetPixel(pixel, pixelColor, bpp);
            }
            else {
                const Color surfColor = PixelToColor(surface.format, GetPixelValue(pixel, bpp));
                const uint32_t blendedPixel = ColorToPixel(surface.format, BlendColors(color, surfColor));
                SetPixel(pixel, blendedPixel, bpp);
            }
        }
    }
}

static void FillFlatLine(Surface surface, int x1, int x2, int y, Color color) {
    const uint32_t pixelColor = ColorToPixel(surface.format, color);
    const uint8_t bpp = surface.format->bytesPerPixel;

    for (int x = x1; x < x2; ++x) {
        if (x < 0 || x >= surface.width) continue;
        uint8_t* pixel = surface.pixels + (y * surface.width + x) * bpp;

        if (color.a == 255) {
            SetPixel(pixel, pixelColor, bpp);
        }
        else {
            const Color surfColor = PixelToColor(surface.format, GetPixelValue(pixel, bpp));
            const uint32_t blendedPixel = ColorToPixel(surface.format, BlendColors(color, surfColor));
            SetPixel(pixel, blendedPixel, bpp);
        }
    }
}

static void FillBottomFlatTriangle(Surface surface, int x1, int y1, int x2, int y2, int x3, int y3, Color color) {
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

static void FillTopFlatTriangle(Surface surface, int x1, int y1, int x2, int y2, int x3, int y3, Color color) {
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

    if (y2 == y3) {
        if (x2 > x3) SwapInts(&x2, &x3);
        FillBottomFlatTriangle(surface, x1, y1, x2, y2, x3, y3, color);
    }
    else if (y1 == y2) {
        if (x1 > x2) SwapInts(&x1, &x2);
        FillTopFlatTriangle(surface, x1, y1, x2, y2, x3, y3, color);
    }
    else {
        int x4 = x1 + (int)(((float)(y2 - y1) / (float)(y3 - y1)) * (float)(x3 - x1));
        const int y4 = y2;
        if (x2 > x4) SwapInts(&x2, &x4);
        FillBottomFlatTriangle(surface, x1, y1, x2, y2, x4, y4, color);
        FillTopFlatTriangle(surface, x2, y2, x4, y4, x3, y3, color);
    }
}

void DrawLine(Surface surface, int x1, int y1, int x2, int y2, Color color) {
    const uint8_t bpp = surface.format->bytesPerPixel;
    const uint32_t pixelColor = ColorToPixel(surface.format, color);

    const int dx = x2 - x1;
    const int dy = y2 - y1;

    if (dx != 0) {  // non-vertical line
        if (abs(dx) >= abs(dy)) {  // slope less or equal to 1
            const int x2Original = x2;
            if (x1 > x2) SwapInts(&x1, &x2);
            for (int x = x1; x < x2; ++x) {
                if (x < 0 || x >= surface.width) continue;
                const int y = y2 + dy * (x - x2Original) / dx;
                if (y < 0 || y >= surface.height) continue;

                uint8_t* pixel = surface.pixels + (y * surface.width + x) * bpp;

                if (color.a == 255) {
                    SetPixel(pixel, pixelColor, bpp);
                }
                else {
                    const Color surfColor = PixelToColor(surface.format, GetPixelValue(pixel, bpp));
                    const uint32_t blendedPixel = ColorToPixel(surface.format, BlendColors(color, surfColor));
                    SetPixel(pixel, blendedPixel, bpp);
                }
            }
        }
        else {  // slope greater than 1
            const int y2Original = y2;
            if (y1 > y2) SwapInts(&y1, &y2);
            for (int y = y1; y < y2; ++y) {
                if (y < 0 || y >= surface.height) continue;
                const int x = x2 + dx * (y - y2Original) / dy;
                if (x < 0 || x >= surface.width) continue;

                uint8_t* pixel = surface.pixels + (y * surface.width + x) * bpp;

                if (color.a == 255) {
                    SetPixel(pixel, pixelColor, bpp);
                }
                else {
                    const Color surfColor = PixelToColor(surface.format, GetPixelValue(pixel, bpp));
                    const uint32_t blendedPixel = ColorToPixel(surface.format, BlendColors(color, surfColor));
                    SetPixel(pixel, blendedPixel, bpp);
                }
            }
        }
    }
    else {  // vertical line
        const int x = x2;
        if (x < 0 || x >= surface.width) return;
        if (y1 > y2) SwapInts(&y1, &y2);
        for (int y = y1; y < y2; ++y) {
            if (y < 0 || y >= surface.height) continue;

            uint8_t* pixel = surface.pixels + (y * surface.width + x) * bpp;
            SetPixel(pixel, pixelColor, bpp);
        }
    }
}
