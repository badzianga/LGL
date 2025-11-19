#ifndef LGL_FILL_RECT_H
#define LGL_FILL_RECT_H

#include "Surface.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

void FillRect(Surface surface, int x, int y, int w, int h, Color color);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif // LGL_FILL_RECT_H
