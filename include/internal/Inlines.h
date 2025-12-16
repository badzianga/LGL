#ifndef LGL_INLINES_H
#define LGL_INLINES_H
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

// TODO: optimize with loop unrolling
inline static void Memset4(void* ptr, uint32_t value, size_t n) {
    uint32_t* p = ptr;
    while (n--) {
        *p++ = value;
    }
}

inline static Color BlendColors(Color src, Color dst, uint8_t a, uint8_t invA) {
    dst.r = (src.r * a + dst.r * invA) / 255;
    dst.g = (src.g * a + dst.g * invA) / 255;
    dst.b = (src.b * a + dst.b * invA) / 255;
    dst.a = src.a + (dst.a * invA) / 255;
    return dst;
}

static inline uint32_t ConvertPixel(uint32_t pixel, const PixelFormat* srcFmt, const PixelFormat* dstFmt) {
    uint32_t r = 0, g = 0, b = 0, a = 0;

    r = ((pixel & srcFmt->rMask) >> srcFmt->rShift) << srcFmt->rLoss;
    g = ((pixel & srcFmt->gMask) >> srcFmt->gShift) << srcFmt->gLoss;
    b = ((pixel & srcFmt->bMask) >> srcFmt->bShift) << srcFmt->bLoss;
    a = ((pixel & srcFmt->aMask) >> srcFmt->aShift) << srcFmt->aLoss;

    uint32_t out = 0;

    out |= (r << dstFmt->rShift) >> dstFmt->rLoss;
    out |= (g << dstFmt->gShift) >> dstFmt->gLoss;
    out |= (b << dstFmt->bShift) >> dstFmt->bLoss;
    out |= (a << dstFmt->aShift) >> dstFmt->aLoss;

    return out;
}

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // LGL_INLINES_H
