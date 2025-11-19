#pragma once
#include "Surface.h"

typedef struct Font {
    void* internal;
    void* hbFont;
    int size;
} Font;

Font FontLoad(const char* path, int pixelSize);
void FontFree(Font* font);
void DrawFontChar(Surface surface, int x, int y, char c, Font* font, Color color);
void DrawFontText(Surface surface, int x, int y, const char* text, Font* font, Color color);
void ShutdownFontModule();
