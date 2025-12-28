#ifndef LGL_SURFACE_H
#define LGL_SURFACE_H

#include "PixelFormat.h"
#include "Rect.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef enum SurfaceFlags {
    SURFACE_FLAG_NONE          =  0,        // none flags
    SURFACE_FLAG_PREALLOCATED  = (1 << 0),  // memory is not allocated with malloc
    SURFACE_FLAG_HAS_ALPHA     = (1 << 1),  // any pixel has alpha < 255
    SURFACE_FLAG_HAS_COLOR_KEY = (1 << 2),  // colorkey is defined
} SurfaceFlags;

typedef struct Surface {
    int width;
    int height;
    void* pixels;
    int stride;
    SurfaceFlags flags;
    const PixelFormat* format;
} Surface;

/**
 * Creates a new surface using global allocator. Throws critical error on fail.
 * @param width Width of the surface in pixels (must be bigger than 0).
 * @param height Height of the surface in pixels (must be bigger than 0).
 * @param format Pointer to the one of the pixel formats defined in PixelFormat.h.
 * @return Empty surface with all pixels set to 0 and flag HAS_ALPHA if pixel format has alpha channel.
 */
Surface SurfaceCreate(int width, int height, const PixelFormat* format);

/**
 * Creates a new surface using existing buffer.
 * @param width Width of the surface in pixels (must be bigger than 0).
 * @param height Height of the surface in pixels (must be bigger than 0).
 * @param format Pointer to the one of the pixel formats defined in PixelFormat.h.
 * @param buffer Pointer to the buffer.
 * @return Surface using passed buffer and flag PREALLOCATED + HAS_ALPHA (if any pixel's alpha < 255).
 */
Surface SurfaceCreateFromBuffer(int width, int height, const PixelFormat* format, void* buffer);

/**
 * Creates a subsurface defined by the given rectangle.
 * @param surface Parent surface.
 * @param rect clipping area.
 * @return Valid intersection of surface and rect with parent's flags + PREALLOCATED.
 */
Surface SurfaceGetSubsurface(Surface surface, Rect rect);

/**
 * Creates a subsurface without performing any bounds checking.
 * @param surface Parent surface.
 * @param rect Clipping area.
 * @return Region defined by the rect of the parent surface with parent's flags + PREALLOCATED.
 */
Surface SurfaceGetSubsurfaceUnchecked(Surface surface, Rect rect);

/**
 * Destroys a surface, freeing its buffer (if not PREALLOCATED) and setting all fields to 0.
 * @param surface Pointer to the surface,
 */
void SurfaceDestroy(Surface* surface);

/**
 * Creates a deep copy of the source surface using global allocator.
 * @param src Original surface.
 * @return Exact copy of the original surface, without PREALLOCATED flag.
 */
Surface SurfaceCopy(Surface src);

/**
 * Creates a new surface (using global allocator) converted to passed pixel format.
 * @param surface Original surface.
 * @param format Target format of the new surface.
 * @return Surface converted to the target format with original's flag (+ HAS_ALPHA when target format supports alpha).
 */
Surface SurfaceConvert(Surface surface, const PixelFormat* format);

/**
 * Fills whole surface with passed color.
 * @param surface Target surface.
 * @param color Fill color.
 */
void SurfaceFill(Surface surface, Color color);

/**
 * Safely draws source surface to the target surface with clipping.
 * Surfaces with different pixel formats can be used, but blit will be slower.
 * @param dest Target surface.
 * @param src Source surface.
 * @param x X coordinate of the top-left corner.
 * @param y Y coordinate of the top-left corner.
 */
void SurfaceBlit(Surface dest, Surface src, int x, int y);

/**
 * Sets a color key used for transparency, sets COLOR_KEY flag and stores color info in the surface.
 * Pixels with alpha != 255 are premultiplied by alpha. HAS_ALPHA flag is invalidated.
 * @param surface Pointer to the configured surface.
 * @param color Color key for the surface (alpha is ignored in this struct).
 */
void SurfaceSetColorKey(Surface* surface, Color color);

/**
 * Returns a color key from the surface. If COLOR_KEY flag is not present, returned color struct is empty.
 * @param surface Target surface.
 * @return Color with RGB values from color key and A = 255, or empty color if COLOR_KEY flag is not present.
 */
Color SurfaceGetColorKey(Surface surface);

/**
 * Removes color key and COLOR_KEY flag from the surface. If color key was not defined, this function does nothing.
 * @param surface Target surface.
 */
void SurfaceUnsetColorKey(Surface* surface);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif // LGL_SURFACE_H
