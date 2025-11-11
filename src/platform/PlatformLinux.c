#include <X11/keysym.h>
#include <X11/X.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <unistd.h> 
#include <X11/Xlib.h>
#include "Input.h"
#include "Window.h"

static LGL_KeyCode MapKeyCode(KeySym sym) {
    switch (sym) {
        case XK_Escape:     return KEY_ESCAPE;
        case XK_Return:     return KEY_ENTER;
        case XK_Tab:        return KEY_TAB;
        case XK_space:      return KEY_SPACE;
        case XK_BackSpace:  return KEY_BACKSPACE;

        case XK_Left:       return KEY_LEFT;
        case XK_Right:      return KEY_RIGHT;
        case XK_Up:         return KEY_UP;
        case XK_Down:       return KEY_DOWN;

        case XK_Shift_L:    return KEY_LEFT_SHIFT;
        case XK_Shift_R:    return KEY_RIGHT_SHIFT;
        case XK_Control_L:  return KEY_LEFT_CTRL;
        case XK_Control_R:  return KEY_RIGHT_CTRL;
        case XK_Alt_L:      return KEY_LEFT_ALT;
        case XK_Alt_R:      return KEY_RIGHT_ALT;

        case XK_F1:         return KEY_F1;
        case XK_F2:         return KEY_F2;
        case XK_F3:         return KEY_F3;
        case XK_F4:         return KEY_F4;
        case XK_F5:         return KEY_F5;
        case XK_F6:         return KEY_F6;
        case XK_F7:         return KEY_F7;
        case XK_F8:         return KEY_F8;
        case XK_F9:         return KEY_F9;
        case XK_F10:        return KEY_F10;
        case XK_F11:        return KEY_F11;
        case XK_F12:        return KEY_F12;
    }

    if (sym >= XK_a && sym <= XK_z) {
        return (LGL_KeyCode)(KEY_A + (sym - XK_a));
    }

    if (sym >= XK_0 && sym <= XK_9) {
        return (LGL_KeyCode)(KEY_0 + (sym - XK_0));
    }

    return KEY_UNKNOWN;
}

#define MAX_KEYS 256

typedef struct KeyboardState {
    bool down[MAX_KEYS];
    bool pressed[MAX_KEYS];
    bool released[MAX_KEYS];
} KeyboardState;

static KeyboardState keys = { 0 };

bool IsKeyPressed(LGL_KeyCode key) {
    return keys.pressed[key];
}

bool IsKeyDown(LGL_KeyCode key) {
    return keys.down[key];
}

bool IsKeyReleased(LGL_KeyCode key) {
    return keys.released[key];
}

// --------------------------------------------------------------------------------------------------------------------

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

static Platform platform = { 0 };

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

    const long mask = KeyPressMask | KeyReleaseMask | ExposureMask;
    XSelectInput(platform.display, platform.window, mask);

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

inline static void HandleKeyboardEvent(XEvent event) {
    // check autorepeat
    if (event.type == KeyRelease) {
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
    }

    KeySym sym = XLookupKeysym(&event.xkey, 0);
    KeyCode key = MapKeyCode(sym);
    if (key == KEY_UNKNOWN) return;

    if (event.type == KeyPress) {
        if (!keys.down[key]) keys.pressed[key] = true;
        keys.down[key] = true;
    } else {
        keys.down[key] = false;
        keys.released[key] = true;
    }
}

void WindowBeginFrame() {
    XEvent event;

    memset(keys.pressed, 0, sizeof(keys.pressed));
    memset(keys.released, 0, sizeof(keys.released));

    while (XPending(platform.display)) {
        XNextEvent(platform.display, &event);
        if (event.type == ClientMessage) {
            if ((Atom)event.xclient.data.l[0] == platform.wmDeleteWindow) {
                platform.shouldClose = 1;
            }
        }
        if (event.type == KeyPress || event.type == KeyRelease) {
            HandleKeyboardEvent(event);
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
