#include <math.h>
#include <string.h>

#include "Allocator.h"
#include "internal/FixedPoint.h"
#include "Transform.h"

#define MAKE_FLIP_X_FUNCTION(TYPE, BYTES)                                     \
static void FlipX##BYTES(Surface surface) {                                   \
    const int last = surface.width - 1;                                       \
    for (int y = 0; y < surface.height; ++y) {                                \
        TYPE* left = (TYPE*)((uint8_t*)surface.pixels + surface.stride * y);  \
        TYPE* right = left + last;                                            \
        while (left < right) {                                                \
            TYPE tmp = *left;                                                 \
            *left++ = *right;                                                 \
            *right-- = tmp;                                                   \
        }                                                                     \
    }                                                                         \
}

MAKE_FLIP_X_FUNCTION(uint8_t, 1);
MAKE_FLIP_X_FUNCTION(uint16_t, 2);
MAKE_FLIP_X_FUNCTION(uint32_t, 4);

void TransformFlipX(Surface surface) {
    switch (surface.format->bytesPerPixel) {
        case 1: FlipX1(surface); break;
        case 2: FlipX2(surface); break;
        case 4: FlipX4(surface); break;
        default: break;
    }
}

void TransformFlipY(Surface surface) {
    const int stride = surface.stride;
    const int lastRow = surface.height - 1;

    uint8_t* top = surface.pixels;
    uint8_t* bottom = (uint8_t*)surface.pixels + lastRow * stride;
    uint8_t* temp = AllocatorAlloc(stride);

    while (top < bottom) {
        memcpy(temp, top, stride);
        memcpy(top, bottom, stride);
        memcpy(bottom, temp, stride);

        top += stride;
        bottom -= stride;
    }

    AllocatorFree(temp);
}

static const fixed_t sinLUT[360] = {
    0, 1143, 2287, 3429, 4571, 5711, 6850, 7986, 9120, 10252, 11380, 12504, 13625, 14742, 15854,
    16961, 18064, 19160, 20251, 21336, 22414, 23486, 24550, 25606, 26655, 27696, 28729, 29752, 30767, 31772,
    32767, 33753, 34728, 35693, 36647, 37589, 38521, 39440, 40347, 41243, 42125, 42995, 43852, 44695, 45525,
    46340, 47142, 47929, 48702, 49460, 50203, 50931, 51643, 52339, 53019, 53683, 54331, 54963, 55577, 56175,
    56755, 57319, 57864, 58393, 58903, 59395, 59870, 60326, 60763, 61183, 61583, 61965, 62328, 62672, 62997,
    63302, 63589, 63856, 64103, 64331, 64540, 64729, 64898, 65047, 65176, 65286, 65376, 65446, 65496, 65526,
    65536, 65526, 65496, 65446, 65376, 65286, 65176, 65047, 64898, 64729, 64540, 64331, 64103, 63856, 63589,
    63302, 62997, 62672, 62328, 61965, 61583, 61183, 60763, 60326, 59870, 59395, 58903, 58393, 57864, 57319,
    56755, 56175, 55577, 54963, 54331, 53683, 53019, 52339, 51643, 50931, 50203, 49460, 48702, 47929, 47142,
    46340, 45525, 44695, 43852, 42995, 42125, 41243, 40347, 39440, 38521, 37589, 36647, 35693, 34728, 33753,
    32767, 31772, 30767, 29752, 28729, 27696, 26655, 25606, 24550, 23486, 22414, 21336, 20251, 19160, 18064,
    16961, 15854, 14742, 13625, 12504, 11380, 10252, 9120, 7986, 6850, 5711, 4571, 3429, 2287, 1143,
    0, -1143, -2287, -3429, -4571, -5711, -6850, -7986, -9120, -10252, -11380, -12504, -13625, -14742, -15854,
    -16961, -18064, -19160, -20251, -21336, -22414, -23486, -24550, -25606, -26655, -27696, -28729, -29752, -30767, -31772,
    -32768, -33753, -34728, -35693, -36647, -37589, -38521, -39440, -40347, -41243, -42125, -42995, -43852, -44695, -45525,
    -46340, -47142, -47929, -48702, -49460, -50203, -50931, -51643, -52339, -53019, -53683, -54331, -54963, -55577, -56175,
    -56755, -57319, -57864, -58393, -58903, -59395, -59870, -60326, -60763, -61183, -61583, -61965, -62328, -62672, -62997,
    -63302, -63589, -63856, -64103, -64331, -64540, -64729, -64898, -65047, -65176, -65286, -65376, -65446, -65496, -65526,
    -65536, -65526, -65496, -65446, -65376, -65286, -65176, -65047, -64898, -64729, -64540, -64331, -64103, -63856, -63589,
    -63302, -62997, -62672, -62328, -61965, -61583, -61183, -60763, -60326, -59870, -59395, -58903, -58393, -57864, -57319,
    -56755, -56175, -55577, -54963, -54331, -53683, -53019, -52339, -51643, -50931, -50203, -49460, -48702, -47929, -47142,
    -46340, -45525, -44695, -43852, -42995, -42125, -41243, -40347, -39440, -38521, -37589, -36647, -35693, -34728, -33753,
    -32768, -31772, -30767, -29752, -28729, -27696, -26655, -25606, -24550, -23486, -22414, -21336, -20251, -19160, -18064,
    -16961, -15854, -14742, -13625, -12504, -11380, -10252, -9120, -7986, -6850, -5711, -4571, -3429, -2287, -1143,
};

