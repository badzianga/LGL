#include <SPI.h>
#include <TFT_eSPI.h>

#include "Window.h"

typedef struct Platform {
    TFT_eSPI tft;
    Surface surface;
    int shouldClose;
} Platform;

typedef struct TimeHandling {
    double targetFrameTime;
    int64_t startTimeUs;
    int64_t lastFrameUs;
    float frameTimeSec;
} TimeHandling;

static Platform platform = { 0 };
static TimeHandling timeHandling = { 0 };

#ifdef USE_RGB332
#include "Allocator.h"

static uint16_t RGB332To565LUT[256];
static uint16_t* tftLineBuffer = NULL;

static void InitRGB332LUT(void) {
    for (int i = 0; i < 256; ++i) {
        RGB332To565LUT[i] = ColorToPixel(&FORMAT_RGB565, PixelToColor(&FORMAT_RGB332, i));;
    }
}
#endif

Surface WindowInit(int width, int height, const char* title) {
    (void)title;
    platform.tft = TFT_eSPI();
    platform.tft.init();
    platform.tft.setRotation(0);
    platform.tft.setSwapBytes(true);  // ESP32 uses little endian, display uses big endian...
    platform.tft.fillScreen(0x0000);

#ifdef USE_RGB332
    InitRGB332LUT();
    tftLineBuffer = (uint16_t*)AllocatorAlloc(width * sizeof(uint16_t));

    platform.surface = SurfaceCreate(width, height, &FORMAT_RGB332);
#else
    platform.surface = SurfaceCreate(width, height, &FORMAT_RGB565);
#endif

    timeHandling.lastFrameUs = timeHandling.startTimeUs = esp_timer_get_time();

    return platform.surface;
}

void WindowDestroy() {}

void WindowSetClose(bool close) {
    platform.shouldClose = close;
}

bool WindowShouldClose() {
    return platform.shouldClose;
}

void WindowSetTitle(const char* title) {
    (void)title;
}

void WindowBeginFrame() {}

static void FrameTick() {
    int64_t now = esp_timer_get_time();
    const double frameTime = (double)(now - timeHandling.lastFrameUs) / 1000000.0;

    if (frameTime < timeHandling.targetFrameTime) {
        const double sleep_us = (timeHandling.targetFrameTime - frameTime) * 1000000.0;

        if (sleep_us > 2000) {
            esp_sleep_enable_timer_wakeup((uint64_t)sleep_us);
            esp_light_sleep_start();
        }
    }

    while ((double)(esp_timer_get_time() - timeHandling.lastFrameUs) / 1000000.0 < timeHandling.targetFrameTime) {
        __asm__ __volatile__("nop");
    }

    now = esp_timer_get_time();
    timeHandling.frameTimeSec = (float)(now - timeHandling.lastFrameUs) / 1000000.f;
    timeHandling.lastFrameUs = now;
}

void WindowEndFrame() {
#ifdef USE_RGB332
    const int width = platform.surface.width;
    const int height = platform.surface.height;
    const int stride = platform.surface.stride;

    uint8_t* pixels = (uint8_t*)platform.surface.pixels;

    for (int y = 0; y < height; ++y) {
        const uint8_t* pixelIn = pixels + y * stride;
        const uint8_t* end = pixels + (y + 1) * stride;
        uint16_t* pixelOut = tftLineBuffer;

        while (pixelIn < end) {
            *pixelOut++ = RGB332To565LUT[*pixelIn++];
        }

        platform.tft.pushImage(0, y, platform.surface.width, 1, tftLineBuffer);
    }
#else
    platform.tft.pushImage(0, 0, platform.surface.width, platform.surface.height, (uint16_t*)platform.surface.pixels);
#endif

    FrameTick();
}

void WindowSetTargetFPS(int fps) {
    if (fps <= 0) {
        timeHandling.targetFrameTime = 0.0;
    }
    else {
        timeHandling.targetFrameTime = 1.0 / (double)fps;
    }
}

float WindowGetFrameTime(void) {
    return timeHandling.frameTimeSec;
}

double WindowGetTime(void) {
    int64_t now = esp_timer_get_time();
    return (double)(now - timeHandling.startTimeUs) / 1000000.0;
}
