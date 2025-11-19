#ifndef LGL_TRANSFORM_H
#define LGL_TRANSFORM_H

#include "Surface.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

void TransformFlipX(Surface surface);
void TransformFlipY(Surface surface);
Surface TransformRotate(Surface src, float angle);  // TODO: don't use floats, for now I can't come up with better impl
Surface TransformScale(Surface src, int destWidth, int destHeight);
Surface TransformScale2x(Surface original);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif // LGL_TRANSFORM_H
