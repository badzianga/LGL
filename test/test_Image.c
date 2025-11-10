#include "unity.h"
#include "Image.h"
#include "PixelFormat.h"
#include "Surface.h"

void setUp(void) {}
void tearDown(void) {}

void test_ShouldLoadPngImageSuccessfully() {
    Surface image = ImageLoad("test/support/colors.png");

    TEST_ASSERT_NOT_NULL(image.pixels);
    TEST_ASSERT_EQUAL(3, image.width);
    TEST_ASSERT_EQUAL(2, image.height);
    TEST_ASSERT_EQUAL_PTR(&FORMAT_RGBA8888, image.format);

    const uint8_t* pixelArray = (uint8_t*)image.pixels;
    const uint8_t expected[] = {
        0xFF, 0x00, 0x00, 0xFF,
        0x00, 0xFF, 0x00, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,

        0x00, 0x00, 0xFF, 0xFF,
        0xFF, 0xFF, 0x00, 0xFF,
        0x00, 0x00, 0x00, 0x80,
    };

    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, pixelArray, sizeof(expected));

    SurfaceDestroy(&image);
}

void test_ShouldLoadJpgImageSuccessfully() {
    Surface image = ImageLoad("test/support/colors.jpg");

    TEST_ASSERT_NOT_NULL(image.pixels);
    TEST_ASSERT_EQUAL(3, image.width);
    TEST_ASSERT_EQUAL(2, image.height);
    TEST_ASSERT_EQUAL_PTR(&FORMAT_RGBA8888, image.format);

    const uint8_t* pixelArray = (uint8_t*)image.pixels;
    const uint8_t expected[] = {
        0xFD, 0x00, 0x00, 0xFF,
        0x00, 0xFF, 0x01, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,

        0x00, 0x00, 0xFE, 0xFF,
        0xFF, 0xFE, 0x00, 0xFF,
        0x00, 0x00, 0x00, 0xFF,
    };

    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, pixelArray, sizeof(expected));

    SurfaceDestroy(&image);
}
