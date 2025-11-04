#pragma once
#include <stdint.h>
#include <stdbool.h>

typedef struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} Color;

#define RED     (Color){ 0xFF, 0x00, 0x00, 0xFF }
#define GREEN   (Color){ 0x00, 0xFF, 0x00, 0xFF }
#define BLUE    (Color){ 0x00, 0x00, 0xFF, 0xFF }
#define CYAN    (Color){ 0x00, 0xFF, 0xFF, 0xFF }
#define MAGENTA (Color){ 0xFF, 0x00, 0xFF, 0xFF }
#define YELLOW  (Color){ 0xFF, 0xFF, 0x00, 0xFF }
#define BLACK   (Color){ 0x00, 0x00, 0x00, 0xFF }
#define WHITE   (Color){ 0xFF, 0xFF, 0xFF, 0xFF }

typedef struct PixelFormat {
    uint32_t rMask;
    uint32_t gMask;
    uint32_t bMask;
    uint32_t aMask;
    
    uint8_t rShift;
    uint8_t gShift;
    uint8_t bShift;
    uint8_t aShift;

    uint8_t rLoss;
    uint8_t gLoss;
    uint8_t bLoss;
    uint8_t aLoss;
} PixelFormat;

inline static const PixelFormat FORMAT_RGBA8888 = {
    .rMask = 0xFF000000,
    .gMask = 0x00FF0000,
    .bMask = 0x0000FF00,
    .aMask = 0x000000FF,

    .rShift = 24,
    .gShift = 16,
    .bShift = 8,
    .aShift = 0,

    .rLoss = 0,
    .gLoss = 0,
    .bLoss = 0,
    .aLoss = 0,
};

inline static const PixelFormat FORMAT_ABGR8888 = {
    .rMask = 0x000000FF,
    .gMask = 0x0000FF00,
    .bMask = 0x00FF0000,
    .aMask = 0xFF000000,

    .rShift = 0,
    .gShift = 8,
    .bShift = 16,
    .aShift = 24,

    .rLoss = 0,
    .gLoss = 0,
    .bLoss = 0,
    .aLoss = 0,
};

inline static const PixelFormat FORMAT_ARGB8888 = {
    .rMask = 0x00FF0000,
    .gMask = 0x0000FF00,
    .bMask = 0x000000FF,
    .aMask = 0xFF000000,

    .rShift = 16,
    .gShift = 8,
    .bShift = 0,
    .aShift = 24,

    .rLoss = 0,
    .gLoss = 0,
    .bLoss = 0,
    .aLoss = 0,
};

inline static const PixelFormat FORMAT_BGRA8888 = {
    .rMask = 0x0000FF00,
    .gMask = 0x00FF0000,
    .bMask = 0xFF000000,
    .aMask = 0x000000FF,

    .rShift = 8,
    .gShift = 16,
    .bShift = 24,
    .aShift = 0,

    .rLoss = 0,
    .gLoss = 0,
    .bLoss = 0,
    .aLoss = 0,
};

inline static const PixelFormat FORMAT_RGB565 = {
    .rMask = 0xF800,
    .gMask = 0x07E0,
    .bMask = 0x001F,
    .aMask = 0x0000,

    .rShift = 11,
    .gShift = 5,
    .bShift = 0,
    .aShift = 0,

    .rLoss = 3,
    .gLoss = 2,
    .bLoss = 3,
    .aLoss = 8,
};

inline static const PixelFormat FORMAT_BGR565 = {
    .rMask = 0x001F,
    .gMask = 0x07E0,
    .bMask = 0xF800,
    .aMask = 0x0000,

    .rShift = 0,
    .gShift = 5,
    .bShift = 11,
    .aShift = 0,

    .rLoss = 3,
    .gLoss = 2,
    .bLoss = 3,
    .aLoss = 8,
};
