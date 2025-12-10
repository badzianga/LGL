#include <stdlib.h>

#include "Draw.h"
#include "FillRect.h"
#include "internal/Inlines.h"

static inline void SetPixel(uint8_t* pixel, uint32_t color, uint8_t bpp) {
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

static void FillHLine(Surface surface, int y, int x0, int x1, uint32_t color) {
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
            int rest = w & 3;
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

static void BlendFillHLine(Surface surface, int y, int x0, int x1, Color color) {
    if (y < 0 || y >= surface.height) return;

    if (x0 < 0) x0 = 0;
    if (x1 > surface.width) x1 = surface.width;
    if (x0 >= x1) return;

    const uint8_t bpp = surface.format->bytesPerPixel;
    const uint8_t a = color.a;
    const uint8_t invA = 255 - a;

    uint8_t* row = (uint8_t*)surface.pixels + y * surface.stride + x0 * bpp;
    int n = x1 - x0;
    switch (bpp) {
        case 1: {
            uint8_t* pixel = row;
            while (n--) {
                Color c = PixelToColor(surface.format, *pixel);
                c = BlendColors(color, c, a, invA);
                *pixel++ = (uint8_t)ColorToPixel(surface.format, c);
            }
        } break;
        case 2: {
            uint16_t* pixel = (uint16_t*)row;
            while (n--) {
                Color c = PixelToColor(surface.format, *pixel);
                c = BlendColors(color, c, a, invA);
                *pixel++ = (uint16_t)ColorToPixel(surface.format, c);
            }
        } break;
        case 4: {
            uint32_t* pixel = (uint32_t*)row;
            while (n--) {
                Color c = PixelToColor(surface.format, *pixel);
                c = BlendColors(color, c, a, invA);
                *pixel++ = ColorToPixel(surface.format, c);
            }
        } break;
        default: break;
    }
}

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

    while (x >= y) {
        int startX = cx - x;
        int endX = cx + x;
        BlendFillHLine(surface, cy + y, startX, endX, color);
        if (y != 0) {
            BlendFillHLine(surface, cy - y, startX, endX, color);
        }
        ++y;

        if (d < 0) {
            d += (y << 1) + 1;
        }
        else {
            if (x >= y) {
                startX = cx - y + 1;
                endX = cx + y - 1;
                BlendFillHLine(surface, cy + x, startX, endX, color);
                BlendFillHLine(surface, cy - x, startX, endX, color);
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

// TODO: check if making exact copy of fill functions but for blending really enhances performance

static void FillTriangleFlatTop(Surface surface, int x1, int x2, int y1_2, int x3, int y3, uint32_t color) {
    const int dy = y3 - y1_2;

    int xl = x1 << 16;
    int xr = x2 << 16;

    const int dx1 = ((x3 - x1) << 16) / dy;
    const int dx2 = ((x3 - x2) << 16) / dy;

    for (int y = y1_2; y <= y3; ++y) {
        FillHLine(surface, y, xl >> 16, xr >> 16, color);
        xl += dx1;
        xr += dx2;
    }
}

static void FillTriangleFlatBottom(Surface surface, int x1, int y1, int x2, int x3, int y2_3, uint32_t color) {
    const int dy = y2_3 - y1;

    int xl = x1 << 16;
    int xr = x1 << 16;

    const int dx1 = ((x2 - x1) << 16) / dy;
    const int dx2 = ((x3 - x1) << 16) / dy;

    for (int y = y1; y <= y2_3; ++y) {
        FillHLine(surface, y, xl >> 16, xr >> 16, color);
        xl += dx1;
        xr += dx2;
    }
}

static void BlendTriangleFlatTop(Surface surface, int x1, int x2, int y1_2, int x3, int y3, Color color) {
    const int dy = y3 - y1_2;

    int xl = x1 << 16;
    int xr = x2 << 16;

    const int dx1 = ((x3 - x1) << 16) / dy;
    const int dx2 = ((x3 - x2) << 16) / dy;

    for (int y = y1_2; y <= y3; ++y) {
        BlendFillHLine(surface, y, xl >> 16, xr >> 16, color);
        xl += dx1;
        xr += dx2;
    }
}

static void BlendTriangleFlatBottom(Surface surface, int x1, int y1, int x2, int x3, int y2_3, Color color) {
    const int dy = y2_3 - y1;

    int xl = x1 << 16;
    int xr = x1 << 16;

    const int dx1 = ((x2 - x1) << 16) / dy;
    const int dx2 = ((x3 - x1) << 16) / dy;

    for (int y = y1; y <= y2_3; ++y) {
        BlendFillHLine(surface, y, xl >> 16, xr >> 16, color);
        xl += dx1;
        xr += dx2;
    }
}

void DrawTriangle(Surface surface, int x1, int y1, int x2, int y2, int x3, int y3, Color color) {
    if (color.a == 0) return;
    SortTrianglePointsAscendingByY(&x1, &y1, &x2, &y2, &x3, &y3);
    const uint32_t c = ColorToPixel(surface.format, color);

    if (y2 == y3) {
        if (x2 > x3) {
            SwapInts(&x2, &x3);
        }
        if (color.a == 255) {
            FillTriangleFlatBottom(surface, x1, y1, x2, x3, y3, c);
        }
        else {
            BlendTriangleFlatBottom(surface, x1, y1, x2, x3, y3, color);
        }
    }
    else if (y1 == y2) {
        if (x1 > x2) {
            SwapInts(&x1, &x2);
        }
        if (color.a == 255) {
            FillTriangleFlatTop(surface, x1, x2, y2, x3, y3, c);
        }
        else {
            BlendTriangleFlatTop(surface, x1, x2, y2, x3, y3, color);
        }
    }
    else {
        // TODO: use fixed point arithmetic here
        int x4 = x1 + (int)(((float)(y2 - y1) / (float)(y3 - y1)) * (float)(x3 - x1));
        if (x2 > x4) SwapInts(&x4, &x2);
        if (color.a == 255) {
            FillTriangleFlatBottom(surface, x1, y1, x2, x4, y2 - 1, c);
            FillTriangleFlatTop(surface, x2, x4, y2, x3, y3, c);
        }
        else {
            BlendTriangleFlatBottom(surface, x1, y1, x2, x4, y2 - 1, color);
            BlendTriangleFlatTop(surface, x2, x4, y2, x3, y3, color);
        }
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
