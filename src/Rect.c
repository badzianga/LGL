#include "Rect.h"

bool RectIntersection(Rect a, Rect b, Rect* result) {
    const int x1 = (a.x > b.x) ? a.x : b.x;
    const int y1 = (a.y > b.y) ? a.y : b.y;
    const int x2 = ((a.x + a.width) < (b.x + b.width)) ? (a.x + a.width) : (b.x + b.width);
    const int y2 = ((a.y + a.height) < (b.y + b.height)) ? (a.y + a.height) : (b.y + b.height);

    if (x2 <= x1 || y2 <= y1) return false;

    if (result) {
        result->x = x1;
        result->y = y1;
        result->width = x2 - x1;
        result->height = y2 - y1;
    }

    return true;
}
