#include <stdlib.h>

#include "Transform.h"

#define EQ(color1, color2) (*(uint32_t*)&(color1) == *(uint32_t*)&(color2))

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
