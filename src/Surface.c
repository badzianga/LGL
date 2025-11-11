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
            const void* fromPtr = (uint8_t*)surface.pixels + (y * surface.width + x) * srcBpp;
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

            const void* toPtr = (uint8_t*)new.pixels + (y * new.width + x) * dstBpp;
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

static void BlitSameFormat(Surface dest, Surface src, int x, int y) {
    const int bpp = src.format->bytesPerPixel;

    for (int sy = 0; sy < src.height; ++sy) {
        const int dy = y + sy;
        if (dy < 0 || dy >= dest.height) continue;
        for (int sx = 0; sx < src.width; ++sx) {
            const int dx = x + sx;
            if (dx < 0 || dx >= dest.width) continue;
            const uint8_t* srcPixel = (uint8_t*)src.pixels + (sy * src.width  + sx) * bpp;
            uint8_t* destPixel = (uint8_t*)dest.pixels + (dy * dest.width + dx) * bpp;

            for (int b = 0; b < bpp; ++b) {
                destPixel[b] = srcPixel[b];
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

            const uint8_t* srcPixel = (uint8_t*)src.pixels + (sy * src.width  + sx) * srcBpp;
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
            }
            const uint32_t newValue = ColorToPixel(dest.format, PixelToColor(src.format, srcPixelValue));

            uint8_t* destPixel = (uint8_t*)dest.pixels + (dy * dest.width + dx) * destBpp;

            for (int b = 0; b < destBpp; ++b) {
                destPixel[b] = ((uint8_t*)&newValue)[b];
            }
        }
    }
}

void SurfaceBlit(Surface dest, Surface src, int x, int y) {
    if (dest.format == src.format) {
        BlitSameFormat(dest, src, x, y);
    }
    else {
        BlitDifferentFormat(dest, src, x, y);
    }
}
