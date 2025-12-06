#ifndef LGL_BITMAP_FONT_H
#define LGL_BITMAP_FONT_H

#include <stdint.h>

#include "Surface.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct BitmapFont {
    int charWidth;
    int charHeight;
    const uint8_t* data;
    int firstChar;
    int lastChar;
} BitmapFont;

extern const BitmapFont DEFAULT_BITMAP_FONT;

void DrawCharBitmapFont(Surface surface, int x, int y, char c, const BitmapFont* font, Color color);
void DrawTextBitmapFont(Surface surface, int x, int y, const char* text, const BitmapFont* font, Color color);
void MeasureBitmapFontText(const char* text, const BitmapFont* font, int* outWidth, int* outHeight);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif // LGL_BITMAP_FONT_H
