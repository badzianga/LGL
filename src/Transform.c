#include <math.h>
#include <stdlib.h>

#include "Transform.h"

void TransformFlipX(Surface surface) {
    const uint8_t bpp = surface.format->bytesPerPixel;

    for (int y = 0; y < surface.height; y++) {
        for (int x = 0; x < surface.width / 2; x++) {
            const int xEnd = surface.width - x - 1;

            uint8_t* left = (uint8_t*)surface.pixels + (y * surface.width + x) * bpp;
            uint8_t* right = (uint8_t*)surface.pixels + (y * surface.width + xEnd) * bpp;

            for (int b = 0; b < bpp; ++b) {
                const uint8_t tmp = left[b];
                left[b] = right[b];
                right[b] = tmp;
            }
        }
    }
}

void TransformFlipY(Surface surface) {
    const uint8_t bpp = surface.format->bytesPerPixel;

    for (int y = 0; y < surface.height / 2; y++) {
        const int yEnd = surface.height - y - 1;

        for (int x = 0; x < surface.width; x++) {
            uint8_t* top = (uint8_t*)surface.pixels + (y * surface.width + x) * bpp;
            uint8_t* bottom = (uint8_t*)surface.pixels + (yEnd * surface.width + x) * bpp;

            for (int b = 0; b < bpp; ++b) {
                const uint8_t tmp = top[b];
                top[b] = bottom[b];
                bottom[b] = tmp;
            }
        }
    }
}

Surface TransformRotate(Surface src, float angle) {
    const uint8_t bpp = src.format->bytesPerPixel;

    float rad = fmodf(angle, 360.f);
    if (rad < 0) rad += 360.f;
    rad = rad * (float)M_PI / 180.f;

    const float cosAlpha = cosf(rad);
    const float sinAlpha = sinf(rad);

    const int w = src.width;
    const int h = src.height;

    const float hw = (float)w * 0.5f;
    const float hh = (float)h * 0.5f;

    const float corners[4][2] = {
        {-hw, -hh},
        { hw, -hh},
        { hw,  hh},
        {-hw,  hh}
    };

    float minX =  1e30f, minY =  1e30f;
    float maxX = -1e30f, maxY = -1e30f;

    for (int i = 0; i < 4; i++) {
        const float x = corners[i][0];
        const float y = corners[i][1];
        const float rx = x * cosAlpha - y * sinAlpha;
        const float ry = x * sinAlpha + y * cosAlpha;

        if (rx < minX) minX = rx;
        if (rx > maxX) maxX = rx;
        if (ry < minY) minY = ry;
        if (ry > maxY) maxY = ry;
    }

    const int newW = (int)ceilf(maxX - minX);
    const int newH = (int)ceilf(maxY - minY);

    Surface dst = SurfaceCreate(newW, newH, src.format);

    const float halfNewW = (float)newW * 0.5f;
    const float halfNewH = (float)newH * 0.5f;

    for (int y = 0; y < newH; y++) {
        for (int x = 0; x < newW; x++) {
            const float dx = (float)x - halfNewW;
            const float dy = (float)y - halfNewH;

            const float sx =  dx * cosAlpha + dy * sinAlpha + hw;
            const float sy = -dx * sinAlpha + dy * cosAlpha + hh;

            const int isx = (int)floorf(sx + 0.5f);
            const int isy = (int)floorf(sy + 0.5f);

            if (isx < 0 || isx >= w || isy < 0 || isy >= h) continue;

            uint8_t* dstPixel = (uint8_t*)dst.pixels + (y * dst.width + x) * bpp;
            const uint8_t* srcPixel = (uint8_t*)src.pixels + (isy * src.width + isx) * bpp;

            for (int b = 0; b < bpp; ++b) {
                dstPixel[b] = srcPixel[b];
            }
        }
    }

    return dst;
}

