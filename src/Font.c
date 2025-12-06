#include <harfbuzz/hb.h>
#include <harfbuzz/hb-ft.h>
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

    hb_font_t* hbFont = hb_ft_font_create(face, NULL);
    if (!hbFont) {
        FT_Done_Face(face);
        return (Font){ 0 };
    }

    return (Font) { face, hbFont, pixelSize };
}

void FontFree(Font* font) {
    if (font == NULL) return;
    if (font->hbFont != NULL) {
        hb_font_destroy(font->hbFont);
        font->hbFont = NULL;
    }
    if (font->internal != NULL) {
        FT_Done_Face(font->internal);
        font->internal = NULL;
    }
    font->size = 0;
}

static int ComputeBaselineFromTop(FT_Face face, int topY) {
    return topY + (face->size != 0 ? (int)(face->size->metrics.ascender >> 6) : 0);
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
    const int da = dstColor.a;
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
    // TODO: should it be >= ?
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

void DrawFontChar(Surface surface, int x, int y, char c, const Font* font, Color color) {
    const char s[2] = { c, '\0' };
    DrawFontText(surface, x, y, s, font, color);
}

void DrawFontText(Surface surface, int x, int y, const char* text, const Font* font, Color color) {
    if (surface.pixels == NULL || font == NULL || font->internal == NULL) return;
    FT_Face face = font->internal;
    hb_font_t* hbFont = font->hbFont;

    hb_buffer_t* buf = hb_buffer_create();
    hb_buffer_add_utf8(buf, text, -1, 0, -1);
    hb_buffer_guess_segment_properties(buf);
    hb_shape(hbFont, buf, NULL, 0);

    uint32_t glyphCount = 0;
    hb_glyph_info_t* glyphInfo = hb_buffer_get_glyph_infos(buf, &glyphCount);
    hb_glyph_position_t* glyphPos = hb_buffer_get_glyph_positions(buf, &glyphCount);

    const int baseline = ComputeBaselineFromTop(face, y);

    long penX = x;
    long penY = baseline;

    for (uint32_t i = 0; i < glyphCount; i++) {
        const hb_glyph_info_t gi = glyphInfo[i];
        const hb_glyph_position_t gp = glyphPos[i];

        uint32_t glyphIndex = gi.codepoint;

        const int xOffset = (int)(gp.x_offset >> 6);
        const int yOffset = (int)(gp.y_offset >> 6);
        const int xAdvance = (int)(gp.x_advance >> 6);
        const int yAdvance = (int)(gp.y_advance >> 6);

        if (FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT)) {
            penX += xAdvance;
            penY += yAdvance;
            continue;
        }

        if (face->glyph->format != FT_GLYPH_FORMAT_BITMAP) {
            if (FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL)) {
                penX += xAdvance;
                penY += yAdvance;
                continue;
            }
        }

        const FT_Bitmap* bitmap = &face->glyph->bitmap;
        const int dstX = (int)(penX + xOffset) + face->glyph->bitmap_left;
        const int dstY = (int)(penY + yOffset) - face->glyph->bitmap_top;
        BlitGlyphToSurface(surface, bitmap, dstX, dstY, color);

        penX += xAdvance;
        penY += yAdvance;
    }

    hb_buffer_destroy(buf);
}

void MeasureFontText(const char* text, const Font* font, int* outWidth, int* outHeight) {
    if (!text || !font || !font->internal || !font->hbFont) {
        if (outWidth) *outWidth = 0;
        if (outHeight) *outHeight = 0;
        return;
    }

    FT_Face face = (FT_Face)font->internal;
    hb_font_t* hbFont = (hb_font_t*)font->hbFont;

    int maxWidth = 0;
    int lines = 1;

    const int asc = (int)(face->size->metrics.ascender >> 6);
    const int desc = (int)(face->size->metrics.descender >> 6);
    int lineHeight = asc - desc;
    if (lineHeight < 0) lineHeight = -lineHeight;
    if (*text == '\0') lineHeight = 0;

    const char* start = text;

    for (const char* p = text; ; ++p) {
        if (*p != '\n' && *p != '\0') continue;

        const size_t len = p - start;

        if (len > 0) {
            hb_buffer_t* buf = hb_buffer_create();
            hb_buffer_add_utf8(buf, start, (int)len, 0, (int)len);
            hb_buffer_guess_segment_properties(buf);
            hb_shape(hbFont, buf, NULL, 0);

            unsigned int glyphCount;
            hb_glyph_position_t* pos = hb_buffer_get_glyph_positions(buf, &glyphCount);

            int lineWidth = 0;
            for (unsigned int i = 0; i < glyphCount; i++) {
                lineWidth += (pos[i].x_advance >> 6);
            }

            if (lineWidth > maxWidth)
                maxWidth = lineWidth;

            hb_buffer_destroy(buf);
        }

        if (*p == '\0') break;

        lines++;
        start = p + 1;
    }

    if (outWidth) *outWidth = maxWidth;
    if (outHeight) *outHeight = lines * lineHeight;
}

void ShutdownFontModule() {
    if (ftLibrary != NULL) {
        FT_Done_FreeType(ftLibrary);
        ftLibrary = NULL;
    }
}
