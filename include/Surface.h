#ifndef LGL_SURFACE_H
#define LGL_SURFACE_H

#include "PixelFormat.h"
#include "Rect.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef enum SurfaceFlags {
    SURFACE_FLAG_NONE          = 0,
    SURFACE_FLAG_PREALLOCATED  = (1 << 0),
    SURFACE_FLAG_HAS_ALPHA     = (1 << 1),  // any pixel has alpha < 255, so surface should be blended
    SURFACE_FLAG_HAS_COLOR_KEY = (1 << 2),
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
Surface SurfaceGetSubsurface(Surface surface, Rect rect);
Surface SurfaceGetSubsurfaceUnchecked(Surface surface, Rect rect);
void SurfaceDestroy(Surface* surface);
Surface SurfaceCopy(Surface src);
Surface SurfaceConvert(Surface surface, const PixelFormat* format);
void SurfaceFill(Surface surface, Color color);
void SurfaceBlit(Surface dest, Surface src, int x, int y);
void SurfaceSetColorKey(Surface* surface, Color color);
Color SurfaceGetColorKey(Surface surface);
void SurfaceUnsetColorKey(Surface* surface);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif // LGL_SURFACE_H
