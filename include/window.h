#pragma once
#include <stdbool.h>
#include "surface.h"

Surface WindowInit(int width, int height, const char* title);
void WindowDestroy();
void WindowSetClose(bool close);
bool WindowShouldClose();
void WindowSetTitle(const char* title);
void WindowBeginFrame();
void WindowEndFrame();
