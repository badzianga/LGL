#pragma once
#include <stdbool.h>

#include "Surface.h"

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
