#include <ft2build.h>
#include FT_FREETYPE_H

#include "Font.h"

static FT_Library ftLibrary = NULL;

static int EnsureFtLibrary() {
    if (ftLibrary) return 0;
    if (FT_Init_FreeType(&ftLibrary) != 0) {
        ftLibrary = NULL;
        return -1;
    }
    return 0;
}

static uint32_t UTF8Decode(const char* s, int* bytesOut) {
    const unsigned char* us = (const unsigned char*)s;
    if (us[0] < 0x80) {
        *bytesOut = 1;
        return us[0];
    }
    if ((us[0] & 0xE0) == 0xC0) {
        *bytesOut = 2;
        return ((us[0] & 0x1F) << 6) | (us[1] & 0x3F);
    }
    if ((us[0] & 0xF0) == 0xE0) {
        *bytesOut = 3;
        return ((us[0] & 0x0F) << 12) | ((us[1] & 0x3F) << 6) | (us[2] & 0x3F);
    }
    if ((us[0] & 0xF8) == 0xF0) {
        *bytesOut = 4;
        return ((us[0] & 0x07) << 18) | ((us[1] & 0x3F) << 12) | ((us[2] & 0x3F) << 6) | (us[3] & 0x3F);
    }
    *bytesOut = 1;
    return 0xFFFD;
}

Font FontLoad(const char* path, int pixelSize) {
    if (!path || pixelSize <= 0 || EnsureFtLibrary() != 0) return (Font){ 0 };

    FT_Face face;
    if (FT_New_Face(ftLibrary, path, 0, &face) != 0) {
        return (Font){ 0 };
    }

    if (FT_Set_Pixel_Sizes(face, 0, pixelSize) != 0) {
        FT_Done_Face(face);
        return (Font){ 0 };
    }

    return (Font) { face, pixelSize };
}

void FontFree(Font* font) {
    if (font->internal != NULL) {
        FT_Done_Face(font->internal);
        font->internal = NULL;
        font->size = 0;
    }
}

static void BlendPixel(uint8_t* dst, Color color, uint8_t bpp, const PixelFormat* format) {
    Color dstColor;
    switch (bpp) {
        case 1: {
            dstColor = PixelToColor(format, *dst);
        } break;
        case 2: {
            dstColor = PixelToColor(format, *(uint16_t*)dst);
        } break;
        case 4: {
            dstColor = PixelToColor(format, *(uint32_t*)dst);
        } break;
        default: {
            dstColor = (Color){ 0 };
        } break;
    }

    const int sa = color.a;
    const int da = (int)dstColor.a;
    const int invSa = 255 - sa;

    const Color outColor = {
        (uint8_t)((color.r * sa + dstColor.r * invSa) / 255),
        (uint8_t)((color.g * sa + dstColor.g * invSa) / 255),
        (uint8_t)((color.b * sa + dstColor.b * invSa) / 255),
        (uint8_t)((sa + da * invSa / 255))
    };

    const uint32_t outPixel = ColorToPixel(format, outColor);

    for (int b = 0; b < bpp; b++) {
        dst[b] = ((uint8_t*)&outPixel)[b];
    }
}

static void BlitGlyphToSurface(Surface surface, const FT_Bitmap* bitmap, int dstX, int dstY, Color color) {
    if (bitmap == NULL || bitmap->buffer == NULL) return;

    const int bpp = surface.format->bytesPerPixel;

    const int bmW = (int)bitmap->width;
    const int bmH = (int)bitmap->rows;

    int startX = 0, startY = 0;
    if (dstX < 0) startX = -dstX;
    if (dstY < 0) startY = -dstY;
    int endX = bmW;
    int endY = bmH;
    if (dstX + bmW > surface.width) endX = surface.width - dstX;
    if (dstY + bmH > surface.height) endY = surface.height - dstY;
    if (startX >= endX || startY >= endY) return;

    for (int gy = startY; gy < endY; ++gy) {
        const int sy = dstY + gy;
        uint8_t* row = &((uint8_t*)surface.pixels)[sy * surface.width * bpp];
        for (int gx = startX; gx < endX; ++gx) {
            const int sx = (dstX + gx) * bpp;
            const uint8_t glyphAlpha = bitmap->buffer[gy * bitmap->pitch + gx];
            if (glyphAlpha == 0) continue;
            const uint8_t combinedAlpha = (uint16_t)glyphAlpha * (uint16_t)color.a / 255;
            if (combinedAlpha == 0) continue;
            BlendPixel(&row[sx], (Color){ color.r, color.g, color.b, combinedAlpha }, bpp, surface.format);
        }
    }
}

void DrawFontChar(Surface surface, int x, int y, char c, Font* font, Color color) {
    const char s[2] = { c, '\0' };
    DrawFontText(surface, x, y, s, font, color);
}

void DrawFontText(Surface surface, int x, int y, const char* text, Font* font, Color color) {
    if (surface.pixels == NULL || font == NULL || font->internal == NULL) return;
    FT_Face face = font->internal;

    const int ascender = face->size != 0 && face->size->metrics.ascender != 0 ?
        (int)(face->size->metrics.ascender >> 6) : font->size;
    const int baseline = y + ascender;

    int penX = x;
    const char* p = text;
    while (*p) {
        int bytes;
        const uint32_t cp = UTF8Decode(p, &bytes);
        p += bytes;

        const FT_UInt glyphIndex = FT_Get_Char_Index(face, cp);
        if (glyphIndex == 0) {
            continue;
        }

        if (FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT) != 0) continue;
        if (face->glyph->format != FT_GLYPH_FORMAT_BITMAP) {
            if (FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL) != 0) continue;
        }

        const FT_Bitmap* bitmap = &face->glyph->bitmap;
        const int dstX = penX + face->glyph->bitmap_left;
        const int dstY = baseline - face->glyph->bitmap_top;
        BlitGlyphToSurface(surface, bitmap, dstX, dstY, color);

        const int adv = (int)(face->glyph->advance.x >> 6);
        penX += adv;
    }
}

void ShutdownFontModule() {
    if (ftLibrary != NULL) {
        FT_Done_FreeType(ftLibrary);
        ftLibrary = NULL;
    }
}
