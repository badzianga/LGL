#include "BitmapFont.h"
#include "internal/Inlines.h"
#include "PixelFormat.h"

// Based on Thick 8x8 (https://frostyfreeze.itch.io/pixel-bitmap-fonts-png-xml) 
static const uint8_t defaultBitmapFontData[] = {
    // 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,  //
    0b11000000, 0b11000000, 0b11000000, 0b11000000, 0b11000000, 0b00000000, 0b11000000, 0b00000000,  // !
    0b11011000, 0b11011000, 0b11011000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,  // "
    0b01101100, 0b01101100, 0b11111110, 0b01101100, 0b11111110, 0b01101100, 0b01101100, 0b00000000,  // #
    0b00110000, 0b01111100, 0b11000000, 0b01111000, 0b00001100, 0b11111000, 0b00110000, 0b00000000,  // $
    0b11000110, 0b11001100, 0b00011000, 0b00110000, 0b01100110, 0b11000110, 0b00000000, 0b00000000,  // %
    0b01110000, 0b11011000, 0b01110000, 0b01110000, 0b11011100, 0b11001000, 0b01111100, 0b00000000,  // &
    0b11000000, 0b11000000, 0b11000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,  // '
    0b00001100, 0b00011000, 0b00011000, 0b00011000, 0b00011000, 0b00011000, 0b00001100, 0b00000000,  // (
    0b11000000, 0b01100000, 0b01100000, 0b01100000, 0b01100000, 0b01100000, 0b11000000, 0b00000000,  // )
    0b00000000, 0b01101100, 0b00111000, 0b11111110, 0b00111000, 0b01101100, 0b00000000, 0b00000000,  // *
    0b00000000, 0b00110000, 0b00110000, 0b11111100, 0b00110000, 0b00110000, 0b00000000, 0b00000000,  // +
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b01100000, 0b01100000, 0b11000000,  // ,
    0b00000000, 0b00000000, 0b00000000, 0b11111100, 0b00000000, 0b00000000, 0b00000000, 0b00000000,  // -
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b11000000, 0b11000000, 0b00000000,  // .
    0b00110000, 0b00110000, 0b01100000, 0b01100000, 0b01100000, 0b11000000, 0b11000000, 0b00000000,  // /

    0b01111000, 0b11001100, 0b11001100, 0b11001100, 0b11001100, 0b11001100, 0b01111000, 0b00000000,  // 0
    0b00110000, 0b01110000, 0b00110000, 0b00110000, 0b00110000, 0b00110000, 0b01111000, 0b00000000,  // 1
    0b01111000, 0b11001100, 0b00001100, 0b00011000, 0b00110000, 0b01100000, 0b11111100, 0b00000000,  // 2
    0b01111000, 0b11001100, 0b00001100, 0b00111000, 0b00001100, 0b11001100, 0b01111000, 0b00000000,  // 3
    0b11001100, 0b11001100, 0b11001100, 0b11111100, 0b00001100, 0b00001100, 0b00001100, 0b00000000,  // 4
    0b11111100, 0b11000000, 0b11000000, 0b01111000, 0b00001100, 0b11001100, 0b01111000, 0b00000000,  // 5
    0b01111000, 0b11001100, 0b11000000, 0b11111000, 0b11001100, 0b11001100, 0b01111000, 0b00000000,  // 6
    0b11111100, 0b00001100, 0b00011000, 0b00011000, 0b00110000, 0b00110000, 0b00110000, 0b00000000,  // 7
    0b01111000, 0b11001100, 0b11001100, 0b01111000, 0b11001100, 0b11001100, 0b01111000, 0b00000000,  // 8
    0b01111000, 0b11001100, 0b11001100, 0b01111100, 0b00001100, 0b11001100, 0b01111000, 0b00000000,  // 9
    0b00000000, 0b11000000, 0b11000000, 0b00000000, 0b11000000, 0b11000000, 0b00000000, 0b00000000,  // :
    0b00000000, 0b11000000, 0b11000000, 0b00000000, 0b11000000, 0b11000000, 0b11000000, 0b11000000,  // ;
    0b00011000, 0b00110000, 0b01100000, 0b11000000, 0b01100000, 0b00110000, 0b00011000, 0b00000000,  // <
    0b00000000, 0b00000000, 0b11111100, 0b00000000, 0b11111100, 0b00000000, 0b00000000, 0b00000000,  // =
    0b01100000, 0b00110000, 0b00011000, 0b00001100, 0b00011000, 0b00110000, 0b01100000, 0b00000000,  // >
    0b01111000, 0b11001100, 0b00001100, 0b00011000, 0b00110000, 0b00000000, 0b00110000, 0b00000000,  // ?

    0b01111000, 0b11001100, 0b11001100, 0b11011100, 0b11011100, 0b11000000, 0b01111100, 0b00000000,  // @
    0b00110000, 0b01111000, 0b11001100, 0b11001100, 0b11111100, 0b11001100, 0b11001100, 0b00000000,  // A
    0b11111000, 0b11001100, 0b11001100, 0b11111000, 0b11001100, 0b11001100, 0b11111000, 0b00000000,  // B
    0b01111000, 0b11001100, 0b11000000, 0b11000000, 0b11000000, 0b11001100, 0b01111000, 0b00000000,  // C
    0b11111000, 0b11001100, 0b11001100, 0b11001100, 0b11001100, 0b11001100, 0b11111000, 0b00000000,  // D
    0b11111100, 0b11000000, 0b11000000, 0b11110000, 0b11000000, 0b11000000, 0b11111100, 0b00000000,  // E
    0b11111100, 0b11000000, 0b11000000, 0b11110000, 0b11000000, 0b11000000, 0b11000000, 0b00000000,  // F
    0b01111100, 0b11000000, 0b11000000, 0b11000000, 0b11001100, 0b11001100, 0b01111100, 0b00000000,  // G
    0b11001100, 0b11001100, 0b11001100, 0b11111100, 0b11001100, 0b11001100, 0b11001100, 0b00000000,  // H
    0b11111100, 0b00110000, 0b00110000, 0b00110000, 0b00110000, 0b00110000, 0b11111100, 0b00000000,  // I
    0b00001100, 0b00001100, 0b00001100, 0b00001100, 0b11001100, 0b11001100, 0b01111000, 0b00000000,  // J
    0b11001100, 0b11001100, 0b11011000, 0b11110000, 0b11011000, 0b11001100, 0b11001100, 0b00000000,  // K
    0b11000000, 0b11000000, 0b11000000, 0b11000000, 0b11000000, 0b11000000, 0b11111100, 0b00000000,  // L
    0b11000110, 0b11101110, 0b11111110, 0b11010110, 0b11000110, 0b11000110, 0b11000110, 0b00000000,  // M
    0b11001100, 0b11101100, 0b11111100, 0b11011100, 0b11001100, 0b11001100, 0b11001100, 0b00000000,  // N
    0b01111000, 0b11001100, 0b11001100, 0b11001100, 0b11001100, 0b11001100, 0b01111000, 0b00000000,  // O

    0b11111000, 0b11001100, 0b11001100, 0b11001100, 0b11111000, 0b11000000, 0b11000000, 0b00000000,  // P
    0b01111000, 0b11001100, 0b11001100, 0b11001100, 0b11001100, 0b11001100, 0b01111000, 0b00011100,  // Q
    0b11111000, 0b11001100, 0b11001100, 0b11111000, 0b11001100, 0b11001100, 0b11001100, 0b00000000,  // R
    0b01111100, 0b11000000, 0b11000000, 0b01111000, 0b00001100, 0b00001100, 0b11111000, 0b00000000,  // S
    0b11111100, 0b00110000, 0b00110000, 0b00110000, 0b00110000, 0b00110000, 0b00110000, 0b00000000,  // T
    0b11001100, 0b11001100, 0b11001100, 0b11001100, 0b11001100, 0b11001100, 0b01111000, 0b00000000,  // U
    0b11001100, 0b11001100, 0b11001100, 0b01111000, 0b01111000, 0b00110000, 0b00110000, 0b00000000,  // V
    0b11000110, 0b11000110, 0b11000110, 0b11010110, 0b11111110, 0b11101110, 0b11000110, 0b00000000,  // W
    0b11001100, 0b11001100, 0b01111000, 0b00110000, 0b01111000, 0b11001100, 0b11001100, 0b00000000,  // X
    0b11001100, 0b11001100, 0b11001100, 0b01111000, 0b00110000, 0b00110000, 0b00110000, 0b00000000,  // Y
    0b11111100, 0b00001100, 0b00011000, 0b00110000, 0b01100000, 0b11000000, 0b11111100, 0b00000000,  // Z
    0b00111100, 0b00110000, 0b00110000, 0b00110000, 0b00110000, 0b00110000, 0b00111100, 0b00000000,  // [
    0b11000000, 0b11000000, 0b01100000, 0b01100000, 0b01100000, 0b00110000, 0b00110000, 0b00000000,  // \ /
    0b11110000, 0b00110000, 0b00110000, 0b00110000, 0b00110000, 0b00110000, 0b11110000, 0b00000000,  // ]
    0b00010000, 0b00111000, 0b01101100, 0b11000110, 0b00000000, 0b00000000, 0b00000000, 0b00000000,  // ^
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b11111100, 0b00000000,  // _

    0b00110000, 0b00110000, 0b00011000, 0b00001100, 0b00000000, 0b00000000, 0b00000000, 0b00000000,  // `
    0b00000000, 0b00000000, 0b01111000, 0b00001100, 0b01111100, 0b11001100, 0b01111100, 0b00000000,  // a
    0b11000000, 0b11000000, 0b11111000, 0b11001100, 0b11001100, 0b11001100, 0b11111000, 0b00000000,  // b
    0b00000000, 0b00000000, 0b01111100, 0b11000000, 0b11000000, 0b11000000, 0b01111100, 0b00000000,  // c
    0b00001100, 0b00001100, 0b01111100, 0b11001100, 0b11001100, 0b11001100, 0b01111100, 0b00000000,  // d
    0b00000000, 0b00000000, 0b01111000, 0b11001100, 0b11111100, 0b11000000, 0b01111100, 0b00000000,  // e
    0b00000000, 0b00111000, 0b01100000, 0b11111000, 0b01100000, 0b01100000, 0b01100000, 0b00000000,  // f
    0b00000000, 0b00000000, 0b01111100, 0b11001100, 0b11001100, 0b01111100, 0b00001100, 0b01111000,  // g
    0b11000000, 0b11000000, 0b11111000, 0b11001100, 0b11001100, 0b11001100, 0b11001100, 0b00000000,  // h
    0b00011000, 0b00000000, 0b00011000, 0b00011000, 0b00011000, 0b00011000, 0b00011000, 0b00000000,  // i
    0b00011000, 0b00000000, 0b00011000, 0b00011000, 0b00011000, 0b00011000, 0b00011000, 0b11110000,  // j
    0b11000000, 0b11000000, 0b11001100, 0b11011000, 0b11110000, 0b11011000, 0b11001100, 0b00000000,  // k
    0b00110000, 0b00110000, 0b00110000, 0b00110000, 0b00110000, 0b00110000, 0b00110000, 0b00000000,  // l
    0b00000000, 0b00000000, 0b11101100, 0b11111110, 0b11010110, 0b11000110, 0b11000110, 0b00000000,  // m
    0b00000000, 0b00000000, 0b11111000, 0b11001100, 0b11001100, 0b11001100, 0b11001100, 0b00000000,  // n
    0b00000000, 0b00000000, 0b01111000, 0b11001100, 0b11001100, 0b11001100, 0b01111000, 0b00000000,  // o

    0b00000000, 0b00000000, 0b11111000, 0b11001100, 0b11001100, 0b11111000, 0b11000000, 0b11000000,  // p
    0b00000000, 0b00000000, 0b01111100, 0b11001100, 0b11001100, 0b01111100, 0b00001100, 0b00001100,  // q
    0b00000000, 0b00000000, 0b11111000, 0b11001100, 0b11000000, 0b11000000, 0b11000000, 0b00000000,  // r
    0b00000000, 0b00000000, 0b01111100, 0b11000000, 0b01111000, 0b00001100, 0b11111000, 0b00000000,  // s
    0b00000000, 0b00110000, 0b11111100, 0b00110000, 0b00110000, 0b00110000, 0b00011100, 0b00000000,  // t
    0b00000000, 0b00000000, 0b11001100, 0b11001100, 0b11001100, 0b11001100, 0b01111100, 0b00000000,  // u
    0b00000000, 0b00000000, 0b11001100, 0b11001100, 0b11001100, 0b01111000, 0b00110000, 0b00000000,  // v
    0b00000000, 0b00000000, 0b11000110, 0b11000110, 0b11010110, 0b11111110, 0b01101100, 0b00000000,  // w
    0b00000000, 0b00000000, 0b11001100, 0b01111000, 0b00110000, 0b01111000, 0b11001100, 0b00000000,  // x
    0b00000000, 0b00000000, 0b11001100, 0b11001100, 0b11001100, 0b01111100, 0b00001100, 0b11111000,  // y
    0b00000000, 0b00000000, 0b11111100, 0b00011000, 0b00110000, 0b01100000, 0b11111100, 0b00000000,  // z
    0b00110000, 0b01100000, 0b01100000, 0b11000000, 0b01100000, 0b01100000, 0b00110000, 0b00000000,  // {
    0b00110000, 0b00110000, 0b00110000, 0b00110000, 0b00110000, 0b00110000, 0b00110000, 0b00110000,  // |
    0b00110000, 0b00011000, 0b00011000, 0b00001100, 0b00011000, 0b00011000, 0b00110000, 0b00000000,  // }
    0b00000000, 0b00000000, 0b00000000, 0b01100110, 0b11011011, 0b11001100, 0b00000000, 0b00000000,  // ~
};

