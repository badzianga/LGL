#pragma once
#include "Surface.h"

void TransformFlipX(Surface surface);
void TransformFlipY(Surface surface);
Surface TransformScale(Surface src, int destWidth, int destHeight);
Surface TransformScale2x(Surface original);
