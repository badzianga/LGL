#ifndef LGL_IMAGE_H
#define LGL_IMAGE_H

#include "Surface.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

Surface ImageLoad(const char* path);
void ImageSave(Surface image, const char* path);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // LGL_IMAGE_H
