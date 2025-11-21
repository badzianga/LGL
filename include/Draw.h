#ifndef LGL_DRAW_H
#define LGL_DRAW_H

#include "Surface.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

void DrawRect(Surface surface, int x, int y, int w, int h, Color color);
void DrawCircle(Surface surface, int x, int y, int r, Color color);
void DrawTriangle(Surface surface, int x1, int y1, int x2, int y2, int x3, int y3, Color color);
void DrawLine(Surface surface, int x1, int y1, int x2, int y2, Color color);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // LGL_DRAW_H
