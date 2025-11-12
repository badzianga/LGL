#pragma once
#include <stdint.h>

typedef struct BitmapFont {
    int charWidth;
    int charHeight;
    const uint8_t* data;
    int firstChar;
    int lastChar;
} BitmapFont;
