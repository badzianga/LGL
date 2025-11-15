#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <X11/keysym.h>
#include <X11/Xlib.h>

#include "Input.h"
#include "Window.h"

#define MAX_KEYS 256
#define MAX_MOUSE_BUTTONS 8

typedef struct Platform {
    Display* display;
    Window window;
    int screen;
    int shouldClose;
    Surface surface;
    Atom wmDeleteWindow;
    XImage* image;
    GC gc;
} Platform;

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

    Cursor invisibleCursor;
    bool cursorHidden;

    char wheelMove;    
} MouseState;

static Platform platform = { 0 };
static KeyboardState keys = { 0 };
static MouseState mouse = { 0 };

// --------------------------------------------------------------------------------------------------------------------

static KeyboardKey MapKeyCode(KeySym sym) {
    switch (sym) {
        case XK_A: case XK_a: return KEY_A;
        case XK_B: case XK_b: return KEY_B;
        case XK_C: case XK_c: return KEY_C;
        case XK_D: case XK_d: return KEY_D;
        case XK_E: case XK_e: return KEY_E;
        case XK_F: case XK_f: return KEY_F;
        case XK_G: case XK_g: return KEY_G;
        case XK_H: case XK_h: return KEY_H;
        case XK_I: case XK_i: return KEY_I;
        case XK_J: case XK_j: return KEY_J;
        case XK_K: case XK_k: return KEY_K;
        case XK_L: case XK_l: return KEY_L;
        case XK_M: case XK_m: return KEY_M;
        case XK_N: case XK_n: return KEY_N;
        case XK_O: case XK_o: return KEY_O;
        case XK_P: case XK_p: return KEY_P;
        case XK_Q: case XK_q: return KEY_Q;
        case XK_R: case XK_r: return KEY_R;
        case XK_S: case XK_s: return KEY_S;
        case XK_T: case XK_t: return KEY_T;
        case XK_U: case XK_u: return KEY_U;
        case XK_V: case XK_v: return KEY_V;
        case XK_W: case XK_w: return KEY_W;
        case XK_X: case XK_x: return KEY_X;
        case XK_Y: case XK_y: return KEY_Y;
        case XK_Z: case XK_z: return KEY_Z;

        case XK_1:            return KEY_1;
        case XK_2:            return KEY_2;
        case XK_3:            return KEY_3;
        case XK_4:            return KEY_4;
        case XK_5:            return KEY_5;
        case XK_6:            return KEY_6;
        case XK_7:            return KEY_7;
        case XK_8:            return KEY_8;
        case XK_9:            return KEY_9;
        case XK_0:            return KEY_0;

        case XK_Return:       return KEY_ENTER;
        case XK_Escape:       return KEY_ESCAPE;
        case XK_BackSpace:    return KEY_BACKSPACE;
        case XK_Tab:          return KEY_TAB;
        case XK_space:        return KEY_SPACE;

        case XK_minus:        return KEY_MINUS;
        case XK_equal:        return KEY_EQUAL;
        case XK_bracketleft:  return KEY_LEFT_BRACKET;
        case XK_bracketright: return KEY_RIGHT_BRACKET;
        case XK_backslash:    return KEY_BACKSLASH;
        case XK_semicolon:    return KEY_SEMICOLON;
        case XK_apostrophe:   return KEY_APOSTROPHE;
        case XK_grave:        return KEY_GRAVE;
        case XK_comma:        return KEY_COMMA;
        case XK_period:       return KEY_PERIOD;
        case XK_slash:        return KEY_SLASH;

        case XK_Caps_Lock:    return KEY_CAPS_LOCK;
        case XK_F1:           return KEY_F1;
        case XK_F2:           return KEY_F2;
        case XK_F3:           return KEY_F3;
        case XK_F4:           return KEY_F4;
        case XK_F5:           return KEY_F5;
        case XK_F6:           return KEY_F6;
        case XK_F7:           return KEY_F7;
        case XK_F8:           return KEY_F8;
        case XK_F9:           return KEY_F9;
        case XK_F10:          return KEY_F10;
        case XK_F11:          return KEY_F11;
        case XK_F12:          return KEY_F12;

        case XK_Scroll_Lock:  return KEY_SCROLL_LOCK;
        case XK_Pause:        return KEY_PAUSE;
        case XK_Insert:       return KEY_INSERT;
        case XK_Home:         return KEY_HOME;
        case XK_Page_Up:      return KEY_PAGE_UP;
        case XK_Delete:       return KEY_DELETE;
        case XK_End:          return KEY_END;
        case XK_Page_Down:    return KEY_PAGE_DOWN;

        case XK_Right:        return KEY_RIGHT;
        case XK_Left:         return KEY_LEFT;
        case XK_Down:         return KEY_DOWN;
        case XK_Up:           return KEY_UP;

        case XK_Num_Lock:     return KEY_KP_NUM_LOCK;
        case XK_KP_Divide:    return KEY_KP_SLASH;
        case XK_KP_Multiply:  return KEY_KP_ASTERISK;
        case XK_KP_Subtract:  return KEY_KP_MINUS;
        case XK_KP_Add:       return KEY_KP_PLUS;
        case XK_KP_Enter:     return KEY_KP_ENTER;
        case XK_KP_1:         return KEY_KP_1;
        case XK_KP_2:         return KEY_KP_2;
        case XK_KP_3:         return KEY_KP_3;
        case XK_KP_4:         return KEY_KP_4;
        case XK_KP_5:         return KEY_KP_5;
        case XK_KP_6:         return KEY_KP_6;
        case XK_KP_7:         return KEY_KP_7;
        case XK_KP_8:         return KEY_KP_8;
        case XK_KP_9:         return KEY_KP_9;
        case XK_KP_0:         return KEY_KP_0;
        case XK_KP_Decimal:   return KEY_KP_PERIOD;

        case XK_Control_L:    return KEY_LEFT_CTRL;
        case XK_Shift_L:      return KEY_LEFT_SHIFT;
        case XK_Alt_L:        return KEY_LEFT_ALT;
        case XK_Super_L:      return KEY_LEFT_SUPER;
        case XK_Control_R:    return KEY_RIGHT_CTRL;
        case XK_Shift_R:      return KEY_RIGHT_SHIFT;
        case XK_Alt_R:        return KEY_RIGHT_ALT;
        case XK_Super_R:      return KEY_RIGHT_SUPER;

        default:              return KEY_UNKNOWN;
    }
}

