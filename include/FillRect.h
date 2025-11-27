#ifndef LGL_FILL_RECT_H
#define LGL_FILL_RECT_H

#include "Rect.h"
#include "Surface.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

void FillRect(Surface surface, const Rect* rect, uint32_t color);
void BlendFillRect(Surface surface, const Rect* rect, Color color);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif // LGL_FILL_RECT_H
