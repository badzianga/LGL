#pragma once
#include "Surface.h"

void TransformFlipX(Surface surface);
void TransformFlipY(Surface surface);
Surface TransformRotate(Surface src, float angle);  // TODO: don't use floats, for now I can't come up with better impl
Surface TransformScale(Surface src, int destWidth, int destHeight);
Surface TransformScale2x(Surface original);
