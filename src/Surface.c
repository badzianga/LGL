#include <string.h>

#include "Allocator.h"
#include "Color.h"
#include "Error.h"
#include "FillRect.h"
#include "PixelFormat.h"
#include "Surface.h"

Surface SurfaceCreate(int width, int height, const PixelFormat* format) {
    if (width <= 0 || height <= 0 || format == NULL) {
        THROW_ERROR(ERR_INVALID_PARAMS);
        return (Surface){ 0 };
    }

    const int stride = width * format->bytesPerPixel;
    void* pixels = AllocatorAlloc(stride * height);
    if (pixels == NULL) {
        THROW_ERROR(ERR_OUT_OF_MEMORY);
        return (Surface){ 0 };
    }
    memset(pixels, 0, stride * height);

    SurfaceFlags flags = SURFACE_FLAG_NONE;
    if (format->aMask != 0) {
        flags |= SURFACE_FLAG_HAS_ALPHA;
    }

    return (Surface){ width, height, pixels, stride, flags, format };
}

Surface SurfaceCreateFromBuffer(int width, int height, const PixelFormat* format, void* buffer) {
    if (width <= 0 || height <= 0 || format == NULL || buffer == NULL) {
        THROW_ERROR(ERR_INVALID_PARAMS);
        return (Surface){ 0 };
    }

    const int stride = width * format->bytesPerPixel;

    SurfaceFlags flags = SURFACE_FLAG_PREALLOCATED;
    if (format->aMask != 0) {
        // analyze buffer to set proper flags
        for (int i = 0; i < width * height; ++i) {
            // for now, only 4-byte pixel format with alpha is suppoerted, so uint32_t can be safely used
            const uint32_t pixel = ((uint32_t*)buffer)[i];
            const Color pixelColor = PixelToColor(format, pixel);
            if (pixelColor.a < 255) {
                flags |= SURFACE_FLAG_HAS_ALPHA;
                break;
            }
        }
    }

    return (Surface){ width, height, buffer, stride, flags, format };
}

void SurfaceDestroy(Surface* surface) {
    if (surface == NULL || (surface->flags & SURFACE_FLAG_PREALLOCATED)) return;
    AllocatorFree(surface->pixels);
    *surface = (Surface){ 0 };
}

Surface SurfaceCopy(Surface src) {
    if (src.pixels == NULL || src.format == NULL) {
        THROW_ERROR(ERR_INVALID_PARAMS);
        return (Surface){ 0 };
    }
    Surface copy = SurfaceCreate(src.width, src.height, src.format);
    memcpy(copy.pixels, src.pixels, src.stride * src.height);
    copy.flags &= ~SURFACE_FLAG_PREALLOCATED;
    return copy;
}

Surface SurfaceConvert(Surface surface, const PixelFormat* format) {
    if (surface.pixels == NULL || surface.format == NULL || format == NULL) {
        THROW_ERROR(ERR_INVALID_PARAMS);
        return (Surface){ 0 };
    }

    if (surface.format == format) {
        Surface copy = SurfaceCopy(surface);
        copy.flags &= ~SURFACE_FLAG_PREALLOCATED;
        return copy;
    }

    Surface converted = SurfaceCreate(surface.width, surface.height, format);
    if (surface.format->aMask != 0 && converted.format->aMask == 0) {
        // formats with bpp < 4 cannot have alpha channel, so alpha flags should be removed
        if (converted.flags & SURFACE_FLAG_HAS_ALPHA) {
            converted.flags &= ~SURFACE_FLAG_HAS_ALPHA;
        }
    }
    converted.flags &= ~SURFACE_FLAG_PREALLOCATED;

    // SurfaceBlit can convert formats on the fly, so it is used here to avoid code duplication
    SurfaceBlit(converted, surface, 0, 0);

    return converted;
}

void SurfaceFill(Surface surface, Color color) {
    const Rect rect = { 0, 0, surface.width, surface.height };
    if (color.a == 0) return;
    if (color.a == 255) {
        FillRect(surface, &rect, ColorToPixel(surface.format, color));
    }
    else {
        BlendFillRect(surface, &rect, color);
    }
}

