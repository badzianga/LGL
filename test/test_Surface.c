#include "PixelFormat.h"
#include "Surface.h"
#include "unity.h"
#include <stdlib.h>

void setUp() {}
void tearDown() {}

#define ASSERT_SURFACE_IS_INVALID(surface)      \
    do {                                        \
        TEST_ASSERT_NULL((surface).pixels);     \
        TEST_ASSERT_EQUAL(0, (surface).width);  \
        TEST_ASSERT_EQUAL(0, (surface).height); \
        TEST_ASSERT_NULL((surface).format);     \
    } while (0)

void test_SurfaceCreateShouldReturnInvalidSurfaceWhenDimensionsAreNotPositive() {
    Surface surface1 = SurfaceCreate(-800, 600, &FORMAT_RGB332);
    ASSERT_SURFACE_IS_INVALID(surface1);
    
    Surface surface2 = SurfaceCreate(800, 0, &FORMAT_RGB332);
    ASSERT_SURFACE_IS_INVALID(surface2);
}

void test_SurfaceCreateShouldReturnInvalidSurfaceWhenPixelFormatIsNotSpecified() {
    Surface surface = SurfaceCreate(800, 600, NULL);
    ASSERT_SURFACE_IS_INVALID(surface);
}

void test_CreatedSurfaceShouldHaveProperValues() {
    Surface surface = SurfaceCreate(40, 30, &FORMAT_RGB332);

    TEST_ASSERT_NOT_NULL(surface.pixels);
    TEST_ASSERT_EQUAL(40, surface.width);
    TEST_ASSERT_EQUAL(30, surface.height);
    TEST_ASSERT_EQUAL_PTR(&FORMAT_RGB332, surface.format);

    free(surface.pixels);
}

void test_SurfaceShouldBeDestroyedProperly() {
    Surface surface = SurfaceCreate(3, 2, &FORMAT_RGB332);
    SurfaceDestroy(&surface);
    ASSERT_SURFACE_IS_INVALID(surface);
}

void test_SurfaceCopyShouldReturnExactCopyOfTheOriginal() {
    Surface original = SurfaceCreate(3, 2, &FORMAT_RGB332);
    *(uint8_t*)original.pixels = 0xAC;

    Surface copy = SurfaceCopy(original);

    TEST_ASSERT_NOT_EQUAL(original.pixels, copy.pixels);
    TEST_ASSERT_EQUAL(original.width, copy.width);
    TEST_ASSERT_EQUAL(original.height, copy.height);
    TEST_ASSERT_EQUAL_PTR(original.format, copy.format);

    TEST_ASSERT_EQUAL_UINT8(*(uint8_t*)original.pixels, *(uint8_t*)copy.pixels);

    SurfaceDestroy(&original);
    SurfaceDestroy(&copy);
}
