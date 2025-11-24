#ifndef LGL_RECT_H
#define LGL_RECT_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct Rect {
    int x;
    int y;
    int width;
    int height;
} Rect;

bool RectIntersection(const Rect* a, const Rect* b, Rect* result);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // LGL_RECT_H
