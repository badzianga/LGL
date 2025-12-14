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

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // LGL_INLINES_H
