#include <stdlib.h>

#include "Draw.h"
#include "FillRect.h"
#include "internal/Inlines.h"

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

static Color BlendColorz(Color c1, Color c2) {
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
    const Rect rect = { x, y, w, h };
    if (color.a == 0) return;
    if (color.a == 255) {
        FillRect(surface, &rect, ColorToPixel(surface.format, color));
    }
    else {
        BlendFillRect(surface, &rect, color);
    }
}

static inline void FillHLine(Surface surface, int y, int x0, int x1, uint32_t color) {
    if (y < 0 || y >= surface.height) return;

    if (x0 < 0) x0 = 0;
    if (x1 > surface.width) x1 = surface.width;
    if (x0 >= x1) return;

    const int bpp = surface.format->bytesPerPixel;
    const int w = x1 - x0;

    uint8_t* row = (uint8_t*)surface.pixels + y * surface.stride + x0 * bpp;

    switch (bpp) {
        case 1: {
            color |= color << 8;
            color |= color << 16;
            const int quads = w >> 2;
            const int offset = quads << 2;
            int rest  = w & 3;
            Memset4(row, color, quads);
            uint8_t* p = row + offset;
            while (rest--) {
                *p++ = (uint8_t)color;
            }
        } break;
        case 2: {
            color |= color << 16;
            const int pairs = w >> 1;
            const int offset = pairs << 2;
            const int odd = w & 1;
            Memset4(row, color, pairs);
            if (odd) {
                *(uint16_t*)(row + offset) = (uint16_t)color;
            }
        } break;
        case 4: {
            Memset4(row, color, w);
        } break;
        default: break;
    }
}

#define MAKE_BLEND_FILL_FUNCTION(TYPE, BYTES)                                                   \
static inline void BlendFillHLine##BYTES(Surface surface, int y, int x0, int x1, Color color) { \
    if (y < 0 || y >= surface.height) return;                                                   \
    if (x0 < 0) x0 = 0;                                                                         \
    if (x1 > surface.width) x1 = surface.width;                                                 \
    if (x0 >= x1) return;                                                                       \
    const uint8_t a = color.a;                                                                  \
    const uint8_t invA = 255 - a;                                                               \
    uint8_t* row = (uint8_t*)surface.pixels + y * surface.stride + x0 * BYTES;                  \
    TYPE* pixel = (TYPE*)row;                                                                          \
    int n = x1 - x0;                                                                            \
    while (n--) {                                                                               \
        Color c = PixelToColor(surface.format, *pixel);                                         \
        c = BlendColors(color, c, a, invA);                                                     \
        *pixel++ = (TYPE)ColorToPixel(surface.format, c);                                       \
    }                                                                                           \
}

MAKE_BLEND_FILL_FUNCTION(uint8_t, 1)
MAKE_BLEND_FILL_FUNCTION(uint16_t, 2)
MAKE_BLEND_FILL_FUNCTION(uint32_t, 4)

// Based on https://stackoverflow.com/questions/10878209/midpoint-circle-algorithm-for-filled-circles by colinday
static void FillCircle(Surface surface, int cx, int cy, int r, uint32_t color) {
    int x = r;
    int y = 0;
    int d = 1 - x;

    while (x >= y) {
        int startX = cx - x;
        int endX = cx + x;
        FillHLine(surface, cy + y, startX, endX, color);
        if (y != 0) {
            FillHLine(surface, cy - y, startX, endX, color);
        }
        ++y;

        if (d < 0) {
            d += (y << 1) + 1;
        }
        else {
            if (x >= y) {
                startX = cx - y + 1;
                endX = cx + y - 1;
                FillHLine(surface, cy + x, startX, endX, color);
                FillHLine(surface, cy - x, startX, endX, color);
            }
            --x;
            d += (y - x + 1) << 1;
        }
    }
}

static void BlendFillCircle(Surface surface, int cx, int cy, int r, Color color) {
    int x = r;
    int y = 0;
    int d = 1 - x;

    void (*BlendFillHLineFunction)(Surface surface, int y, int x0, int x1, Color color) = NULL;
    switch (surface.format->bytesPerPixel) {
        case 1: BlendFillHLineFunction = &BlendFillHLine1; break;
        case 2: BlendFillHLineFunction = &BlendFillHLine2; break;
        case 4: BlendFillHLineFunction = &BlendFillHLine4; break;
        default: return;
    }

    while (x >= y) {
        int startX = cx - x;
        int endX = cx + x;
        BlendFillHLineFunction(surface, cy + y, startX, endX, color);
        if (y != 0) {
            BlendFillHLineFunction(surface, cy - y, startX, endX, color);
        }
        ++y;

        if (d < 0) {
            d += (y << 1) + 1;
        }
        else {
            if (x >= y) {
                startX = cx - y + 1;
                endX = cx + y - 1;
                BlendFillHLineFunction(surface, cy + x, startX, endX, color);
                BlendFillHLineFunction(surface, cy - x, startX, endX, color);
            }
            --x;
            d += (y - x + 1) << 1;
        }
    }
}

void DrawCircle(Surface surface, int x, int y, int r, Color color) {
    if (r <= 0 || color.a == 0) return;
    if (color.a == 255) {
        FillCircle(surface, x, y, r, ColorToPixel(surface.format, color));
    }
    else {
        BlendFillCircle(surface, x, y, r, color);
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
            const uint32_t blendedPixel = ColorToPixel(surface.format, BlendColorz(color, surfColor));
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
    const uint32_t c = ColorToPixel(surface.format, color);
    const uint8_t bpp = surface.format->bytesPerPixel;

    const int dx = abs(x2 - x1);
    const int sx = x1 < x2 ? 1 : -1;
    const int dy = -abs(y2 - y1);
    const int sy = y1 < y2 ? 1 : -1;

    int err = dx + dy;

    while (x1 != x2 && y1 != y2) {
        if (x1 >= 0 && x1 < surface.width && y1 >= 0 && y1 < surface.height) {
            uint8_t* pixel = (uint8_t*)surface.pixels + y1 * surface.stride + x1 * bpp;
            SetPixel(pixel, c, bpp);
        }

        const int e2 = err << 1;

        if (e2 >= dy) {
            err += dy;
            x1 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y1 += sy;
        }
    }
}
