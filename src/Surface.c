#if defined(__SSE4_1__)
#include <smmintrin.h>
#elif defined(__SSE2__)
#include <emmintrin.h>
#endif
#include <string.h>

#include "Allocator.h"
#include "Color.h"
#include "Error.h"
#include "FillRect.h"
#include "internal/Inlines.h"
#include "PixelFormat.h"
#include "Surface.h"

Surface SurfaceCreate(int width, int height, const PixelFormat* format) {
    if (width <= 0 || height <= 0 || format == NULL) {
        THROW_ERROR(ERR_INVALID_PARAMS);
        return (Surface){ 0 };
    }

    const int stride = width * format->bytesPerPixel;
    void* pixels = AllocatorAlloc(stride * height);
    if (pixels == NULL) {
        THROW_ERROR(ERR_OUT_OF_MEMORY);
        return (Surface){ 0 };
    }
    memset(pixels, 0, stride * height);

    SurfaceFlags flags = SURFACE_FLAG_NONE;
    if (format->aMask != 0) {
        flags |= SURFACE_FLAG_HAS_ALPHA;
    }

    return (Surface){ width, height, pixels, stride, flags, format };
}

Surface SurfaceCreateFromBuffer(int width, int height, const PixelFormat* format, void* buffer) {
    if (width <= 0 || height <= 0 || format == NULL || buffer == NULL) {
        THROW_ERROR(ERR_INVALID_PARAMS);
        return (Surface){ 0 };
    }

    const int stride = width * format->bytesPerPixel;

    SurfaceFlags flags = SURFACE_FLAG_PREALLOCATED;
    if (format->aMask != 0) {
        // analyze buffer to set proper flags
        for (int i = 0; i < width * height; ++i) {
            // for now, only 4-byte pixel format with alpha is suppoerted, so uint32_t can be safely used
            const uint32_t pixel = ((uint32_t*)buffer)[i];
            const Color pixelColor = PixelToColor(format, pixel);
            if (pixelColor.a < 255) {
                flags |= SURFACE_FLAG_HAS_ALPHA;
                break;
            }
        }
    }

    return (Surface){ width, height, buffer, stride, flags, format };
}

void SurfaceDestroy(Surface* surface) {
    if (surface == NULL || (surface->flags & SURFACE_FLAG_PREALLOCATED)) return;
    AllocatorFree(surface->pixels);
    *surface = (Surface){ 0 };
}

Surface SurfaceCopy(Surface src) {
    if (src.pixels == NULL || src.format == NULL) {
        THROW_ERROR(ERR_INVALID_PARAMS);
        return (Surface){ 0 };
    }
    Surface copy = SurfaceCreate(src.width, src.height, src.format);
    memcpy(copy.pixels, src.pixels, src.stride * src.height);
    copy.flags &= ~SURFACE_FLAG_PREALLOCATED;
    return copy;
}

Surface SurfaceConvert(Surface surface, const PixelFormat* format) {
    if (surface.pixels == NULL || surface.format == NULL || format == NULL) {
        THROW_ERROR(ERR_INVALID_PARAMS);
        return (Surface){ 0 };
    }

    if (surface.format == format) {
        Surface copy = SurfaceCopy(surface);
        copy.flags &= ~SURFACE_FLAG_PREALLOCATED;
        return copy;
    }

    Surface converted = SurfaceCreate(surface.width, surface.height, format);
    if (surface.format->aMask != 0 && converted.format->aMask == 0) {
        // formats with bpp < 4 cannot have alpha channel, so alpha flags should be removed
        if (converted.flags & SURFACE_FLAG_HAS_ALPHA) {
            converted.flags &= ~SURFACE_FLAG_HAS_ALPHA;
        }
    }
    converted.flags &= ~SURFACE_FLAG_PREALLOCATED;

    // SurfaceBlit can convert formats on the fly, so it is used here to avoid code duplication
    SurfaceBlit(converted, surface, 0, 0);

    return converted;
}

void SurfaceFill(Surface surface, Color color) {
    const Rect rect = { 0, 0, surface.width, surface.height };
    if (color.a == 0) return;
    if (color.a == 255) {
        FillRect(surface, &rect, ColorToPixel(surface.format, color));
    }
    else {
        BlendFillRect(surface, &rect, color);
    }
}