const BitmapFont DEFAULT_BITMAP_FONT = {
    .charWidth = 8,
    .charHeight = 8,
    .data = defaultBitmapFontData,
    .firstChar = '!',
    .lastChar = '~',
};

void DrawCharBitmapFont(Surface surface, int x, int y, char c, const BitmapFont* font, Color color) {
    const char s[2] = { c, '\0' };
    DrawTextBitmapFont(surface, x, y, s, font, color);
}

// This is bad, lots of code is duplicated, but it's hard for me to make it clean and performant
static void FillText(Surface surface, int x, int y, const char* text, const BitmapFont* font, uint32_t colorValue) {
    const int charW = font->charWidth;
    const int charH = font->charHeight;

    const int bpp = surface.format->bytesPerPixel;

    int cursorX = x;
    int cursorY = y;

    char c;
    while ((c = *text++)) {
        if (c == '\n') {
            cursorX = x;
            cursorY += charH;
            continue;
        }

        const int glyphIndex = c - font->firstChar;
        const int glyphX = cursorX;
        const int glyphY = cursorY;

        const int left = glyphX;
        const int top = glyphY;
        const int right = glyphX + charW;
        const int bottom = glyphY + charH;

        // glyph completely offscreen
        if (right <= 0 || left >= surface.width || bottom <= 0 || top >= surface.height) {
            cursorX += charW;
            continue;
        }

        // glyph is fully inside the surface
        if (left >= 0 && top >= 0 && right <= surface.width && bottom <= surface.height) {
            for (int gy = 0; gy < charH; ++gy) {
                const uint8_t rowBits = font->data[glyphIndex * charH + gy];
                const int dstY = glyphY + gy;
                const int dstIndex = dstY * surface.stride + glyphX * bpp;

                for (int gx = 0; gx < charW; ++gx) {
                    if (rowBits & (1 << (7 - gx))) {
                        uint8_t* pixel = (uint8_t*)surface.pixels + dstIndex + gx * bpp;
                        switch (bpp) {
                            case 1: {
                                *pixel = (uint8_t)colorValue;
                            } break;
                            case 2: {
                                *(uint16_t*)pixel = (uint16_t)colorValue;
                            } break;
                            case 4: {
                                *(uint32_t*)pixel = colorValue;
                            } break;
                            default: break;
                        }
                    }
                }
            }
        }
        // glyph is partially visible
        else {
            const int startX = left < 0 ? 0 : left;
            const int endX = right  > surface.width ? surface.width : right;

            const int startY = top < 0 ? 0 : top;
            const int endY = bottom > surface.height ? surface.height : bottom;

            for (int py = startY; py < endY; ++py) {
                const int gy = py - glyphY;
                const uint8_t rowBits = font->data[glyphIndex * charH + gy];

                for (int px = startX; px < endX; ++px) {
                    const int gx = px - glyphX;

                    if (rowBits & (1 << (7 - gx))) {
                        uint8_t* pixel = (uint8_t*)surface.pixels + py * surface.stride + px * bpp;
                        switch (bpp) {
                            case 1: {
                                *pixel = (uint8_t)colorValue;
                            } break;
                            case 2: {
                                *(uint16_t*)pixel = (uint16_t)colorValue;
                            } break;
                            case 4: {
                                *(uint32_t*)pixel = colorValue;
                            } break;
                            default: break;
                        }
                    }
                }
            }
        }
        cursorX += charW;
    }
}

