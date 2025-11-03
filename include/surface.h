#pragma once
#include "color.h"

typedef struct {
    Color* pixels;
    int width;
    int height;
} Surface;

Surface SurfaceCreate(int width, int height);
Surface SurfaceCreateFromBuffer(int width, int height, void* buffer);
Surface SurfaceCopy(Surface src);
void SurfaceDestroy(Surface surface);
void SurfaceFill(Surface surface, Color color);
void SurfaceBlit(Surface dest, Surface src, int x, int y);

#define SetPixel(surface, x, y, color) (surface).pixels[(y) * (surface).width + (x)] = color
