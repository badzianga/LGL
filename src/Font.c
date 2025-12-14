#include <harfbuzz/hb.h>
#include <harfbuzz/hb-ft.h>
#include FT_FREETYPE_H

#include "Font.h"
#include "Error.h"
#include "internal/Inlines.h"

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
    if (!path || pixelSize <= 0 || EnsureFtLibrary() != 0) {
        if (EnsureFtLibrary() != 0) THROW_ERROR(ERR_INTERNAL_ERROR);
        else THROW_ERROR(ERR_INVALID_PARAMS);
        return (Font){ 0 };
    }

    FT_Face face;
    if (FT_New_Face(ftLibrary, path, 0, &face) != 0) {
        THROW_ERROR(ERR_INTERNAL_ERROR);
        return (Font){ 0 };
    }

    if (FT_Set_Pixel_Sizes(face, 0, pixelSize) != 0) {
        FT_Done_Face(face);
        THROW_ERROR(ERR_INTERNAL_ERROR);
        return (Font){ 0 };
    }

    hb_font_t* hbFont = hb_ft_font_create(face, NULL);
    if (!hbFont) {
        FT_Done_Face(face);
        THROW_ERROR(ERR_INTERNAL_ERROR);
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

static inline int ComputeBaselineFromTop(FT_Face face, int topY) {
    return topY + (face->size != NULL ? (int)(face->size->metrics.ascender >> 6) : 0);
}

static void BlitGlyphToSurface(Surface surface, const FT_Bitmap* bitmap, int dstX, int dstY, Color color) {
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

    const int a = color.a;
    const int invA = 255 - a;
    const uint32_t fullColor = ColorToPixel(surface.format, color);

    for (int gy = startY; gy < endY; ++gy) {
        const int sy = dstY + gy;
        uint8_t* row = &((uint8_t*)surface.pixels)[sy * surface.width * bpp];
        for (int gx = startX; gx < endX; ++gx) {
            const int sx = (dstX + gx) * bpp;
            const uint8_t glyphAlpha = bitmap->buffer[gy * bitmap->pitch + gx];
            if (glyphAlpha == 0) continue;
            const uint8_t combinedAlpha = (uint16_t)glyphAlpha * (uint16_t)color.a / 255;
            if (combinedAlpha == 0) continue;
            uint8_t* dst = row + sx;
            if (combinedAlpha == 255) {
                switch (bpp) {
                    case 1: {
                        *dst = (uint8_t)fullColor;
                    } break;
                    case 2: {
                        *(uint16_t*)dst = (uint16_t)fullColor;
                    } break;
                    case 4: {
                        *(uint32_t*)dst = fullColor;
                    } break;
                    default: break;
                }
            }
            else {
                const Color newColor = { color.r, color.g, color.b, combinedAlpha };
                switch (bpp) {
                    case 1: {
                        Color c = PixelToColor(surface.format, *dst);
                        c = BlendColors(newColor, c, a, invA);
                        *dst = (uint8_t)ColorToPixel(surface.format, c);
                    } break;
                    case 2: {
                        Color c = PixelToColor(surface.format, *(uint16_t*)dst);
                        c = BlendColors(newColor, c, a, invA);
                        *(uint16_t*)dst = (uint16_t)ColorToPixel(surface.format, c);
                    } break;
                    case 4: {
                        Color c = PixelToColor(surface.format, *(uint32_t*)dst);
                        c = BlendColors(newColor, c, a, invA);
                        *(uint32_t*)dst = ColorToPixel(surface.format, c);
                    } break;
                    default: break;
                }
            }
        }
    }
}

void DrawFontChar(Surface surface, int x, int y, char c, const Font* font, Color color) {
    const char s[2] = { c, '\0' };
    DrawFontText(surface, x, y, s, font, color);
}

void DrawFontText(Surface surface, int x, int y, const char* text, const Font* font, Color color) {
    if (text == NULL || color.a == 0) return;
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

    const int asc = (int)(face->size->metrics.ascender >> 6);
    const int desc = (int)(face->size->metrics.descender >> 6);
    int lineHeight = asc - desc;
    if (lineHeight < 0) lineHeight = -lineHeight;

    for (uint32_t i = 0; i < glyphCount; i++) {
        if (text[glyphInfo[i].cluster] == '\n') {
            penX = x;
            penY += lineHeight;
            continue;
        }

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
        THROW_ERROR(ERR_INVALID_PARAMS);
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
            const hb_glyph_position_t* pos = hb_buffer_get_glyph_positions(buf, &glyphCount);

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
