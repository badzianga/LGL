#include "PixelFormat.h"
#include "unity.h"

void setUp() {}
void tearDown() {}

void test_ColorToPixelShouldReturnEmptyPixelWhenFormatIsNull() {
    const uint32_t pixel = ColorToPixel(NULL, CYAN);
    TEST_ASSERT_EQUAL_HEX32(0, pixel);
}

static const Color testedColor = { 0xAA, 0xBB, 0xCC, 0xDD };

void test_ColorToPixelShouldProperlyConvertToRGBA8888() {
    const uint32_t pixel = ColorToPixel(&FORMAT_RGBA8888, testedColor);
    TEST_ASSERT_EQUAL_HEX32(0xAABBCCDD, pixel);
}

void test_ColorToPixelShouldProperlyConvertToABGR8888() {
    const uint32_t pixel = ColorToPixel(&FORMAT_ABGR8888, testedColor);
    TEST_ASSERT_EQUAL_HEX32(0xDDCCBBAA, pixel);
}

void test_ColorToPixelShouldProperlyConvertToARGB8888() {
    const uint32_t pixel = ColorToPixel(&FORMAT_ARGB8888, testedColor);
    TEST_ASSERT_EQUAL_HEX32(0xDDAABBCC, pixel);
}

void test_ColorToPixelShouldProperlyConvertToBGRA8888() {
    const uint32_t pixel = ColorToPixel(&FORMAT_BGRA8888, testedColor);
    TEST_ASSERT_EQUAL_HEX32(0xCCBBAADD, pixel);
}

void test_ColorToPixelShouldProperlyConvertToRGB565() {
    const uint32_t pixel = ColorToPixel(&FORMAT_RGB565, testedColor);
    TEST_ASSERT_EQUAL_HEX32(0xADD9, pixel);
}

void test_ColorToPixelShouldProperlyConvertToBGR565() {
    const uint32_t pixel = ColorToPixel(&FORMAT_BGR565, testedColor);
    TEST_ASSERT_EQUAL_HEX32(0xCDD5, pixel);
}

void test_ColorToPixelShouldProperlyConvertToRGB332() {
    const uint32_t pixel = ColorToPixel(&FORMAT_RGB332, testedColor);
    TEST_ASSERT_EQUAL_HEX32(0xB7, pixel);
}

void test_ColorToPixelShouldProperlyConvertToBGR233() {
    const uint32_t pixel = ColorToPixel(&FORMAT_BGR233, testedColor);
    TEST_ASSERT_EQUAL_HEX32(0xED, pixel);
}

#define COMPARE_COLOR_COMPONENTS(expected, actual)         \
    do {                                                   \
        TEST_ASSERT_EQUAL_HEX8((expected).r, (actual).r);  \
        TEST_ASSERT_EQUAL_HEX8((expected).g, (actual).g);  \
        TEST_ASSERT_EQUAL_HEX8((expected).b, (actual).b);  \
        TEST_ASSERT_EQUAL_HEX8((expected).a, (actual).a);  \
    } while (0);

void test_PixelToColorShouldReturnEmptyColorWhenFormatIsNull() {
    const Color expected = (Color){ 0 };
    const Color actual = PixelToColor(NULL, 0xDEADBEEF);

    COMPARE_COLOR_COMPONENTS(expected, actual);
}

void test_PixelToColorShouldProperlyConvertFromRGBA8888() {
    const Color expected = { 0xAA, 0xBB, 0xCC, 0xDD };
    const uint32_t pixel = 0xAABBCCDD;
    const Color actual = PixelToColor(&FORMAT_RGBA8888, pixel);

    COMPARE_COLOR_COMPONENTS(expected, actual);
}

void test_PixelToColorShouldProperlyConvertFromABGR8888() {
    const Color expected = { 0xAA, 0xBB, 0xCC, 0xDD };
    const uint32_t pixel = 0xDDCCBBAA;
    const Color actual = PixelToColor(&FORMAT_ABGR8888, pixel);

    COMPARE_COLOR_COMPONENTS(expected, actual);
}

void test_PixelToColorShouldProperlyConvertFromARGB8888() {
    const Color expected = { 0xAA, 0xBB, 0xCC, 0xDD };
    const uint32_t pixel = 0xDDAABBCC;
    const Color actual = PixelToColor(&FORMAT_ARGB8888, pixel);

    COMPARE_COLOR_COMPONENTS(expected, actual);
}

void test_PixelToColorShouldProperlyConvertFromBGRA8888() {
    const Color expected = { 0xAA, 0xBB, 0xCC, 0xDD };
    const uint32_t pixel = 0xCCBBAADD;
    const Color actual = PixelToColor(&FORMAT_BGRA8888, pixel);

    COMPARE_COLOR_COMPONENTS(expected, actual);
}

void test_PixelToColorShouldProperlyConvertFromRGB565() {
    const Color expected = { 0xA8, 0xB8, 0xC8, 0x00 };
    const uint32_t pixel = 0xADD9;
    const Color actual = PixelToColor(&FORMAT_RGB565, pixel);

    COMPARE_COLOR_COMPONENTS(expected, actual);
    
}

void test_PixelToColorShouldProperlyConvertFromBGR565() {
    const Color expected = { 0xA8, 0xB8, 0xC8, 0x00 };
    const uint32_t pixel = 0xCDD5;
    const Color actual = PixelToColor(&FORMAT_BGR565, pixel);

    COMPARE_COLOR_COMPONENTS(expected, actual);
}

void test_PixelToColorShouldProperlyConvertFromRGB332() {
    const Color expected = { 0xA0, 0xA0, 0xC0, 0x00 };
    const uint32_t pixel = 0xB7;
    const Color actual = PixelToColor(&FORMAT_RGB332, pixel);

    COMPARE_COLOR_COMPONENTS(expected, actual);
}

void test_PixelToColorShouldProperlyConvertFromBGR233() {
    const Color expected = { 0xA0, 0xA0, 0xC0, 0x00 };
    const uint32_t pixel = 0xED;
    const Color actual = PixelToColor(&FORMAT_BGR233, pixel);

    COMPARE_COLOR_COMPONENTS(expected, actual);
}
