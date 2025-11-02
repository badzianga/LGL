#include <stdint.h>
#include "unity.h"
#include "surface.h"

static const int SURFACE_WIDTH = 4;
static const int SURFACE_HEIGHT = 2;

static const uint32_t ColorToHex(Color color) {
    return *(uint32_t*)&color;
}

void test_ShouldGetProperSizeOfSurfaceStruct(void) {
    TEST_ASSERT_EQUAL(16, sizeof(Surface));
}

void test_ShouldCreateSurfaceWhenProperInputs(void) {
    Surface surface = SurfaceCreate(SURFACE_WIDTH, SURFACE_HEIGHT);

    TEST_ASSERT_EQUAL(SURFACE_WIDTH, surface.width);
    TEST_ASSERT_EQUAL(SURFACE_HEIGHT, surface.height);
    TEST_ASSERT_NOT_NULL(surface.pixels);

    SurfaceDestroy(surface);
}

void test_ShouldCreateEmptySurfaceWhenNegativeDimension(void) {
    Surface surface = SurfaceCreate(-SURFACE_WIDTH, SURFACE_HEIGHT);

    TEST_ASSERT_EQUAL(0, surface.width);
    TEST_ASSERT_EQUAL(0, surface.height);
    TEST_ASSERT_NULL(surface.pixels);

    SurfaceDestroy(surface);
}

void test_ShouldCreateEmptySurfaceWhenDimensionIsZero(void) {
    Surface surface = SurfaceCreate(SURFACE_WIDTH, 0);

    TEST_ASSERT_EQUAL(0, surface.width);
    TEST_ASSERT_EQUAL(0, surface.height);
    TEST_ASSERT_EQUAL(NULL, surface.pixels);

    SurfaceDestroy(surface);
}

void test_ShouldFillSurfaceWithColor(void) {
    Surface surface = SurfaceCreate(SURFACE_WIDTH, SURFACE_HEIGHT);

    SurfaceFill(surface, YELLOW);
    for (int i = 0; i < SURFACE_WIDTH * SURFACE_HEIGHT; ++i) {
        TEST_ASSERT_EQUAL(ColorToHex(YELLOW), ColorToHex(surface.pixels[i]));
    }

    SurfaceDestroy(surface);
}

void test_ShouldCreateCopyOfSurface(void) {
    Surface original = SurfaceCreate(SURFACE_WIDTH, SURFACE_HEIGHT);
    SurfaceFill(original, BLUE);
    Surface copy = SurfaceCopy(original);
    TEST_ASSERT_EQUAL(original.width, copy.width);
    TEST_ASSERT_EQUAL(original.height, copy.height);

    TEST_ASSERT_EQUAL_MEMORY_ARRAY(
        original.pixels,
        copy.pixels,
        sizeof(*original.pixels),
        original.width * original.height
    );

    SurfaceDestroy(original);
    SurfaceDestroy(copy);
}

void test_ShouldBlitNothingWhenCoordsAreAwayFromDestination(void) {
    Surface destination = SurfaceCreate(SURFACE_WIDTH, SURFACE_HEIGHT);
    Surface source = SurfaceCreate(SURFACE_WIDTH, SURFACE_HEIGHT);

    SurfaceFill(destination, BLUE);
    SurfaceFill(source, YELLOW);

    SurfaceBlit(destination, source, -10, 0);
    SurfaceBlit(destination, source, 10, 0);
    SurfaceBlit(destination, source, 0, -10);
    SurfaceBlit(destination, source, 0, 10);

    for (int i = 0; i < destination.width * destination.height; ++i) {
        TEST_ASSERT_EQUAL(ColorToHex(destination.pixels[i]), ColorToHex(BLUE));
    }

    SurfaceDestroy(destination);
    SurfaceDestroy(source);
}

void test_ShouldBlitFullSourceOnDestination(void) {
    Surface destination = SurfaceCreate(SURFACE_WIDTH, SURFACE_HEIGHT);
    Surface source = SurfaceCreate(SURFACE_WIDTH, SURFACE_HEIGHT);

    SurfaceFill(destination, BLUE);
    SurfaceFill(source, YELLOW);

    SurfaceBlit(destination, source, 0, 0);

    for (int i = 0; i < destination.width * destination.height; ++i) {
        TEST_ASSERT_EQUAL(ColorToHex(destination.pixels[i]), ColorToHex(YELLOW));
    }

    SurfaceDestroy(destination);
    SurfaceDestroy(source);
}

void test_ShouldBlitPartialSourceOnDestination(void) {
    Surface destination = SurfaceCreate(SURFACE_WIDTH, SURFACE_HEIGHT);
    Surface source = SurfaceCreate(SURFACE_WIDTH, SURFACE_HEIGHT);

    SurfaceFill(destination, BLUE);
    SurfaceFill(source, YELLOW);

    SurfaceBlit(destination, source, 0, SURFACE_HEIGHT / 2);

    for (int i = 0; i < destination.width * (destination.height / 2); ++i) {
        TEST_ASSERT_EQUAL(ColorToHex(destination.pixels[i]), ColorToHex(BLUE));
    }
    for (int i = destination.width * (destination.height / 2); i < destination.width * destination.height; ++i) {
        TEST_ASSERT_EQUAL(ColorToHex(destination.pixels[i]), ColorToHex(YELLOW));
    }

    SurfaceDestroy(destination);
    SurfaceDestroy(source);
}
