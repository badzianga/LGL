#include "unity.h"
#include "FillRect.h"
#include "PixelFormat.h"
#include "Surface.h"

void setUp(void) {}
void tearDown(void) {}

void test_ShouldFillWhole1ByteSurface() {
    Surface surface = SurfaceCreate(3, 2, &FORMAT_RGB332);

    FillRect(surface, 0, 0, surface.width, surface.height, CYAN);
    const uint8_t expectedPixel = ColorToPixel(&FORMAT_RGB332, CYAN);
    for (int i = 0; i < 6; ++i) {
        TEST_ASSERT_EQUAL_HEX8(expectedPixel, ((uint8_t*)surface.pixels)[i]);
    }

    SurfaceDestroy(&surface);
}

void test_ShouldFillWhole2ByteSurface() {
    Surface surface = SurfaceCreate(3, 2, &FORMAT_RGB565);

    FillRect(surface, 0, 0, surface.width, surface.height, CYAN);
    const uint16_t expectedPixel = ColorToPixel(&FORMAT_RGB565, CYAN);
    for (int i = 0; i < 6; ++i) {
        TEST_ASSERT_EQUAL_HEX16(expectedPixel, ((uint16_t*)surface.pixels)[i]);
    }

    SurfaceDestroy(&surface);
}

void test_ShouldFillWhole4ByteSurface() {
    Surface surface = SurfaceCreate(3, 2, &FORMAT_ARGB8888);

    FillRect(surface, 0, 0, surface.width, surface.height, CYAN);
    const uint32_t expectedPixel = ColorToPixel(&FORMAT_ARGB8888, CYAN);
    for (int i = 0; i < 6; ++i) {
        TEST_ASSERT_EQUAL_HEX32(expectedPixel, ((uint32_t*)surface.pixels)[i]);
    }

    SurfaceDestroy(&surface);
}