static void BlitSameFormat(Surface dest, Surface src, int x, int y) {
    const int bpp = src.format->bytesPerPixel;

    const Rect destRect = { 0, 0, dest.width, dest.height };
    const Rect srcRect = { x, y, src.width, src.height };
    Rect clipped;
    if (!RectIntersection(&srcRect, &destRect, &clipped)) return;

    int h = clipped.height;
    const int w = clipped.width * bpp;
    uint8_t* dstRow = (uint8_t*)dest.pixels + clipped.y * dest.stride + clipped.x * bpp;
    const uint8_t* srcRow = (uint8_t*)src.pixels + (clipped.y - y) * src.stride + (clipped.x - x) * bpp;
    while (h--) {
        memcpy(dstRow, srcRow, w);
        dstRow += dest.stride;
        srcRow += src.stride;
    }
}

#ifdef __SSE2__

static inline void UnpackRGBA_SSE2(__m128i px, const PixelFormat* f, __m128i* r, __m128i* g, __m128i* b, __m128i* a) {
    const __m128i rm = _mm_set1_epi32((int)(f->rMask));
    const __m128i gm = _mm_set1_epi32((int)(f->gMask));
    const __m128i bm = _mm_set1_epi32((int)(f->bMask));
    const __m128i am = _mm_set1_epi32((int)(f->aMask));

    *r = _mm_and_si128(px, rm);
    *g = _mm_and_si128(px, gm);
    *b = _mm_and_si128(px, bm);
    *a = _mm_and_si128(px, am);

    *r = _mm_srli_epi32(*r, f->rShift);
    *g = _mm_srli_epi32(*g, f->gShift);
    *b = _mm_srli_epi32(*b, f->bShift);
    *a = _mm_srli_epi32(*a, f->aShift);
}

static inline __m128i PackRGBA_SSE2(__m128i r, __m128i g, __m128i b, __m128i a, const PixelFormat* f) {
    r = _mm_slli_epi32(r, f->rShift);
    g = _mm_slli_epi32(g, f->gShift);
    b = _mm_slli_epi32(b, f->bShift);
    a = _mm_slli_epi32(a, f->aShift);

    return _mm_or_si128(
        _mm_or_si128(r, g),
        _mm_or_si128(b, a)
    );
}

// On SSE4.2 operation _mm_mullo_epi32 is present, but on SSE2 it has to be emulated
#ifndef __SSE4_1__
static inline __m128i _mm_mullo_epi32(__m128i a, __m128i b) {
    const __m128i a_lo = _mm_and_si128(a, _mm_set1_epi32(0xFFFF));
    const __m128i b_lo = _mm_and_si128(b, _mm_set1_epi32(0xFFFF));

    const __m128i a_hi = _mm_srli_epi32(a, 16);
    const __m128i b_hi = _mm_srli_epi32(b, 16);

    const __m128i lo = _mm_mullo_epi16(a_lo, b_lo);
    __m128i hi = _mm_mullo_epi16(a_hi, b_lo);
    __m128i mid = _mm_mullo_epi16(a_lo, b_hi);

    hi = _mm_slli_epi32(hi, 16);
    mid = _mm_slli_epi32(mid, 16);

    return _mm_add_epi32(lo, _mm_add_epi32(hi, mid));
}
#endif  // __SSE4_1__

static inline __m128i BlendChannel_SSE2(__m128i s, __m128i d, __m128i a) {
    const __m128i inv255 = _mm_set1_epi32(255);
    const __m128i ia = _mm_sub_epi32(inv255, a);

    const __m128i sm = _mm_mullo_epi32(s, a);
    const __m128i dm = _mm_mullo_epi32(d, ia);

    const __m128i sum = _mm_add_epi32(sm, dm);

    return _mm_srli_epi32(sum, 8);
}

static inline void BlendRGBA_SSE2(
    __m128i sr, __m128i sg, __m128i sb, __m128i sa,
    __m128i dr, __m128i dg, __m128i db, __m128i da,
    __m128i* or, __m128i* og, __m128i* ob, __m128i* oa)
{
    *or = BlendChannel_SSE2(sr, dr, sa);
    *og = BlendChannel_SSE2(sg, dg, sa);
    *ob = BlendChannel_SSE2(sb, db, sa);

    const __m128i inv255 = _mm_set1_epi32(255);
    const __m128i ia = _mm_sub_epi32(inv255, sa);

    const __m128i da_m = _mm_mullo_epi32(da, ia);
    *oa = _mm_add_epi32(sa, _mm_srli_epi32(da_m, 8));
}

