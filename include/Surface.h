#ifndef LGL_SURFACE_H
#define LGL_SURFACE_H

#include "PixelFormat.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef enum SurfaceFlags {
    SURFACE_FLAG_NONE         = 0,
    SURFACE_FLAG_LOCKED       = (1 << 0),  // unused yet
    SURFACE_FLAG_HAS_ALPHA    = (1 << 1),
    SURFACE_FLAG_BLIT_BLENDED = (1 << 2),
} SurfaceFlags;

typedef struct Surface {
    int width;
    int height;
    void* pixels;
    int stride;
    SurfaceFlags flags;
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