static void BlendText(Surface surface, int x, int y, const char* text, const BitmapFont* font, Color color) {
    const int charW = font->charWidth;
    const int charH = font->charHeight;

    const int bpp = surface.format->bytesPerPixel;

    int cursorX = x;
    int cursorY = y;

    char c;
    while ((c = *text++)) {
        if (c == '\n') {
            cursorX = x;
            cursorY += charH;
            continue;
        }

        const int glyphIndex = c - font->firstChar;
        const int glyphX = cursorX;
        const int glyphY = cursorY;

        const int left = glyphX;
        const int top = glyphY;
        const int right = glyphX + charW;
        const int bottom = glyphY + charH;

        // glyph completely offscreen
        if (right <= 0 || left >= surface.width || bottom <= 0 || top >= surface.height) {
            cursorX += charW;
            continue;
        }

        const int a = color.a;
        const int invA = 255 - a;

        // glyph is fully inside the surface
        if (left >= 0 && top >= 0 && right <= surface.width && bottom <= surface.height) {
            for (int gy = 0; gy < charH; ++gy) {
                const uint8_t rowBits = font->data[glyphIndex * charH + gy];
                const int dstY = glyphY + gy;
                const int dstIndex = dstY * surface.stride + glyphX * bpp;

                for (int gx = 0; gx < charW; ++gx) {
                    if (rowBits & (1 << (7 - gx))) {
                        uint8_t* pixel = (uint8_t*)surface.pixels + dstIndex + gx * bpp;
                        switch (bpp) {
                            case 1: {
                                Color dst = PixelToColor(surface.format, *pixel);
                                dst = BlendColors(color, dst, a, invA);
                                *pixel = (uint8_t)ColorToPixel(surface.format, dst);
                            } break;
                            case 2: {
                                Color dst = PixelToColor(surface.format, *(uint16_t*)pixel);
                                dst = BlendColors(color, dst, a, invA);
                                *(uint16_t*)pixel = (uint16_t)ColorToPixel(surface.format, dst);
                            } break;
                            case 4: {
                                Color dst = PixelToColor(surface.format, *(uint32_t*)pixel);
                                dst = BlendColors(color, dst, a, invA);
                                *(uint32_t*)pixel = ColorToPixel(surface.format, dst);
                            } break;
                            default: break;
                        }
                    }
                }
            }
        }
        // glyph is partially visible
        else {
            const int startX = left < 0 ? 0 : left;
            const int endX = right  > surface.width ? surface.width : right;

            const int startY = top < 0 ? 0 : top;
            const int endY = bottom > surface.height ? surface.height : bottom;

            for (int py = startY; py < endY; ++py) {
                const int gy = py - glyphY;
                const uint8_t rowBits = font->data[glyphIndex * charH + gy];

                for (int px = startX; px < endX; ++px) {
                    const int gx = px - glyphX;

                    if (rowBits & (1 << (7 - gx))) {
                        uint8_t* pixel = (uint8_t*)surface.pixels + py * surface.stride + px * bpp;
                        switch (bpp) {
                            case 1: {
                                Color dst = PixelToColor(surface.format, *pixel);
                                dst = BlendColors(color, dst, a, invA);
                                *pixel = (uint8_t)ColorToPixel(surface.format, dst);
                            } break;
                            case 2: {
                                Color dst = PixelToColor(surface.format, *(uint16_t*)pixel);
                                dst = BlendColors(color, dst, a, invA);
                                *(uint16_t*)pixel = (uint16_t)ColorToPixel(surface.format, dst);
                            } break;
                            case 4: {
                                Color dst = PixelToColor(surface.format, *(uint32_t*)pixel);
                                dst = BlendColors(color, dst, a, invA);
                                *(uint32_t*)pixel = ColorToPixel(surface.format, dst);
                            } break;
                            default: break;
                        }
                    }
                }
            }
        }
        cursorX += charW;
    }
}

void DrawTextBitmapFont(Surface surface, int x, int y, const char* text, const BitmapFont* font, Color color) {
    if (!surface.pixels || !surface.format || !text || !font || color.a == 0) return;

    if (color.a == 255) {
        FillText(surface, x, y, text, font, ColorToPixel(surface.format, color));
    }
    else {
        BlendText(surface, x, y, text, font, color);
    }
}

void MeasureBitmapFontText(const char* text, const BitmapFont* font, int* outWidth, int* outHeight) {
    if (!text || !font || !font->data) {
        if (outWidth) *outWidth = 0;
        if (outHeight) *outHeight = 0;
        return;
    }

    int width = 0;
    int maxWidth = 0;
    int lines = (*text != '\0');

    char c;
    while ((c = *text++)) {
        if (c == '\n') {
            if (width > maxWidth) maxWidth = width;
            width = 0;
            lines++;
            continue;
        }
        width += font->charWidth;
    }

    if (outWidth) *outWidth = maxWidth;
    if (outHeight) *outHeight = font->charHeight * lines;
}
