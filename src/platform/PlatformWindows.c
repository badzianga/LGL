#include <assert.h>
#ifndef UNICODE
#define UNICODE
#endif//UNICODE
#include <windows.h>

#include "Input.h"
#include "Window.h"

typedef struct Platform {
    HWND hwnd;
    int shouldClose;
    Surface surface;
} Platform;

static Platform platform = { 0 };

extern int main(int argc, char** argv);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    return main(__argc, __argv);
}

static LRESULT CALLBACK WndProc(const HWND hwnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam) {
    switch (uMsg) {
        case WM_CLOSE: {
            platform.shouldClose = true;
            DestroyWindow(hwnd);
            return 0;
        }
        case WM_DESTROY: {
            PostQuitMessage(0);
            return 0;
        }
        default: {
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }
}

// --------------------------------------------------------------------------------------------------------------------

bool IsKeyPressed(KeyboardKey key) {
    assert(0 && "not implemented");
}

bool IsKeyDown(KeyboardKey key) {
    assert(0 && "not implemented");
}

bool IsKeyReleased(KeyboardKey key) {
    assert(0 && "not implemented");
}

int GetMouseX() {
    return 0;
}

int GetMouseY() {
    return 0;
}

void GetMousePosition(int* x, int* y) {
    assert(0 && "not implemented");
}

int GetMouseWheelMove() {
    assert(0 && "not implemented");
}

bool IsMouseButtonPressed(MouseButton button) {
    assert(0 && "not implemented");
}

bool IsMouseButtonDown(MouseButton button) {
    assert(0 && "not implemented");
}

bool IsMouseButtonReleased(MouseButton button) {
    assert(0 && "not implemented");
}

void SetMousePosition(int x, int y) {
    assert(0 && "not implemented");
}

void CursorShow() {
    assert(0 && "not implemented");
}

void CursorHide() {
    assert(0 && "not implemented");
}

bool IsCursorHidden() {
    assert(0 && "not implemented");
}

// --------------------------------------------------------------------------------------------------------------------

static wchar_t* MakeWideTitle(const char* title) {
    int len = MultiByteToWideChar(CP_UTF8, 0, title, -1, NULL, 0);
    wchar_t* wTitle = malloc(len * sizeof(wchar_t));
    MultiByteToWideChar(CP_UTF8, 0, title, -1, wTitle, len);
    return wTitle;
}

Surface WindowInit(int width, int height, const char* title) {
    const HINSTANCE hInstance = GetModuleHandle(NULL);

    const WNDCLASS wc = {
        .lpfnWndProc = WndProc,
        .hInstance = hInstance,
        .lpszClassName = L"LGL_Class"
    };
    RegisterClass(&wc);

    wchar_t* wTitle = MakeWideTitle(title);

    RECT rect = { 0, 0, width, height };
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
    const int winWidth = rect.right - rect.left;
    const int winHeight = rect.bottom - rect.top;

    platform.hwnd = CreateWindow(
        wc.lpszClassName,
        wTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        winWidth, winHeight,
        NULL, NULL, hInstance, NULL
    );
    free(wTitle);

    if (!platform.hwnd) {
        return (Surface){ 0 };
    }

    platform.surface = SurfaceCreate(width, height, &FORMAT_ARGB8888);

    ShowWindow(platform.hwnd, SW_SHOW);
    UpdateWindow(platform.hwnd);

    return platform.surface;
}

void WindowDestroy() {
    assert(0 && "not implemented");
}

void WindowSetClose(bool close) {
    platform.shouldClose = close;
}

bool WindowShouldClose() {
    return platform.shouldClose;
}

void WindowSetTitle(const char* title) {
    assert(0 && "not implemented");
}

void WindowBeginFrame() {
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        if  (msg.message == WM_QUIT) {
            platform.shouldClose = true;
        }
    }
}

void WindowEndFrame() {
    assert(0 && "not implemented");
}

void WindowSetTargetFPS(int fps) {
    assert(0 && "not implemented");
}

float WindowGetFrameTime(void) {
    assert(0 && "not implemented");
}

double WindowGetTime(void) {
    assert(0 && "not implemented");
}
