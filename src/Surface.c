#include <stdlib.h>
#include <string.h>

#include "Allocator.h"
#include "Color.h"
#include "FillRect.h"
#include "PixelFormat.h"
#include "Surface.h"

Surface SurfaceCreate(int width, int height, const PixelFormat* format) {
    if (width <= 0 || height <= 0 || format == NULL) {
        return (Surface){ 0 };
    }

    const int stride = width * format->bytesPerPixel;
    void* pixels = AllocatorAlloc(stride * height);
    if (pixels == NULL) {
        return (Surface){ 0 };
    }
    memset(pixels, 0, stride * height);

    return (Surface){ width, height, pixels, stride, 0, format };
}

Surface SurfaceCreateFromBuffer(int width, int height, const PixelFormat* format, void* buffer) {
    if (width <= 0 || height <= 0 || format == NULL || buffer == NULL) {
        return (Surface){ 0 };
    }

    const int stride = width * format->bytesPerPixel;

    return (Surface){ width, height, buffer, stride, 0, format };
}

void SurfaceDestroy(Surface* surface) {
    AllocatorFree(surface->pixels);
    *surface = (Surface){ 0 };
}

Surface SurfaceCopy(Surface src) {
    Surface copy = SurfaceCreate(src.width, src.height, src.format);
    memcpy(copy.pixels, src.pixels, src.stride * src.height);
    return copy;
}

Surface SurfaceConvert(Surface surface, const PixelFormat* format) {
    if (surface.format == NULL || format == NULL) {
        return (Surface){ 0 };
    }

    if (surface.format == format) {
        return SurfaceCopy(surface);
    }

    Surface new = SurfaceCreate(surface.width, surface.height, format);

    // TODO: use SurfaceBlit here

    const uint8_t srcBpp = surface.format->bytesPerPixel;
    const uint8_t dstBpp = new.format->bytesPerPixel;

    for (int y = 0; y < surface.height; ++y) {
        for (int x = 0; x < surface.width; ++x) {
            const void* fromPtr = (uint8_t*)surface.pixels + y * surface.stride + x * srcBpp;
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
                default: break;
            }

            Color color = PixelToColor(surface.format, fromPixel);

            const void* toPtr = (uint8_t*)new.pixels + y * new.stride + x * dstBpp;
            const uint32_t toPixel = ColorToPixel(new.format, color);
            switch (dstBpp) {
                case 1: {
                    *(uint8_t*)toPtr = (uint8_t)toPixel;
                } break;
                case 2: {
                    *(uint16_t*)toPtr = (uint16_t)toPixel;
                } break;
                case 4: {
                    *(uint32_t*)toPtr = toPixel;
                } break;
                default: break;
            }
        }
    }
    return new;
}

void SurfaceFill(Surface surface, Color color) {
    const Rect rect = { 0, 0, surface.width, surface.height };
    // TODO: check if blending is necessary, then call BlendFillRect()
    FillRect(surface, &rect, ColorToPixel(surface.format, color));
}


static void BlitSameFormat(Surface dest, Surface src, int x, int y) {
    const int bpp = src.format->bytesPerPixel;

    for (int sy = 0; sy < src.height; ++sy) {
        const int dy = y + sy;
        if (dy < 0 || dy >= dest.height) continue;
        for (int sx = 0; sx < src.width; ++sx) {
            const int dx = x + sx;
            if (dx < 0 || dx >= dest.width) continue;
            const uint8_t* srcPixel = (uint8_t*)src.pixels + sy * src.stride + sx * bpp;
            uint8_t* destPixel = (uint8_t*)dest.pixels + dy * dest.stride + dx * bpp;

            for (int b = 0; b < bpp; ++b) {
                destPixel[b] = srcPixel[b];
            }
        }
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
                    .a = 255
                    // .a = (srcColor.a + dstColor.a * (255 - srcColor.a) / 255)
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
                    .a = 255
                    // .a = (srcColor.a + destColor.a * (255 - srcColor.a) / 255)
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
        if (dest.format->aMask != 0) {
            BlitSameFormatA(dest, src, x, y);
        }
        else {
            BlitSameFormat(dest, src, x, y);
        }
    }
    else {
        if (src.format->aMask != 0) {
            BlitDifferentFormatA(dest, src, x, y);
        }
        else {
            BlitDifferentFormat(dest, src, x, y);
        }
    }
}
