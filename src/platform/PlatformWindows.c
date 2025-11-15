#include <assert.h>
#ifndef UNICODE
#define UNICODE
#endif//UNICODE
#include <windows.h>

#include "Input.h"
#include "Window.h"

#define MAX_KEYS 256
#define MAX_MOUSE_BUTTONS 8

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

typedef struct KeyboardState {
    bool down[MAX_KEYS];
    bool pressed[MAX_KEYS];
    bool released[MAX_KEYS];
} KeyboardState;

typedef struct MouseState {
    struct {
        bool down[MAX_MOUSE_BUTTONS];
        bool pressed[MAX_MOUSE_BUTTONS];
        bool released[MAX_MOUSE_BUTTONS];
    } buttons;

    int x;
    int y;

    bool cursorHidden;

    char wheelMove;
} MouseState;

static Platform platform = { 0 };
static TimeHandling timeHandling = { 0 };
static KeyboardState keys = { 0 };
static MouseState mouse = { 0 };

extern int main(int argc, char** argv);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    (void)hInstance,(void)hPrevInstance, (void)pCmdLine, (void)nCmdShow;
    return main(__argc, __argv);
}

static KeyboardKey MapKeyCode(WPARAM vk, LPARAM lParam);

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
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN: {
            const KeyboardKey key = MapKeyCode(wParam, lParam);
            const bool isRepeat = (lParam & 0x40000000) != 0;
            if (!isRepeat) keys.pressed[key] = true;
            keys.down[key] = true;
            return 0;
        }
        case WM_KEYUP:
        case WM_SYSKEYUP: {
            const KeyboardKey key = MapKeyCode(wParam, lParam);
            keys.down[key] = false;
            keys.released[key] = true;
            return 0;
        }
        case WM_MOUSEMOVE: {
            mouse.x = LOWORD(lParam);
            mouse.y = HIWORD(lParam);
            return 0;
        }
        case WM_LBUTTONDOWN: {
            mouse.buttons.down[MOUSE_BUTTON_LEFT] = true;
            mouse.buttons.pressed[MOUSE_BUTTON_LEFT] = true;
            return 0;
        }
        case WM_LBUTTONUP: {
            mouse.buttons.down[MOUSE_BUTTON_LEFT] = false;
            mouse.buttons.released[MOUSE_BUTTON_LEFT] = true;
            return 0;
        }
        case WM_RBUTTONDOWN: {
            mouse.buttons.down[MOUSE_BUTTON_RIGHT] = true;
            mouse.buttons.pressed[MOUSE_BUTTON_RIGHT] = true;
            return 0;
        }
        case WM_RBUTTONUP: {
            mouse.buttons.down[MOUSE_BUTTON_RIGHT] = false;
            mouse.buttons.released[MOUSE_BUTTON_RIGHT] = true;
            return 0;
        }
        case WM_MBUTTONDOWN: {
            mouse.buttons.down[MOUSE_BUTTON_MIDDLE] = true;
            mouse.buttons.pressed[MOUSE_BUTTON_MIDDLE] = true;
            return 0;
        }
        case WM_MBUTTONUP: {
            mouse.buttons.down[MOUSE_BUTTON_MIDDLE] = false;
            mouse.buttons.released[MOUSE_BUTTON_MIDDLE] = true;
            return 0;
        }
        case WM_MOUSEWHEEL: {
            SHORT delta = GET_WHEEL_DELTA_WPARAM(wParam);
            if (delta > 0) mouse.wheelMove++;
            else if (delta < 0) mouse.wheelMove--;
            return 0;
        }
        default: {
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }
}

// --------------------------------------------------------------------------------------------------------------------