Surface TransformScale(Surface src, int destWidth, int destHeight) {
    Surface dest = SurfaceCreate(destWidth, destHeight, src.format);
    const uint8_t bpp = src.format->bytesPerPixel;

    int srcY = 0;
    int accY = 0;

    for (int y = 0; y < destHeight; ++y) {
        accY += src.height;
        while (accY >= dest.height) {
            accY -= dest.height;
            ++srcY;
            if (srcY >= src.height) srcY = src.height - 1;
        }

        int srcX = 0;
        int accX = 0;

        for (int x = 0; x < dest.width; ++x) {
            accX += src.width;
            while (accX >= dest.width) {
                accX -= dest.width;
                ++srcX;
                if (srcX >= src.width) srcX = src.width - 1;
            }

            uint8_t* destPixel = (uint8_t*)dest.pixels + (y * destWidth + x) * bpp;
            const uint8_t* srcPixel = (uint8_t*)src.pixels + (srcY * src.width + srcX) * bpp;

            for (int b = 0; b < bpp; ++b) {
                destPixel[b] = srcPixel[b];
            }
        }
    }

    return dest;
}

// TODO: avoid using sColorArray, write pixels to scaled surface in calculation iteration
// TODO: separate calculating borders to avoid unnecessary checking for borders in every iteration
Surface TransformScale2x(Surface original) {
    const PixelFormat* const fmt = original.format;
    const int bpp = fmt->bytesPerPixel;

    const Surface scaled = SurfaceCreate(original.width * 2, original.height * 2, fmt);

    Color* oColorArray = malloc(sizeof(Color) * original.width * original.height);
    Color* sColorArray = malloc(sizeof(Color) * scaled.width * scaled.height);

    for (int i = 0; i < original.width * original.height; i++) {
        const uint8_t* pixel = original.pixels + i * bpp;
        switch (bpp) {
            case 1: {
                oColorArray[i] = PixelToColor(fmt, *pixel);
            } break;
            case 2: {
                oColorArray[i] = PixelToColor(fmt, *(uint16_t*)pixel);
            } break;
            case 4: {
                oColorArray[i] = PixelToColor(fmt, *(uint32_t*)pixel);
            } break;
            default: break;
        }
    }

    for (int y = 0; y < original.height; y++) {
        for (int x = 0; x < original.width; x++) {
            const int upperY = y - 1 < 0 ? 0 : y - 1;
            const int lowerY = y + 1 >= original.height ? original.height - 1 : y + 1;
            const int leftX = x - 1 < 0 ? 0 : x - 1;
            const int rightX = x + 1 >= original.width ? original.width - 1 : x + 1;

            const Color B = oColorArray[upperY * original.width +      x];
            const Color D = oColorArray[     y * original.width +  leftX];
            const Color E = oColorArray[     y * original.width +      x];
            const Color F = oColorArray[     y * original.width + rightX];
            const Color H = oColorArray[lowerY * original.width +      x];

            Color E0, E1, E2, E3;

            #define EQ(color1, color2) (*(uint32_t*)&(color1) == *(uint32_t*)&(color2))

            if (!EQ(B, H) && !EQ(D, F)) {
                E0 = EQ(D, B) ? D : E;
                E1 = EQ(B, F) ? F : E;
                E2 = EQ(D, H) ? D : E;
                E3 = EQ(H, F) ? F : E;
            }
            else {
                E0 = E;
                E1 = E;
                E2 = E;
                E3 = E;
            }

            sColorArray[((2 * y) + 0) * scaled.width + ((2 * x) + 0)] = E0;
            sColorArray[((2 * y) + 0) * scaled.width + ((2 * x) + 1)] = E1;
            sColorArray[((2 * y) + 1) * scaled.width + ((2 * x) + 0)] = E2;
            sColorArray[((2 * y) + 1) * scaled.width + ((2 * x) + 1)] = E3;
        }
    }

    for (int i = 0; i < scaled.width * scaled.height; i++) {
        uint8_t* pixel = scaled.pixels + i * bpp;
        switch (bpp) {
            case 1: {
                *pixel = ColorToPixel(fmt, sColorArray[i]);
            } break;
            case 2: {
                *(uint16_t*)pixel = ColorToPixel(fmt, sColorArray[i]);
            } break;
            case 4: {
                *(uint32_t*)pixel = ColorToPixel(fmt, sColorArray[i]);
            } break;
            default: break;
        }
    }

    free(oColorArray);
    free(sColorArray);

    return scaled;
}