static inline fixed_t SinDeg(int angle) {
    angle %= 360;
    if (angle < 0) angle += 360;
    return sinLUT[angle];
}

static inline fixed_t CosDeg(int angle) {
    angle = (angle + 90) % 360;
    if (angle < 0) angle += 360;
    return sinLUT[angle];
}

Surface TransformRotate(Surface src, int angle) {
    angle %= 360;
    if (angle < 0) angle += 360;

    const int cw = src.width;
    const int ch = src.height;

    const int cosA = CosDeg(angle);
    const int sinA = SinDeg(angle);

    const int cornersX[4] = {0, cw, 0, cw};
    const int cornersY[4] = {0, 0, ch, ch};

    fixed_t minX = 1 << 30;
    fixed_t minY = 1 << 30;
    fixed_t maxX = -1 << 30;
    fixed_t maxY = -1 << 30;

    const int bpp = src.format->bytesPerPixel;

    for (int i = 0; i < 4; ++i) {
        const int x = cornersX[i] - (cw >> 1);
        const int y = cornersY[i] - (ch >> 1);

        const fixed_t xr = FIXED_MUL(TO_FIXED(x), cosA) - FIXED_MUL(TO_FIXED(y), sinA);
        const fixed_t yr = FIXED_MUL(TO_FIXED(x), sinA) + FIXED_MUL(TO_FIXED(y), cosA);

        const fixed_t xf = FIXED_INT_PART(xr);
        const fixed_t yf = FIXED_INT_PART(yr);

        if (xf < minX) minX = xf;
        if (xf > maxX) maxX = xf;
        if (yf < minY) minY = yf;
        if (yf > maxY) maxY = yf;
    }

    const int newW = maxX - minX;
    const int newH = maxY - minY;

    const Surface dst = SurfaceCreate(newW, newH, src.format);

    const int srcCX = cw >> 1;
    const int srcCY = ch >> 1;

    const int dstCX = newW >> 1;
    const int dstCY = newH >> 1;

    for (int y = 0; y < newH; ++y) {
        for (int x = 0; x < newW; ++x) {
            const int dx = x - dstCX;
            const int dy = y - dstCY;

            const fixed_t sxFP = FIXED_MUL(TO_FIXED(dx), cosA) + FIXED_MUL(TO_FIXED(dy), sinA) + TO_FIXED(srcCX);
            const fixed_t syFP = -FIXED_MUL(TO_FIXED(dx), sinA) + FIXED_MUL(TO_FIXED(dy), cosA) + TO_FIXED(srcCY);

            const int sx = FIXED_INT_PART(sxFP);
            const int sy = FIXED_INT_PART(syFP);

            const uint8_t* srcPixel = (uint8_t*)src.pixels + sy * src.stride + sx * bpp;
            uint8_t* dstPixel = (uint8_t*)dst.pixels + y * dst.stride + x * bpp;

            if (sx >= 0 && sx < cw && sy >= 0 && sy < ch) {
                switch (bpp) {
                    case 1: {
                        *dstPixel = *srcPixel;
                    } break;
                    case 2: {
                        *(uint16_t*)dstPixel = *(uint16_t*)srcPixel;
                    } break;
                    case 4: {
                        *(uint32_t*)dstPixel = *(uint32_t*)srcPixel;
                    } break;
                    default: break;
                }
            }
        }
    }

    return dst;
}

