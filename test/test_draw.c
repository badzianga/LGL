#include "surface.h"
#include "unity.h"
#include "draw.h"

static Surface surface;

static const int SURF_WIDTH = 4;
static const int SURF_HEIGHT = 2;

static uint32_t ColorToHex(Color color) {
    return *(uint32_t*)&color;
}

void setUp(void) {
    surface = SurfaceCreate(SURF_WIDTH, SURF_HEIGHT);
    SurfaceFill(surface, BLUE);
}

void tearDown(void) {
    SurfaceDestroy(surface);
}

void test_ShouldNotDrawRectWhenCoordsAreAwayFromSurface(void) {
    DrawRect(surface, -10, 0, SURF_WIDTH, SURF_HEIGHT, YELLOW);
    DrawRect(surface, 10, 0, SURF_WIDTH, SURF_HEIGHT, YELLOW);
    DrawRect(surface, 0, -10, SURF_WIDTH, SURF_HEIGHT, YELLOW);
    DrawRect(surface, 0, 10, SURF_WIDTH, SURF_HEIGHT, YELLOW);

    for (int i = 0; i < surface.width * surface.height; ++i) {
        TEST_ASSERT_EQUAL(ColorToHex(surface.pixels[i]), ColorToHex(BLUE));
    }
}

void test_ShouldDrawFullRectOnSurface(void) {
    DrawRect(surface, 0, 0, SURF_WIDTH, SURF_HEIGHT, YELLOW);

    for (int i = 0; i < surface.width * surface.height; ++i) {
        TEST_ASSERT_EQUAL(ColorToHex(surface.pixels[i]), ColorToHex(YELLOW));
    }
}

void test_ShouldDrawPartialRectOnSurface(void) {
    DrawRect(surface, 0, SURF_HEIGHT / 2, SURF_WIDTH, SURF_HEIGHT / 2, YELLOW);

    for (int i = 0; i < surface.width * (surface.height / 2); ++i) {
        TEST_ASSERT_EQUAL(ColorToHex(surface.pixels[i]), ColorToHex(BLUE));
    }
    for (int i = surface.width * (surface.height / 2); i < surface.width * surface.height; ++i) {
        TEST_ASSERT_EQUAL(ColorToHex(surface.pixels[i]), ColorToHex(YELLOW));
    }
}
