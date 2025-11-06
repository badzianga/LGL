#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "Surface.h"
#include "Color.h"
#include "PixelFormat.h"

Surface SurfaceCreate(int width, int height, const PixelFormat* format) {
    if (width <= 0 || height <= 0 || format == NULL) {
        return (Surface){ 0 };
    }

    void* pixels = malloc(width * height * format->bytesPerPixel);
    if (pixels == NULL) {
        return (Surface){ 0 };
    }

    return (Surface){ pixels, width, height, format };
}

void SurfaceDestroy(Surface* surface) {
    free(surface->pixels);
    *surface = (Surface){ 0 };
}

Surface SurfaceCopy(Surface src) {
    Surface copy = SurfaceCreate(src.width, src.height, src.format);
    memcpy(copy.pixels, src.pixels, src.format->bytesPerPixel * src.width * src.height);
    return copy;
}

Surface SurfaceMakeConverted(Surface surface, const PixelFormat* format) {
    if (surface.format == NULL || format == NULL) {
        return (Surface){ 0 };
    }

    if (surface.format == format) {
        return SurfaceCopy(surface);
    }

    Surface new = SurfaceCreate(surface.width, surface.height, format);

    const uint8_t srcBpp = surface.format->bytesPerPixel;
    const uint8_t dstBpp = new.format->bytesPerPixel;

    for (int y = 0; y < surface.height; ++y) {
        for (int x = 0; x < surface.width; ++x) {
            const void* fromPtr = surface.pixels + (y * surface.width + x) * srcBpp;
            uint32_t fromPixel = 0;
            switch (srcBpp) {
                case 1: {
                    fromPixel = *(uint8_t*)fromPtr;
                } break;
                case 2: {
                    fromPixel = *(uint16_t*)fromPtr;
                } break;
                case 4: {
                    fromPixel = *(uint32_t*)fromPtr;
                } break;
            }

            Color color = PixelToColor(surface.format, fromPixel);

            const void* toPtr = new.pixels + (y * new.width + x) * dstBpp;
            uint32_t toPixel = ColorToPixel(new.format, color);
            switch (dstBpp) {
                case 1: {
                    *(uint8_t*)toPtr = (uint8_t)toPixel;
                } break;
                case 2: {
                    *(uint16_t*)toPtr = (uint16_t)toPixel;
                } break;
                case 4: {
                    *(uint32_t*)toPtr = (uint32_t)toPixel;
                } break;
            }
        }
    }
    return new;
}

static void Fill2(Surface surface, uint16_t color) {
    for (int y = 0; y < surface.height; ++y) {
        for (int x = 0; x < surface.width; ++x) {
            uint16_t* pixel = &((uint16_t*)surface.pixels)[y * surface.width + x];
            *pixel = color;
        }
    }
}

static void Fill4(Surface surface, uint32_t color) {
    for (int y = 0; y < surface.height; ++y) {
        for (int x = 0; x < surface.width; ++x) {
            uint32_t* pixel = &((uint32_t*)surface.pixels)[y * surface.width + x];
            *pixel = color;
        }
    }
}

void SurfaceFill(Surface surface, Color color) {
    if (surface.format->bytesPerPixel == 2) {
        Fill2(surface, ColorToPixel(surface.format, color));
    }
    else if (surface.format->bytesPerPixel == 4) {
        Fill4(surface, ColorToPixel(surface.format, color));
    }
}

void SurfaceBlit(Surface dest, Surface src, int x, int y) {
    assert(dest.format == src.format
           && "Surfaces must be in the same pixel format");
    assert(dest.format->bytesPerPixel == 4 && src.format->bytesPerPixel == 4
           && "TODO: only surfaces with 4 bytes per pixel are supported for now");

    for (int h = 0; h < src.height; ++h) {
        int yi = y + h;
        if (yi < 0 || yi >= dest.height) continue;
        for (int w = 0; w < src.width; ++w) {
            int xi = x + w;
            if (xi < 0 || xi >= dest.width) continue;
            uint32_t* destPixel = &((uint32_t*)dest.pixels)[yi * dest.width + xi];
            const uint32_t srcPixel = ((uint32_t*)src.pixels)[h * src.width + x];
            *destPixel = srcPixel;
        }
    }
}
