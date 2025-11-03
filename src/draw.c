#include "draw.h"
#include "surface.h"

void DrawRect(Surface surface, int x, int y, int width, int height, Color color) {
    for (int h = 0; h < height; ++h) {
        int yi = y + h;
        if (yi < 0 || yi >= surface.height) continue;
        for (int w = 0; w < width; ++w) {
            int xi = x + w;
            if (xi < 0 || xi >= surface.width) continue;
            SetPixel(surface, xi, yi, color);
        }
    }
}

void DrawCircle(Surface surface, int cx, int cy, int radius, Color color) {
    for (int y = cy - radius; y < cy + radius; ++y) {
        if (y < 0 || y >= surface.height) continue;
        for (int x = cx - radius; y < cx + radius; ++x) {
            if (x < 0 || x >= surface.height) continue;
            SetPixel(surface, x, y, color);
        }
    }
}
