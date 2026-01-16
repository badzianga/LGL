# LGL

LGL (Lite Graphics Library) is a graphics library written in C based on software rendering, designed for PC and ESP32 microcontrollers.

## Build instructions

The project can be built using CMake and is dependent on `Freetype` and `harfbuzz` libraries (make sure to have them on your system).

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

## Example - PC

This code below draws a "Hello, world!" text and displays it on the screen.

```C++
#include "BitmapFont.h"
#include "Input.h"
#include "Surface.h"
#include "Window.h"

int main() {
    Surface display = WindowInit(800, 600, "Hello, LGL!");
    
    while (!WindowShouldClose()) {
        WindowBeginFrame();
        SurfaceFill(display, BLACK);
        
        DrawTextBitmapFont(display, 0, 0, "Hello, world!", NULL, WHITE);
        
        if (IsKeyPressed(KEY_ESCAPE)) {
            WindowSetClose(true);
        }

        WindowEndFrame();
    }
    
    WindowDestroy();
    
    return 0;
}
```

### Run tests

LGL uses [Ceedling](https://github.com/ThrowTheSwitch/Ceedling/tree/master) for building and running tests, so necessary dependencies are `ruby3` and `ceedling`.

To run tests, execute:

```bash
ceedling test:all
```

Make sure you have `ruby` in your `PATH`.
