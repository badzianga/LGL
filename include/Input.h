#pragma once
#include <stdbool.h>

// Based on USB HID Usage Table (https://usb.org/sites/default/files/hut1_4.pdf)
typedef enum KeyboardKey {
    KEY_UNKNOWN       = 0x00,

    KEY_A             = 0x04,
    KEY_B             = 0x05,
    KEY_C             = 0x06,
    KEY_D             = 0x07,
    KEY_E             = 0x08,
    KEY_F             = 0x09,
    KEY_G             = 0x0A,
    KEY_H             = 0x0B,
    KEY_I             = 0x0C,
    KEY_J             = 0x0D,
    KEY_K             = 0x0E,
    KEY_L             = 0x0F,
    KEY_M             = 0x10,
    KEY_N             = 0x11,
    KEY_O             = 0x12,
    KEY_P             = 0x13,
    KEY_Q             = 0x14,
    KEY_R             = 0x15,
    KEY_S             = 0x16,
    KEY_T             = 0x17,
    KEY_U             = 0x18,
    KEY_V             = 0x19,
    KEY_W             = 0x1A,
    KEY_X             = 0x1B,
    KEY_Y             = 0x1C,
    KEY_Z             = 0x1D,

    KEY_1             = 0x1E,
    KEY_2             = 0x1F,
    KEY_3             = 0x20,
    KEY_4             = 0x21,
    KEY_5             = 0x22,
    KEY_6             = 0x23,
    KEY_7             = 0x24,
    KEY_8             = 0x25,
    KEY_9             = 0x26,
    KEY_0             = 0x27,

    KEY_ENTER         = 0x28,
    KEY_ESCAPE        = 0x29,
    KEY_BACKSPACE     = 0x2A,
    KEY_TAB           = 0x2B,
    KEY_SPACE         = 0x2C,

    KEY_MINUS         = 0x2D,
    KEY_EQUAL         = 0x2E,
    KEY_LEFT_BRACKET  = 0x2F,
    KEY_RIGHT_BRACKET = 0x30,
    KEY_BACKSLASH     = 0x31,
    KEY_SEMICOLON     = 0x33,
    KEY_APOSTROPHE    = 0x34,
    KEY_GRAVE         = 0x35,
    KEY_COMMA         = 0x36,
    KEY_PERIOD        = 0x37,
    KEY_SLASH         = 0x38,

    KEY_CAPS_LOCK     = 0x39,
    KEY_F1            = 0x3A,
    KEY_F2            = 0x3B,
    KEY_F3            = 0x3C,
    KEY_F4            = 0x3D,
    KEY_F5            = 0x3E,
    KEY_F6            = 0x3F,
    KEY_F7            = 0x40,
    KEY_F8            = 0x41,
    KEY_F9            = 0x42,
    KEY_F10           = 0x43,
    KEY_F11           = 0x44,
    KEY_F12           = 0x45,

    KEY_PRINT_SCREEN  = 0x46,
    KEY_SCROLL_LOCK   = 0x47,
    KEY_PAUSE         = 0x48,
    KEY_INSERT        = 0x49,
    KEY_HOME          = 0x4A,
    KEY_PAGE_UP       = 0x4B,
    KEY_DELETE        = 0x4C,
    KEY_END           = 0x4D,
    KEY_PAGE_DOWN     = 0x4E,

    KEY_RIGHT         = 0x4F,
    KEY_LEFT          = 0x50,
    KEY_DOWN          = 0x51,
    KEY_UP            = 0x52,

    KEY_KP_NUM_LOCK   = 0x53,
    KEY_KP_SLASH      = 0x54,
    KEY_KP_ASTERISK   = 0x55,
    KEY_KP_MINUS      = 0x56,
    KEY_KP_PLUS       = 0x57,
    KEY_KP_ENTER      = 0x58,
    KEY_KP_1          = 0x59,
    KEY_KP_2          = 0x5A,
    KEY_KP_3          = 0x5B,
    KEY_KP_4          = 0x5C,
    KEY_KP_5          = 0x5D,
    KEY_KP_6          = 0x5E,
    KEY_KP_7          = 0x5F,
    KEY_KP_8          = 0x60,
    KEY_KP_9          = 0x61,
    KEY_KP_0          = 0x62,
    KEY_KP_PERIOD     = 0x63,

    KEY_LEFT_CTRL     = 0xE0,
    KEY_LEFT_SHIFT    = 0xE1,
    KEY_LEFT_ALT      = 0xE2,
    KEY_LEFT_SUPER    = 0xE3,
    KEY_RIGHT_CTRL    = 0xE4,
    KEY_RIGHT_SHIFT   = 0xE5,
    KEY_RIGHT_ALT     = 0xE6,
    KEY_RIGHT_SUPER   = 0xE7,
} KeyboardKey;

typedef enum MouseButton {
    MOUSE_BUTTON_LEFT   = 0,
    MOUSE_BUTTON_MIDDLE = 1,
    MOUSE_BUTTON_RIGHT  = 2,
} MouseButton;

bool IsKeyPressed(KeyboardKey key);
bool IsKeyDown(KeyboardKey key);
bool IsKeyReleased(KeyboardKey key);

int GetMouseX();
int GetMouseY();
void GetMousePosition(int* x, int* y);
int GetMouseWheelMove();

bool IsMouseButtonPressed(MouseButton button);
bool IsMouseButtonDown(MouseButton button);
bool IsMouseButtonReleased(MouseButton button);

void SetMousePosition(int x, int y);
void CursorShow();
void CursorHide();
bool IsCursorHidden();