static void BlitSameFormat(Surface dest, Surface src, int x, int y) {
    const int bpp = src.format->bytesPerPixel;

    const Rect destRect = { 0, 0, dest.width, dest.height };
    const Rect srcRect = { x, y, src.width, src.height };
    Rect clipped;
    if (!RectIntersection(&srcRect, &destRect, &clipped)) return;

    int h = clipped.height;
    const int w = clipped.width * bpp;
    uint8_t* dstRow = (uint8_t*)dest.pixels + clipped.y * dest.stride + clipped.x * bpp;
    const uint8_t* srcRow = (uint8_t*)src.pixels + (clipped.y - y) * src.stride + (clipped.x - x) * bpp;
    while (h--) {
        memcpy(dstRow, srcRow, w);
        dstRow += dest.stride;
        srcRow += src.stride;
    }
}

static void BlitSameFormatA(Surface dest, Surface src, int x, int y) {
    const int bpp = src.format->bytesPerPixel;

    for (int sy = 0; sy < src.height; ++sy) {
        const int dy = y + sy;
        if (dy < 0 || dy >= dest.height) continue;
        for (int sx = 0; sx < src.width; ++sx) {
            const int dx = x + sx;
            if (dx < 0 || dx >= dest.width) continue;
            uint32_t srcPixel = 0;
            Color dstColor = { 0 };
            switch (bpp) {
                case 1: {
                    srcPixel = *((uint8_t*)src.pixels + (sy * src.width + sx));
                    dstColor = PixelToColor(src.format, *((uint8_t*)dest.pixels + (dy * dest.width + dx)));
                } break;
                case 2: {
                    srcPixel = *((uint16_t*)src.pixels + (sy * src.width + sx));
                    dstColor = PixelToColor(src.format, *((uint16_t*)dest.pixels + (dy * dest.width + dx)));
                } break;
                case 4: {
                    srcPixel = *((uint32_t*)src.pixels + (sy * src.width + sx));
                    dstColor = PixelToColor(src.format, *((uint32_t*)dest.pixels + (dy * dest.width + dx)));
                } break;
                default: break;
            }
            const Color srcColor = PixelToColor(src.format, srcPixel);
            if (srcColor.a == 0) continue;

            const uint8_t a = srcColor.a;
            uint32_t outColor = 0;
            if (a != 255) {
                const uint8_t invA = 255 - a;

                const Color out = {
                    .r = (srcColor.r * a + dstColor.r * invA) / 255,
                    .g = (srcColor.g * a + dstColor.g * invA) / 255,
                    .b = (srcColor.b * a + dstColor.b * invA) / 255,
                    .a = (srcColor.a + dstColor.a * (255 - srcColor.a) / 255)
                };
                outColor = ColorToPixel(src.format, out);
            }
            else {
                outColor = ColorToPixel(src.format, srcColor);
            }

            uint8_t* destPixel = (uint8_t*)dest.pixels + dy * dest.stride + dx * bpp;

            for (int b = 0; b < bpp; ++b) {
                destPixel[b] = ((uint8_t*)&outColor)[b];
            }
        }
    }
}

static void BlitDifferentFormat(Surface dest, Surface src, int x, int y) {
    const int srcBpp = src.format->bytesPerPixel;
    const int destBpp = dest.format->bytesPerPixel;

    for (int sy = 0; sy < src.height; ++sy) {
        const int dy = y + sy;
        if (dy < 0 || dy >= dest.height) continue;
        for (int sx = 0; sx < src.width; ++sx) {
            const int dx = x + sx;
            if (dx < 0 || dx >= dest.width) continue;

            const uint8_t* srcPixel = (uint8_t*)src.pixels + sy * src.stride + sx * srcBpp;
            uint32_t srcPixelValue = 0;
            switch (srcBpp) {
                case 1: {
                    srcPixelValue = *srcPixel;
                } break;
                case 2: {
                    srcPixelValue = *(uint16_t*)srcPixel;
                } break;
                case 4: {
                    srcPixelValue = *(uint32_t*)srcPixel;
                } break;
                default: break;
            }
            const uint32_t newValue = ColorToPixel(dest.format, PixelToColor(src.format, srcPixelValue));

            uint8_t* destPixel = (uint8_t*)dest.pixels + dy * dest.stride + dx * destBpp;

            for (int b = 0; b < destBpp; ++b) {
                destPixel[b] = ((uint8_t*)&newValue)[b];
            }
        }
    }
}

