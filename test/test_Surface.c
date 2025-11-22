#include <stdlib.h>

#include "PixelFormat.h"
#include "Surface.h"
#include "unity.h"

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
    const Surface surface1 = SurfaceCreate(-800, 600, &FORMAT_RGB332);
    ASSERT_SURFACE_IS_INVALID(surface1);
    
    const Surface surface2 = SurfaceCreate(800, 0, &FORMAT_RGB332);
    ASSERT_SURFACE_IS_INVALID(surface2);
}

void test_SurfaceCreateShouldReturnInvalidSurfaceWhenPixelFormatIsNotSpecified() {
    const Surface surface = SurfaceCreate(800, 600, NULL);
    ASSERT_SURFACE_IS_INVALID(surface);
}

void test_CreatedSurfaceShouldHaveProperValues() {
    const Surface surface = SurfaceCreate(40, 30, &FORMAT_RGB332);

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

void test_SurfaceConvertShouldReturnInvalidSurfaceWhenFormatIsNotSpecified() {
    Surface original = SurfaceCreate(4, 3, &FORMAT_RGB332);
    Surface surface = SurfaceConvert(original, NULL);

    ASSERT_SURFACE_IS_INVALID(surface);

    SurfaceDestroy(&original);
}

void test_ConvertedSurfaceShouldBeCopyOfTheOriginalIfFormatsAreTheSame() {
    Surface original = SurfaceCreate(3, 2, &FORMAT_RGBA8888);
    Surface converted = SurfaceConvert(original, original.format);

    TEST_ASSERT_EQUAL_HEX32_ARRAY(original.pixels, converted.pixels, original.height * original.width);
    TEST_ASSERT_EQUAL(original.width, converted.width);
    TEST_ASSERT_EQUAL(original.height, converted.height);
    TEST_ASSERT_EQUAL_PTR(original.format, converted.format);

    SurfaceDestroy(&original);
    SurfaceDestroy(&converted);
}

void test_ConvertedSurfaceWithTheSameAmountOfBppShouldHasTheSameColors() {
    Surface original = SurfaceCreate(3, 2, &FORMAT_ARGB8888);
    
    const Color colors[6] = { RED, GREEN, BLUE, CYAN, MAGENTA, YELLOW };
    for (int i = 0; i < 6; ++i) {
        ((uint32_t*)original.pixels)[i] = ColorToPixel(original.format, colors[i]);
    }

    Surface converted = SurfaceConvert(original, &FORMAT_RGBA8888);
    for (int i = 0; i < 6; ++i) {
        Color color = PixelToColor(&FORMAT_RGBA8888, ((uint32_t*)converted.pixels)[i]);
        TEST_ASSERT_EQUAL_HEX8(colors[i].r, color.r);
        TEST_ASSERT_EQUAL_HEX8(colors[i].g, color.g);
        TEST_ASSERT_EQUAL_HEX8(colors[i].b, color.b);
        TEST_ASSERT_EQUAL_HEX8(colors[i].a, color.a);
    }

    SurfaceDestroy(&original);
    SurfaceDestroy(&converted);
}

void test_ConversionFromHigherToLowerBppShouldWorkCorrectly() {
    Surface original = SurfaceCreate(3, 2, &FORMAT_ARGB8888);
    const Color colors[6] = { RED, GREEN, BLUE, CYAN, MAGENTA, YELLOW };
    for (int i = 0; i < 6; ++i) {
        ((uint32_t*)original.pixels)[i] = ColorToPixel(original.format, colors[i]);
    }

    Surface converted = SurfaceConvert(original, &FORMAT_RGB565);
    const uint16_t expected[6] = {
        ColorToPixel(&FORMAT_RGB565, colors[0]),
        ColorToPixel(&FORMAT_RGB565, colors[1]),
        ColorToPixel(&FORMAT_RGB565, colors[2]),
        ColorToPixel(&FORMAT_RGB565, colors[3]),
        ColorToPixel(&FORMAT_RGB565, colors[4]),
        ColorToPixel(&FORMAT_RGB565, colors[5]),
    };
    TEST_ASSERT_EQUAL_HEX16_ARRAY(expected, converted.pixels, 6);

    SurfaceDestroy(&original);
    SurfaceDestroy(&converted);
}

void test_ConversionFromHLowerToHigherBppShouldReturnSurfaceWithComponentLosses() {
    Surface original = SurfaceCreate(3, 2, &FORMAT_RGB332);
    const Color colors[6] = { RED, GREEN, BLUE, CYAN, MAGENTA, YELLOW };
    for (int i = 0; i < 6; ++i) {
        ((uint8_t*)original.pixels)[i] = ColorToPixel(original.format, colors[i]);
    }

    Surface converted = SurfaceConvert(original, &FORMAT_ARGB8888);
    const uint32_t expected[6] = {
        ColorToPixel(&FORMAT_ARGB8888, PixelToColor(&FORMAT_RGB332, ColorToPixel(&FORMAT_RGB332, colors[0]))),
        ColorToPixel(&FORMAT_ARGB8888, PixelToColor(&FORMAT_RGB332, ColorToPixel(&FORMAT_RGB332, colors[1]))),
        ColorToPixel(&FORMAT_ARGB8888, PixelToColor(&FORMAT_RGB332, ColorToPixel(&FORMAT_RGB332, colors[2]))),
        ColorToPixel(&FORMAT_ARGB8888, PixelToColor(&FORMAT_RGB332, ColorToPixel(&FORMAT_RGB332, colors[3]))),
        ColorToPixel(&FORMAT_ARGB8888, PixelToColor(&FORMAT_RGB332, ColorToPixel(&FORMAT_RGB332, colors[4]))),
        ColorToPixel(&FORMAT_ARGB8888, PixelToColor(&FORMAT_RGB332, ColorToPixel(&FORMAT_RGB332, colors[5]))),
    };
    TEST_ASSERT_EQUAL_HEX32_ARRAY(expected, converted.pixels, 6);

    SurfaceDestroy(&original);
    SurfaceDestroy(&converted);
}

void test_ShouldBlitWhenSurfacesHaveTheSameFormat() {
    Surface dest = SurfaceCreate(3, 2, &FORMAT_RGBA8888);

    uint8_t expected[] = {
        0xFF, 0xFD, 0x00, 0x00,
        0xFF, 0x00, 0xFF, 0x01,
        0xFF, 0xFF, 0xFF, 0xFF,

        0xFF, 0x00, 0x00, 0xFE,
        0xFF, 0xFF, 0xFE, 0x00,
        0xFF, 0x00, 0x00, 0x00,
    };

    const Surface src = (Surface){
        .pixels = expected,
        .width = 3,
        .height = 2,
        .format = &FORMAT_RGBA8888
    };

    SurfaceBlit(dest, src, 0, 0);

    TEST_ASSERT_EQUAL_HEX32_ARRAY(expected, dest.pixels, 6);

    SurfaceDestroy(&dest);
}

void test_ShouldBlitWhenSurfacesHaveDifferentFormats() {
    Surface dest = SurfaceCreate(3, 1, &FORMAT_RGBA8888);

    uint8_t expected[] = {
        0xFF, 0x00, 0x00, 0xFF,
        0xFF, 0x00, 0xFF, 0x00,
        0xFF, 0xFF, 0x00, 0x00,
    };

    uint8_t buffer[] = {
        0x00, 0x00, 0xFF, 0xFF,
        0x00, 0xFF, 0x00, 0xFF,
        0xFF, 0x00, 0x00, 0xFF,
    };

    const Surface src = (Surface){
        .pixels = buffer,
        .width = 3,
        .height = 1,
        .format = &FORMAT_ARGB8888
    };

    SurfaceBlit(dest, src, 0, 0);

    TEST_ASSERT_EQUAL_HEX32_ARRAY(expected, dest.pixels, 3);

    SurfaceDestroy(&dest);
}
