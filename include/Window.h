#ifndef LGL_WINDOW_H
#define LGL_WINDOW_H

#include <stdbool.h>

#include "Surface.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

Surface WindowInit(int width, int height, const char* title);
void WindowDestroy();
void WindowSetClose(bool close);
bool WindowShouldClose();
void WindowSetTitle(const char* title);
void WindowBeginFrame();
void WindowEndFrame();

void WindowSetTargetFPS(int fps);
float WindowGetFrameTime(void);
double WindowGetTime(void);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif // LGL_WINDOW_H
