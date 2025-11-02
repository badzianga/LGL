#pragma once
#include <stdint.h>

typedef struct BGRA8888 {
    uint8_t b;
    uint8_t g;
    uint8_t r;
    uint8_t a;
} BGRA8888;

#define Color BGRA8888

#define ColorGetR(color) color.r
#define ColorGetG(color) color.g
#define ColorGetB(color) color.b
#define ColorGetA(color) color.a

#define ColorFromComponents(r, g, b, a) (Color){ b, g, r, a }

#define RED     (Color){ 0x00, 0x00, 0xFF, 0xFF }
#define GREEN   (Color){ 0x00, 0xFF, 0x00, 0xFF }
#define BLUE    (Color){ 0xFF, 0x00, 0x00, 0xFF }
#define CYAN    (Color){ 0xFF, 0xFF, 0x00, 0xFF }
#define MAGENTA (Color){ 0xFF, 0x00, 0xFF, 0xFF }
#define YELLOW  (Color){ 0x00, 0xFF, 0xFF, 0xFF }
#define BLACK   (Color){ 0x00, 0x00, 0x00, 0xFF }
#define WHITE   (Color){ 0xFF, 0xFF, 0xFF, 0xFF }
