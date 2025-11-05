#include <X11/Xlib.h>
#include "color.h"
#include "window.h"

typedef struct platform_t {
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

    const long mask = ExposureMask;
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

void WindowBeginFrame() {
    XEvent event;

    while (XPending(platform.display)) {
        XNextEvent(platform.display, &event);
        if (event.type == ClientMessage) {
            if ((Atom)event.xclient.data.l[0] == platform.wmDeleteWindow) {
                platform.shouldClose = 1;
            }
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
}
