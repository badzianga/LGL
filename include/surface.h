#pragma once
#include "color.h"

typedef struct {
    void* pixels;
    int width;
    int height;
    int stride;
    const PixelFormat* format;
} Surface;

Surface SurfaceCreate(int width, int height, const PixelFormat* format);
Surface SurfaceCreateFromBuffer(int width, int height, void* buffer, const PixelFormat* format);
Surface SurfaceCopy(Surface src);
void SurfaceDestroy(Surface surface);
Surface SurfaceConvert(Surface surface, const PixelFormat* format);
void SurfaceFill(Surface surface, Color color);
void SurfaceBlit(Surface dest, Surface src, int x, int y);
