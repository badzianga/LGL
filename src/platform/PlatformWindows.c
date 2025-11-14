#include <windows.h>

#include "Input.h"
#include "Window.h"

typedef struct Platform {

} Platform;

// --------------------------------------------------------------------------------------------------------------------

bool IsKeyPressed(KeyboardKey key) {

}

bool IsKeyDown(KeyboardKey key) {

}

bool IsKeyReleased(KeyboardKey key) {

}

int GetMouseX() {

}

int GetMouseY() {

}

void GetMousePosition(int* x, int* y) {

}

int GetMouseWheelMove() {

}

bool IsMouseButtonPressed(MouseButton button) {

}

bool IsMouseButtonDown(MouseButton button) {

}

bool IsMouseButtonReleased(MouseButton button) {

}

void SetMousePosition(int x, int y) {

}

void CursorShow() {

}

void CursorHide() {

}

bool IsCursorHidden() {

}

// --------------------------------------------------------------------------------------------------------------------

Surface WindowInit(int width, int height, const char* title) {

}

void WindowDestroy() {

}

void WindowSetClose(bool close) {

}

bool WindowShouldClose() {

}

void WindowSetTitle(const char* title) {

}

void WindowBeginFrame() {

}

void WindowEndFrame() {

}

void WindowSetTargetFPS(int fps) {

}

float WindowGetFrameTime(void) {

}

double WindowGetTime(void) {

}