static void BlitDifferentFormatA(Surface dest, Surface src, int x, int y) {
    const int srcBpp = src.format->bytesPerPixel;
    const int destBpp = dest.format->bytesPerPixel;

    for (int sy = 0; sy < src.height; ++sy) {
        const int dy = y + sy;
        if (dy < 0 || dy >= dest.height) continue;
        for (int sx = 0; sx < src.width; ++sx) {
            const int dx = x + sx;
            if (dx < 0 || dx >= dest.width) continue;

            const uint8_t* srcPixel = (uint8_t*)src.pixels + sy * src.stride + sx * srcBpp;
            uint32_t srcPixelValue = 0;
            switch (srcBpp) {
                case 1: {
                    srcPixelValue = *srcPixel;
                } break;
                case 2: {
                    srcPixelValue = *(uint16_t*)srcPixel;
                } break;
                case 4: {
                    srcPixelValue = *(uint32_t*)srcPixel;
                } break;
                default: break;
            }
            const Color srcColor = PixelToColor(src.format, srcPixelValue);
            if (srcColor.a == 0) continue;

            uint8_t* destPixel = (uint8_t*)dest.pixels + dy * dest.stride + dx * destBpp;
            Color destColor = { 0 };
            switch (destBpp) {
                case 1: {
                    destColor = PixelToColor(dest.format, *destPixel);
                } break;
                case 2: {
                    destColor = PixelToColor(dest.format, *(uint16_t*)destPixel);
                } break;
                case 4: {
                    destColor = PixelToColor(dest.format, *(uint32_t*)destPixel);
                } break;
                default: break;
            }

            const uint8_t a = srcColor.a;
            uint32_t outColor = 0;
            if (a != 255) {
                const uint8_t invA = 255 - a;

                const Color out = {
                    .r = (srcColor.r * a + destColor.r * invA) / 255,
                    .g = (srcColor.g * a + destColor.g * invA) / 255,
                    .b = (srcColor.b * a + destColor.b * invA) / 255,
                    .a = (srcColor.a + destColor.a * (255 - srcColor.a) / 255)
                };
                outColor = ColorToPixel(dest.format, out);
            }
            else {
                outColor = ColorToPixel(dest.format, srcColor);
            }

            uint8_t* dstPixel = (uint8_t*)dest.pixels + dy * dest.stride + dx * destBpp;

            for (int b = 0; b < destBpp; ++b) {
                dstPixel[b] = ((uint8_t*)&outColor)[b];
            }
        }
    }
}

void SurfaceBlit(Surface dest, Surface src, int x, int y) {
    if (dest.format == src.format) {
        if (src.flags & SURFACE_FLAG_HAS_ALPHA)
            BlitSameFormatA(dest, src, x, y);
        else
            BlitSameFormat(dest, src, x, y);
    }
    else {
        if (src.format->aMask != 0)
            BlitDifferentFormatA(dest, src, x, y);
        else
            BlitDifferentFormat(dest, src, x, y);
    }
}

void SurfaceSetColorKey(Surface* surface, Color color) {
    const uint8_t r = color.r;
    const uint8_t g = color.g;
    const uint8_t b = color.b;

    surface->flags &= 0x000000FF;  // clear color key components, but leave flags
    surface->flags |= SURFACE_FLAG_HAS_COLOR_KEY;
    surface->flags |= SURFACE_FLAG_HAS_ALPHA;

    surface->flags |= r << 24;
    surface->flags |= g << 16;
    surface->flags |= b << 8;
}

Color SurfaceGetColorKey(Surface surface) {
    if (!(surface.flags & SURFACE_FLAG_HAS_COLOR_KEY)) return (Color){ 0 };
    return (Color){
        .r = (surface.flags & 0xFF000000) >> 24,
        .g = (surface.flags & 0x00FF0000) >> 16,
        .b = (surface.flags & 0x0000FF00) >> 8,
        .a = 0
    };
}

void SurfaceUnsetColorKey(Surface* surface) {
    surface->flags &= 0x000000FF;
    surface->flags &= ~SURFACE_FLAG_HAS_COLOR_KEY;
}
