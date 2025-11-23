#ifndef LGL_SURFACE_H
#define LGL_SURFACE_H

#include "PixelFormat.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct {
    void* pixels;
    int width;
    int height;
    const PixelFormat* format;
} Surface;

Surface SurfaceCreate(int width, int height, const PixelFormat* format);
Surface SurfaceCreateFromBuffer(int width, int height, const PixelFormat* format, void* buffer);
void SurfaceDestroy(Surface* surface);
Surface SurfaceCopy(Surface src);
Surface SurfaceConvert(Surface surface, const PixelFormat* format);
void SurfaceFill(Surface surface, Color color);
void SurfaceBlit(Surface dest, Surface src, int x, int y);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif // LGL_SURFACE_H
