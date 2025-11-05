#pragma once
#include "Color.h"
#include "PixelFormat.h"

typedef struct {
    void* pixels;
    int width;
    int height;
    const PixelFormat* format;
} Surface;

Surface SurfaceCreate(int width, int height, const PixelFormat* format);
void SurfaceDestroy(Surface* surface);
Surface SurfaceCopy(Surface src);
Surface SurfaceConvert(Surface surface, const PixelFormat* format);
void SurfaceFill(Surface surface, Color color);
void SurfaceBlit(Surface dest, Surface src, int x, int y);
