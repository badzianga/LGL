#include <stdlib.h>
#include <string.h>
#include "surface.h"

Surface SurfaceCreate(int width, int height) {
    if (width <= 0 || height <= 0) {
        width = 0;
        height = 0;
        return (Surface){ .pixels = NULL, .width = width, .height = height };
    }

    Color* pixels = malloc(sizeof(*pixels) * width * height);
    if (pixels == NULL) {
        width = 0;
        height = 0;
    }

    return (Surface){ .pixels = pixels, .width = width, .height = height };
}

Surface SurfaceCreateFromBuffer(int width, int height, void* buffer) {
    return (Surface){ .pixels = buffer, .width = width, .height = height };
}

Surface SurfaceCopy(Surface src) {
    Surface copy = SurfaceCreate(src.width, src.height);
    memcpy(copy.pixels, src.pixels, sizeof(*src.pixels) * src.width * src.height);
    return copy;
}

void SurfaceDestroy(Surface surface) {
    free(surface.pixels);
}

void SurfaceFill(Surface surface, Color color) {
    for (int y = 0; y < surface.height; ++y) {
        for (int x = 0; x < surface.width; ++x) {
            surface.pixels[y * surface.width + x] = color;
        }
    }
}

void SurfaceBlit(Surface dest, Surface src, int x, int y) {
    for (int h = 0; h < src.height; ++h) {
        int yi = y + h;
        if (yi < 0 || yi >= dest.height) continue;
        for (int w = 0; w < src.width; ++w) {
            int xi = x + w;
            if (xi < 0 || xi >= dest.width) continue;
            dest.pixels[yi * dest.width + xi] = src.pixels[h * src.width + w];
        }
    }
}
