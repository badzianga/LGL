#include <assert.h>
#ifndef UNICODE
#define UNICODE
#endif//UNICODE
#include <windows.h>

#include "Input.h"
#include "Window.h"

typedef struct Platform {
    HWND hwnd;
    BITMAPINFO bmpInfo;
    int shouldClose;
    Surface surface;
} Platform;

typedef struct TimeHandling {
    LARGE_INTEGER freq;
    LARGE_INTEGER startTime;
    LARGE_INTEGER lastFrameTime;

    double time;
    float frameTime;
    int targetFPS;
    double targetFrameTime;
} TimeHandling;

typedef struct MouseState {
    bool cursorHidden;
} MouseState;

static Platform platform = { 0 };
static TimeHandling timeHandling = { 0 };
static MouseState mouse = { 0 };

extern int main(int argc, char** argv);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    (void)hInstance,(void)hPrevInstance, (void)pCmdLine, (void)nCmdShow;
    return main(__argc, __argv);
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CLOSE: {
            platform.shouldClose = true;
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
    if (!mouse.cursorHidden) return;
    while (ShowCursor(TRUE) < 0) {}
    mouse.cursorHidden = false;
}

void CursorHide() {
    if (mouse.cursorHidden) return;
    while (ShowCursor(FALSE) >= 0) {}
    mouse.cursorHidden = true;
}

bool IsCursorHidden() {
    return mouse.cursorHidden;
}

// --------------------------------------------------------------------------------------------------------------------

static void InitTimer();
static void FrameTick();

static wchar_t* MakeWideTitle(const char* title) {
    const int len = MultiByteToWideChar(CP_UTF8, 0, title, -1, NULL, 0);
    wchar_t* wTitle = malloc(len * sizeof(wchar_t));
    MultiByteToWideChar(CP_UTF8, 0, title, -1, wTitle, len);
    return wTitle;
}

Surface WindowInit(int width, int height, const char* title) {
    HINSTANCE hInstance = GetModuleHandle(NULL);

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

    platform.bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    platform.bmpInfo.bmiHeader.biWidth = width;
    platform.bmpInfo.bmiHeader.biHeight = -height;
    platform.bmpInfo.bmiHeader.biPlanes = 1;
    platform.bmpInfo.bmiHeader.biBitCount = 32;
    platform.bmpInfo.bmiHeader.biCompression = BI_RGB;

    timeBeginPeriod(1);
    InitTimer();

    return platform.surface;
}

void WindowDestroy() {
    timeEndPeriod(1);
    DestroyWindow(platform.hwnd);
}

void WindowSetClose(bool close) {
    platform.shouldClose = close;
}

bool WindowShouldClose() {
    return platform.shouldClose;
}

void WindowSetTitle(const char* title) {
    if (!platform.hwnd) return;

    wchar_t* wTitle = MakeWideTitle(title);
    SetWindowTextW(platform.hwnd, wTitle);
    free(wTitle);
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
    HDC dc = GetDC(platform.hwnd);

    StretchDIBits(dc,
        0, 0, platform.surface.width, platform.surface.height,
        0, 0, platform.surface.width, platform.surface.height,
        platform.surface.pixels,
        &platform.bmpInfo,
        DIB_RGB_COLORS,
        SRCCOPY
    );

    ReleaseDC(platform.hwnd, dc);

    FrameTick();
}

// --------------------------------------------------------------------------------------------------------------------

static void InitTimer() {
    QueryPerformanceFrequency(&timeHandling.freq);
    QueryPerformanceCounter(&timeHandling.startTime);
    timeHandling.lastFrameTime = timeHandling.startTime;
    timeHandling.time = 0.0;
}

static void FrameTick() {
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);

    const double elapsed = (double)(now.QuadPart - timeHandling.lastFrameTime.QuadPart) / (double)timeHandling.freq.QuadPart;
    if (timeHandling.targetFPS > 0 && elapsed < timeHandling.targetFrameTime) {
        const double remaining = timeHandling.targetFrameTime - elapsed;

        const DWORD ms = (DWORD)(remaining * 1000.0);

        if (ms > 0) {
            Sleep(ms);
        }

        while (true) {
            QueryPerformanceCounter(&now);
            const double nowElapsed =
                (double)(now.QuadPart - timeHandling.lastFrameTime.QuadPart) / (double)timeHandling.freq.QuadPart;
            if (nowElapsed >= timeHandling.targetFrameTime) break;
        }
    }

    QueryPerformanceCounter(&now);
    timeHandling.frameTime =
        (float)((double)(now.QuadPart - timeHandling.lastFrameTime.QuadPart) / (double)timeHandling.freq.QuadPart);
    timeHandling.lastFrameTime = now;

    timeHandling.time = (double)(now.QuadPart - timeHandling.startTime.QuadPart) / (double)timeHandling.freq.QuadPart;
}

void WindowSetTargetFPS(int fps) {
    timeHandling.targetFPS = fps;
    timeHandling.targetFrameTime = (fps > 0) ? 1.0 / (double)fps : 0.0;
}

float WindowGetFrameTime(void) {
    return timeHandling.frameTime;
}

double WindowGetTime(void) {
    return timeHandling.time;
}