#define MAKE_SCALE_FUNCTION(TYPE, BYTES)                                                                          \
static Surface Scale##BYTES(Surface src, Surface dest, int scaleX, int scaleY, int lastSrcRow, int lastSrcCol) {  \
    for (int y = 0; y < dest.height; ++y) {                                                                       \
        int srcY = FIXED_INT_PART(y * scaleY);                                                                    \
        if (srcY >= src.height) srcY = lastSrcRow;                                                                \
        const TYPE* srcRow = (TYPE*)((uint8_t*)src.pixels + srcY * src.stride);                                   \
        TYPE* dstRow = (TYPE*)((uint8_t*)dest.pixels + y * dest.stride);                                          \
        for (int x = 0; x < dest.width; ++x) {                                                                    \
            int srcX = FIXED_INT_PART(x * scaleX);                                                                \
            if (srcX >= src.width) srcX = lastSrcCol;                                                             \
            dstRow[x] = srcRow[srcX];                                                                             \
        }                                                                                                         \
    }                                                                                                             \
    return dest;                                                                                                  \
}

MAKE_SCALE_FUNCTION(uint8_t, 1)
MAKE_SCALE_FUNCTION(uint16_t, 2)
MAKE_SCALE_FUNCTION(uint32_t, 4)

Surface TransformScale(Surface src, int destWidth, int destHeight) {
    const Surface dest = SurfaceCreate(destWidth, destHeight, src.format);
    const fixed_t scaleX = FIXED_DIV(src.width, destWidth);
    const fixed_t scaleY = FIXED_DIV(src.height, destHeight);
    const int lastSrcRow = src.height - 1;
    const int lastSrcCol = src.width - 1;

    switch (src.format->bytesPerPixel) {
        case 1: return Scale1(src, dest, scaleX, scaleY, lastSrcRow, lastSrcCol);
        case 2: return Scale2(src, dest, scaleX, scaleY, lastSrcRow, lastSrcCol);
        case 4: return Scale4(src, dest, scaleX, scaleY, lastSrcRow, lastSrcCol);
        default: return dest;
    }
}

// TODO: avoid using sColorArray, write pixels to scaled surface in calculation iteration
// TODO: separate calculating borders to avoid unnecessary checking for borders in every iteration
Surface TransformScale2x(Surface original) {
    const PixelFormat* const fmt = original.format;
    const int bpp = fmt->bytesPerPixel;

    const Surface scaled = SurfaceCreate(original.width * 2, original.height * 2, fmt);

    Color* oColorArray = AllocatorAlloc(sizeof(Color) * original.width * original.height);
    Color* sColorArray = AllocatorAlloc(sizeof(Color) * scaled.width * scaled.height);

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

    AllocatorFree(oColorArray);
    AllocatorFree(sColorArray);

    return scaled;
}
