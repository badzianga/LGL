#include "BlendFillRect.h"
#include "FillRect.h"

// alpha blending works ONLY for 4-byte colors with alpha channel
void BlendFillRect(Surface surface, const Rect* rect, Color color) {
    // fallback to FillRect
    if (!surface.format->aMask) return FillRect(surface, rect, ColorToPixel(surface.format, color));

    const Rect surfaceRect = { 0, 0, surface.width, surface.height };
    Rect clipped;

    if (!RectIntersection(&surfaceRect, rect, &clipped)) return;

    uint8_t* row = (uint8_t*)surface.pixels + clipped.y * surface.stride + clipped.x * surface.format->bytesPerPixel;

    const uint32_t a = color.a;
    const uint32_t invA = 255 - a;

    int h = rect->height;
    while (h--) {
        int w = rect->width;
        uint32_t* ptr = (uint32_t*)row;
        while (w--) {
            Color c = PixelToColor(surface.format, *ptr);
            c.r = (color.r * a + c.r * invA) / 255;
            c.g = (color.g * a + c.g * invA) / 255;
            c.b = (color.b * a + c.b * invA) / 255;
            c.a = 255;

            *ptr++ = ColorToPixel(surface.format, c);
        }
        row += surface.stride;
    }
}