static MouseButton MapMouseButton(uint32_t xButton) {
    switch (xButton) {
        case Button1: return MOUSE_BUTTON_LEFT;
        case Button2: return MOUSE_BUTTON_MIDDLE;
        case Button3: return MOUSE_BUTTON_RIGHT;
        default: return (MouseButton)(xButton - 1);
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
    *x = mouse.x;
    *y = mouse.y;
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
    x = x < 0 ? 0 : (x >= platform.surface.width ? platform.surface.width - 1 : x);
    y = y < 0 ? 0 : (y >= platform.surface.height ? platform.surface.height - 1 : y);

    XWarpPointer(platform.display, None, platform.window, 0, 0, 0, 0, x, y);
    XFlush(platform.display);
    mouse.x = x;
    mouse.y = y;
}

void CursorShow() {
    if (!mouse.cursorHidden) return;
    XUndefineCursor(platform.display, platform.window);
    XFreeCursor(platform.display, mouse.invisibleCursor);
    XFlush(platform.display);
    mouse.cursorHidden = false;
}

void CursorHide() {
    if (mouse.cursorHidden) return;

    Pixmap pixmap;
    XColor black = { 0 };
    static char noData[8] = { 0 };

    pixmap = XCreateBitmapFromData(platform.display, platform.window, noData, 8, 8);
    mouse.invisibleCursor = XCreatePixmapCursor(platform.display, pixmap, pixmap, &black, &black, 0, 0);
    XDefineCursor(platform.display, platform.window, mouse.invisibleCursor);
    XFreePixmap(platform.display, pixmap);
    XFlush(platform.display);

    mouse.cursorHidden = true;
}

bool IsCursorHidden() {
    return mouse.cursorHidden;
}

// --------------------------------------------------------------------------------------------------------------------

static void InitTimer(void);
static void FrameTick(void);

Surface WindowInit(int width, int height, const char* title) {
    platform.display = XOpenDisplay(NULL);
    if (platform.display == NULL) {
        return (Surface){ 0 };
    }
    platform.screen = XDefaultScreen(platform.display);

    platform.window = XCreateSimpleWindow(
        platform.display,
        XRootWindow(platform.display, platform.screen),
        0, 0,
        width, height,
        1,
        BlackPixel(platform.display, platform.screen),
        BlackPixel(platform.display, platform.screen)
    );

    const long eventMask =
        KeyPressMask | KeyReleaseMask |
        ButtonPressMask | ButtonReleaseMask | PointerMotionMask |
        ExposureMask;
    XSelectInput(platform.display, platform.window, eventMask);

    platform.surface = SurfaceCreate(width, height, &FORMAT_ARGB8888);
    WindowSetTitle(title);

    platform.wmDeleteWindow = XInternAtom(platform.display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(platform.display, platform.window, &platform.wmDeleteWindow, 1);

    platform.image = XCreateImage(
        platform.display,
        XDefaultVisual(platform.display, platform.screen),
        XDefaultDepth(platform.display, platform.screen),
        ZPixmap,
        0,
        platform.surface.pixels,
        width,
        height,
        platform.surface.format->bytesPerPixel * 8,
        0
    );
    platform.gc = XCreateGC(platform.display, platform.window, 0, NULL);

    XMapWindow(platform.display, platform.window);

    InitTimer();

    return platform.surface;
}

void WindowDestroy() {
    XUnmapWindow(platform.display, platform.window);
    XCloseDisplay(platform.display);
}

void WindowSetClose(bool close) {
    platform.shouldClose = close;
}

bool WindowShouldClose() {
    return platform.shouldClose;
}

void WindowSetTitle(const char* title) {
    XStoreName(platform.display, platform.window, title);
}

inline static void HandleClientMessageEvent(XEvent event) {
    if ((Atom)event.xclient.data.l[0] == platform.wmDeleteWindow) {
        platform.shouldClose = 1;
    }
}

inline static void HandleKeyPressEvent(XEvent event) {
    KeySym sym = XLookupKeysym(&event.xkey, 0);
    KeyCode key = MapKeyCode(sym);
    if (key == KEY_UNKNOWN) return;

    if (!keys.down[key]) keys.pressed[key] = true;
    keys.down[key] = true;
}

inline static void HandleKeyReleaseEvent(XEvent event) {
    // check autorepeat
    if (XEventsQueued(platform.display, QueuedAfterReading)) {
        XEvent nextEvent;
        XPeekEvent(platform.display, &nextEvent);

        if (nextEvent.type == KeyPress &&
            nextEvent.xkey.time == event.xkey.time &&
            nextEvent.xkey.keycode == event.xkey.keycode) {
            XNextEvent(platform.display, &nextEvent);
            return;
        }
    }

    KeySym sym = XLookupKeysym(&event.xkey, 0);
    KeyCode key = MapKeyCode(sym);
    if (key == KEY_UNKNOWN) return;

    keys.down[key] = false;
    keys.released[key] = true;
}

inline static void HandleButtonPressEvent(XEvent event) {
    switch (event.xbutton.button) {
        case Button4: {
            mouse.wheelMove = 1;
        } break;
        case Button5: {
            mouse.wheelMove = -1;
        } break;
    }
    MouseButton button = MapMouseButton(event.xbutton.button);
    if (!mouse.buttons.down[button]) {
        mouse.buttons.pressed[button] = true;
    }
    mouse.buttons.down[button] = true;
}

inline static void HandleButtonReleaseEvent(XEvent event) {
    MouseButton button = MapMouseButton(event.xbutton.button);
    mouse.buttons.down[button] = false;
    mouse.buttons.released[button] = true;
}

inline static void HandleMouseMotionEvent(XEvent event) {
    const int x = event.xmotion.x;
    const int y = event.xmotion.y;
    
    mouse.x = x < 0 ? 0 : (x >= platform.surface.width ? platform.surface.width - 1 : x);
    mouse.y = y < 0 ? 0 : (y >= platform.surface.height ? platform.surface.height - 1 : y);
}

void WindowBeginFrame() {
    XEvent event;

    memset(keys.pressed, 0, sizeof(keys.pressed));
    memset(keys.released, 0, sizeof(keys.released));

    memset(mouse.buttons.pressed, 0, sizeof(mouse.buttons.pressed));
    memset(mouse.buttons.released, 0, sizeof(mouse.buttons.released));

    mouse.wheelMove = 0;

    while (XPending(platform.display)) {
        XNextEvent(platform.display, &event);
        switch (event.type) {
            case KeyPress: {
                HandleKeyPressEvent(event);
            } break;
            case KeyRelease: {
                HandleKeyReleaseEvent(event);
            } break;
            case ButtonPress: {
                HandleButtonPressEvent(event);
            } break;
            case ButtonRelease: {
                HandleButtonReleaseEvent(event);
            } break;
            case MotionNotify: {
                HandleMouseMotionEvent(event);
            } break;
            case ClientMessage: {
                HandleClientMessageEvent(event);
            } break;
        }
    }
}

void WindowEndFrame() {
    XPutImage(
        platform.display,
        platform.window,
        platform.gc,
        platform.image,
        0, 0,
        0, 0,
        platform.surface.width,
        platform.surface.height
    );

    XSync(platform.display, False);

    FrameTick();
}

// --------------------------------------------------------------------------------------------------------------------

typedef struct TimeHandling {
    double targetFrameTime;
    struct timespec lastFrameTime;
    double deltaTime;
    struct timespec startTime;
} TimeHandling;

static TimeHandling timeHandling = { 0 };

inline static double TimespecToSeconds(struct timespec t) {
    return (double)t.tv_sec + (double)t.tv_nsec / 1e9;
}

static void InitTimer(void) {
    clock_gettime(CLOCK_MONOTONIC, &timeHandling.startTime);
    timeHandling.lastFrameTime = timeHandling.startTime;
}

void WindowSetTargetFPS(int fps) {
    if (fps <= 0) {
        timeHandling.targetFrameTime = 0.0;
    } else {
        timeHandling.targetFrameTime = 1.0 / (double)fps;
    }
}

float WindowGetFrameTime(void) {
    return (float)timeHandling.deltaTime;
}

double WindowGetTime(void) {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return TimespecToSeconds(now) - TimespecToSeconds(timeHandling.startTime);
}

static void FrameTick(void) {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);

    double current = TimespecToSeconds(now);
    double previous = TimespecToSeconds(timeHandling.lastFrameTime);
    timeHandling.deltaTime = current - previous;

    if (timeHandling.targetFrameTime > 0.0 && timeHandling.deltaTime < timeHandling.targetFrameTime) {
        double sleepTime = timeHandling.targetFrameTime - timeHandling.deltaTime;
        if (sleepTime > 0.0) {
            usleep((useconds_t)(sleepTime * 1e6));
        }
        clock_gettime(CLOCK_MONOTONIC, &now);
        current = TimespecToSeconds(now);
        timeHandling.deltaTime = current - previous;
    }

    timeHandling.lastFrameTime = now;
}