static void BlitSameFormatA_SSE2(Surface dest, Surface src, int x, int y) {
    const PixelFormat* fmt = dest.format;

    const Rect destRect = { 0, 0, dest.width, dest.height };
    const Rect srcRect  = { x, y, src.width, src.height };
    Rect clipped;

    if (!RectIntersection(&srcRect, &destRect, &clipped)) return;

    uint8_t* dstRow = (uint8_t*)dest.pixels + clipped.y * dest.stride + (clipped.x << 2);
    const uint8_t* srcRow = (uint8_t*)src.pixels + (clipped.y - y) * src.stride + ((clipped.x - x) << 2);

    for (int iy = 0; iy < clipped.height; ++iy) {
        uint32_t* d = (uint32_t*)dstRow;
        const uint32_t* s = (uint32_t*)srcRow;

        int ix = 0;
        for (; ix + 3 < clipped.width; ix += 4) {
            const __m128i srcPx = _mm_loadu_si128((__m128i*)(s + ix));
            const __m128i dstPx = _mm_loadu_si128((__m128i*)(d + ix));

            __m128i sr, sg, sb, sa, dr, dg, db, da;
            UnpackRGBA_SSE2(srcPx, fmt, &sr,&sg,&sb,&sa);
            UnpackRGBA_SSE2(dstPx, fmt, &dr,&dg,&db,&da);

            __m128i rr, rg, rb, ra;
            BlendRGBA_SSE2(
                sr, sg, sb, sa,
                dr, dg, db, da,
                &rr, &rg, &rb, &ra
            );

            const __m128i out = PackRGBA_SSE2(rr, rg, rb, ra, fmt);
            _mm_storeu_si128((__m128i*)(d + ix), out);
        }

        for (; ix < clipped.width; ++ix) {
            const Color sc = PixelToColor(fmt, s[ix]);
            if (!sc.a) continue;
            if (sc.a == 255) {
                d[ix] = s[ix];
            }
            else {
                Color dc = PixelToColor(fmt, d[ix]);
                const int invA = 255 - sc.a;
                dc = BlendColors(sc, dc, sc.a, invA);
                d[ix] = ColorToPixel(fmt, dc);
            }
        }

        dstRow += dest.stride;
        srcRow += src.stride;
    }
}

#else

// for now, LGL blitting supports ONLY 4-byte colors with alpha channel
static void BlitSameFormatA(Surface dest, Surface src, int x, int y) {
    const PixelFormat* fmt = dest.format;

    const Rect destRect = { 0, 0, dest.width, dest.height };
    const Rect srcRect = { x, y, src.width, src.height };
    Rect clipped;
    if (!RectIntersection(&srcRect, &destRect, &clipped)) return;

    const int h = clipped.height;
    const int w = clipped.width;

    uint8_t* dstRow8 = (uint8_t*)dest.pixels + clipped.y * dest.stride + (clipped.x << 2);
    uint8_t* srcRow8 = (uint8_t*)src.pixels + (clipped.y - y) * src.stride + ((clipped.x - x) << 2);

    for (int iy = 0; iy < h; ++iy) {
        uint32_t* dstRow = (uint32_t*)dstRow8;
        const uint32_t* srcRow = (uint32_t*)srcRow8;

        for (int ix = 0; ix < w; ++ix) {
            const uint32_t srcValue = srcRow[ix];
            const Color srcColor = PixelToColor(fmt, srcValue);
            if (srcColor.a == 0) {
                continue;
            }
            if (srcColor.a == 255) {
                dstRow[ix] = srcValue;
            }
            else {
                const uint32_t dstValue = dstRow[ix];
                Color dstColor = PixelToColor(fmt, dstValue);
                const int invA = 255 - srcColor.a;
                dstColor = BlendColors(srcColor, dstColor, srcColor.a, invA);
                dstRow[ix] = ColorToPixel(fmt, dstColor);
            }
        }
        dstRow8 += dest.stride;
        srcRow8 += src.stride;
    }
}

#endif // __SSE2__

#define LOAD_PIXEL(ptr, bpp)         \
    ((bpp) == 1 ? *(uint8_t*)(ptr) : \
    (bpp) == 2 ? *(uint16_t*)(ptr) : \
                 *(uint32_t*)(ptr))

#define STORE_PIXEL(ptr, bpp, value)                              \
    do {                                                          \
        switch (bpp) {                                            \
            case 1: *(uint8_t*)(ptr)  = (uint8_t)(value); break;  \
            case 2: *(uint16_t*)(ptr) = (uint16_t)(value); break; \
            case 4: *(uint32_t*)(ptr) = (uint32_t)(value); break; \
            default: break;                                       \
        }                                                         \
    } while (0)