static KeyboardKey MapKeyCode(WPARAM vk, LPARAM lParam) {
    const bool extended = (lParam >> 24) & 1;

    switch (vk) {
        case 'A':           return KEY_A;
        case 'B':           return KEY_B;
        case 'C':           return KEY_C;
        case 'D':           return KEY_D;
        case 'E':           return KEY_E;
        case 'F':           return KEY_F;
        case 'G':           return KEY_G;
        case 'H':           return KEY_H;
        case 'I':           return KEY_I;
        case 'J':           return KEY_J;
        case 'K':           return KEY_K;
        case 'L':           return KEY_L;
        case 'M':           return KEY_M;
        case 'N':           return KEY_N;
        case 'O':           return KEY_O;
        case 'P':           return KEY_P;
        case 'Q':           return KEY_Q;
        case 'R':           return KEY_R;
        case 'S':           return KEY_S;
        case 'T':           return KEY_T;
        case 'U':           return KEY_U;
        case 'V':           return KEY_V;
        case 'W':           return KEY_W;
        case 'X':           return KEY_X;
        case 'Y':           return KEY_Y;
        case 'Z':           return KEY_Z;

        case '1':           return KEY_1;
        case '2':           return KEY_2;
        case '3':           return KEY_3;
        case '4':           return KEY_4;
        case '5':           return KEY_5;
        case '6':           return KEY_6;
        case '7':           return KEY_7;
        case '8':           return KEY_8;
        case '9':           return KEY_9;
        case '0':           return KEY_0;

        case VK_RETURN:     return extended ? KEY_KP_ENTER : KEY_ENTER;
        case VK_ESCAPE:     return KEY_ESCAPE;
        case VK_BACK:       return KEY_BACKSPACE;
        case VK_TAB:        return KEY_TAB;
        case VK_SPACE:      return KEY_SPACE;

        case VK_OEM_MINUS:  return KEY_MINUS;
        case VK_OEM_PLUS:   return KEY_EQUAL;
        case VK_OEM_4:      return KEY_LEFT_BRACKET;
        case VK_OEM_6:      return KEY_RIGHT_BRACKET;
        case VK_OEM_5:      return KEY_BACKSLASH;
        case VK_OEM_1:      return KEY_SEMICOLON;
        case VK_OEM_7:      return KEY_APOSTROPHE;
        case VK_OEM_3:      return KEY_GRAVE;
        case VK_OEM_COMMA:  return KEY_COMMA;
        case VK_OEM_PERIOD: return KEY_PERIOD;
        case VK_OEM_2:      return KEY_SLASH;

        case VK_F1:         return KEY_F1;
        case VK_F2:         return KEY_F2;
        case VK_F3:         return KEY_F3;
        case VK_F4:         return KEY_F4;
        case VK_F5:         return KEY_F5;
        case VK_F6:         return KEY_F6;
        case VK_F7:         return KEY_F7;
        case VK_F8:         return KEY_F8;
        case VK_F9:         return KEY_F9;
        case VK_F10:        return KEY_F10;
        case VK_F11:        return KEY_F11;
        case VK_F12:        return KEY_F12;

        case VK_SCROLL:     return KEY_SCROLL_LOCK;
        case VK_PAUSE:      return KEY_PAUSE;
        case VK_INSERT:     return KEY_INSERT;
        case VK_HOME:       return KEY_HOME;
        case VK_PRIOR:      return KEY_PAGE_UP;
        case VK_DELETE:     return KEY_DELETE;
        case VK_END:        return KEY_END;
        case VK_NEXT:       return KEY_PAGE_DOWN;

        case VK_RIGHT:      return KEY_RIGHT;
        case VK_LEFT:       return KEY_LEFT;
        case VK_DOWN:       return KEY_DOWN;
        case VK_UP:         return KEY_UP;

        case VK_NUMLOCK:    return KEY_KP_NUM_LOCK;
        case VK_DIVIDE:     return KEY_KP_SLASH;
        case VK_MULTIPLY:   return KEY_KP_ASTERISK;
        case VK_SUBTRACT:   return KEY_KP_MINUS;
        case VK_ADD:        return KEY_KP_PLUS;

        case VK_NUMPAD1:    return KEY_KP_1;
        case VK_NUMPAD2:    return KEY_KP_2;
        case VK_NUMPAD3:    return KEY_KP_3;
        case VK_NUMPAD4:    return KEY_KP_4;
        case VK_NUMPAD5:    return KEY_KP_5;
        case VK_NUMPAD6:    return KEY_KP_6;
        case VK_NUMPAD7:    return KEY_KP_7;
        case VK_NUMPAD8:    return KEY_KP_8;
        case VK_NUMPAD9:    return KEY_KP_9;
        case VK_NUMPAD0:    return KEY_KP_0;
        case VK_DECIMAL:    return KEY_KP_PERIOD;

        case VK_LSHIFT:     return KEY_LEFT_SHIFT;
        case VK_RSHIFT:     return KEY_RIGHT_SHIFT;
        case VK_LWIN:       return KEY_LEFT_SUPER;
        case VK_RWIN:       return KEY_RIGHT_SUPER;
        case VK_MENU:       return extended ? KEY_RIGHT_ALT : KEY_LEFT_ALT;
        case VK_CONTROL:    return extended ? KEY_RIGHT_CTRL
            : GetAsyncKeyState(VK_RMENU) & 0x8000 ?
                KEY_UNKNOWN : KEY_LEFT_CTRL;
        case VK_SHIFT:      return
            MapVirtualKey((lParam >> 16) & 0xFF, MAPVK_VSC_TO_VK_EX)  == VK_LSHIFT ?
                KEY_LEFT_SHIFT : KEY_RIGHT_SHIFT;

        default:            return KEY_UNKNOWN;
    }
}

bool IsKeyPressed(KeyboardKey key) {
    return keys.pressed[key];
}

bool IsKeyDown(KeyboardKey key) {
    return keys.down[key];
}

bool IsKeyReleased(KeyboardKey key) {
    return keys.released[key];
}

int GetMouseX() {
    return mouse.x;
}

int GetMouseY() {
    return mouse.y;
}

void GetMousePosition(int* x, int* y) {
    if (x) *x = mouse.x;
    if (y) *y = mouse.y;
}

int GetMouseWheelMove() {
    return mouse.wheelMove;
}

bool IsMouseButtonPressed(MouseButton button) {
    return mouse.buttons.pressed[button];
}

bool IsMouseButtonDown(MouseButton button) {
    return mouse.buttons.down[button];
}

bool IsMouseButtonReleased(MouseButton button) {
    return mouse.buttons.released[button];
}

void SetMousePosition(int x, int y) {
    POINT pt = { x, y };
    ClientToScreen(platform.hwnd, &pt);
    SetCursorPos(pt.x, pt.y);

    mouse.x = pt.x;
    mouse.y = pt.y;
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
    memset(keys.pressed, 0, sizeof(keys.pressed));
    memset(keys.released, 0, sizeof(keys.released));

    memset(mouse.buttons.pressed, 0, sizeof(mouse.buttons.pressed));
    memset(mouse.buttons.released, 0, sizeof(mouse.buttons.released));

    mouse.wheelMove = 0;

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
