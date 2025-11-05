#include <stdlib.h>
#include <string.h>
#include "surface.h"
#include "color.h"

Surface SurfaceCreate(int width, int height, const PixelFormat* format) {
    if (width <= 0 || height <= 0) {
        return (Surface){ 0 };
    }

    void* pixels = malloc(width * height * format->bytesPerPixel);
    if (pixels == NULL) {
        return (Surface){ 0 };
    }

    return (Surface){ pixels, width, height, width, format };
}

Surface SurfaceCreateFromBuffer(int width, int height, void* buffer, const PixelFormat* format) {
    return (Surface){ buffer, width, height, width, format };
}

Surface SurfaceCopy(Surface src) {
    Surface copy = SurfaceCreate(src.width, src.height, src.format);
    memcpy(copy.pixels, src.pixels, sizeof(src.format->bytesPerPixel) * src.width * src.height);
    return copy;
}

void SurfaceDestroy(Surface surface) {
    free(surface.pixels);
}

static void Fill2(Surface surface, uint16_t color) {
    for (int y = 0; y < surface.height; ++y) {
        for (int x = 0; x < surface.width; ++x) {
            uint16_t* pixel = &((uint16_t*)surface.pixels)[y * surface.stride + x];
            *pixel = color;
        }
    }
}

static void Fill4(Surface surface, uint32_t color) {
    for (int y = 0; y < surface.height; ++y) {
        for (int x = 0; x < surface.width; ++x) {
            uint32_t* pixel = &((uint32_t*)surface.pixels)[y * surface.stride + x];
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