static void BlitDifferentFormat(Surface dest, Surface src, int x, int y) {
    const int srcBpp  = src.format->bytesPerPixel;
    const int destBpp = dest.format->bytesPerPixel;

    const Rect destRect = { 0, 0, dest.width, dest.height };
    const Rect srcRect  = { x, y, src.width, src.height };
    Rect clipped;

    if (!RectIntersection(&srcRect, &destRect, &clipped)) return;

    const int w = clipped.width;
    const int h = clipped.height;

    const uint8_t* srcRow = (uint8_t*)src.pixels + (clipped.y - y) * src.stride + (clipped.x - x) * srcBpp;

    uint8_t* destRow = (uint8_t*)dest.pixels + clipped.y * dest.stride + clipped.x * destBpp;

    for (int iy = 0; iy < h; ++iy) {
        const uint8_t* srcPixel  = srcRow;
        uint8_t* destPixel = destRow;

        for (int ix = 0; ix < w; ++ix) {
            const uint32_t srcValue = LOAD_PIXEL(srcPixel, srcBpp);

            const Color c = PixelToColor(src.format, srcValue);
            const uint32_t dstValue = ColorToPixel(dest.format, c);

            STORE_PIXEL(destPixel, destBpp, dstValue);

            srcPixel += srcBpp;
            destPixel += destBpp;
        }

        srcRow += src.stride;
        destRow += dest.stride;
    }
}

static void BlitDifferentFormatA(Surface dest, Surface src, int x, int y) {
    const Rect destRect = { 0, 0, dest.width, dest.height };
    const Rect srcRect = { x, y, src.width, src.height };
    Rect clipped;

    if (!RectIntersection(&srcRect, &destRect, &clipped)) return;

    const int w = clipped.width;
    const int h = clipped.height;

    const int srcBpp = src.format->bytesPerPixel;
    const int destBpp = dest.format->bytesPerPixel;

    uint8_t* srcRow = (uint8_t*)src.pixels + (clipped.y - y) * src.stride + (clipped.x - x) * srcBpp;
    uint8_t* dstRow = (uint8_t*)dest.pixels + clipped.y * dest.stride + clipped.x * destBpp;

    for (int iy = 0; iy < h; ++iy) {
        uint8_t* srcPixel = srcRow;
        uint8_t* dstPixel = dstRow;

        for (int ix = 0; ix < w; ++ix) {
            const uint32_t srcValue = LOAD_PIXEL(srcPixel, srcBpp);
            const Color srcColor = PixelToColor(src.format, srcValue);

            const uint8_t a = srcColor.a;
            if (a == 0) continue;
            if (a == 255) {
                const uint32_t out = ColorToPixel(dest.format, srcColor);
                STORE_PIXEL(dstPixel, destBpp, out);
            }
            else {
                const uint32_t dstValue = LOAD_PIXEL(dstPixel, destBpp);
                Color dstColor = PixelToColor(dest.format, dstValue);

                const uint8_t invA = 255 - a;
                dstColor = BlendColors(srcColor, dstColor, a, invA);
                const uint32_t outPixel = ColorToPixel(dest.format, dstColor);
                STORE_PIXEL(dstPixel, destBpp, outPixel);
            }

            srcPixel += srcBpp;
            dstPixel += destBpp;
        }

        srcRow += src.stride;
        dstRow += dest.stride;
    }
}

void SurfaceBlit(Surface dest, Surface src, int x, int y) {
    if (dest.format == src.format) {
        if (src.flags & SURFACE_FLAG_HAS_ALPHA)
#ifdef __SSE2__
            BlitSameFormatA_SSE2(dest, src, x, y);
#else
            BlitSameFormatA(dest, src, x, y);
#endif  // __SSE2__
        else
            BlitSameFormat(dest, src, x, y);
    }
    else {
        if (src.flags & SURFACE_FLAG_HAS_ALPHA)
            BlitDifferentFormatA(dest, src, x, y);
        else
            BlitDifferentFormat(dest, src, x, y);
    }
}

void SurfaceSetColorKey(Surface* surface, Color color) {
    const uint8_t r = color.r;
    const uint8_t g = color.g;
    const uint8_t b = color.b;

    surface->flags &= 0x000000FF;  // clear color key components, but leave flags
    surface->flags |= SURFACE_FLAG_HAS_COLOR_KEY;
    surface->flags |= SURFACE_FLAG_HAS_ALPHA;

    surface->flags |= r << 24;
    surface->flags |= g << 16;
    surface->flags |= b << 8;
}

Color SurfaceGetColorKey(Surface surface) {
    if (!(surface.flags & SURFACE_FLAG_HAS_COLOR_KEY)) return (Color){ 0 };
    return (Color){
        .r = (surface.flags & 0xFF000000) >> 24,
        .g = (surface.flags & 0x00FF0000) >> 16,
        .b = (surface.flags & 0x0000FF00) >> 8,
        .a = 0
    };
}

void SurfaceUnsetColorKey(Surface* surface) {
    surface->flags &= 0x000000FF;
    surface->flags &= ~SURFACE_FLAG_HAS_COLOR_KEY;
}
