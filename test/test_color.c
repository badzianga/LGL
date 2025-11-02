#include "unity.h"
#include "color.h"

void setUp(void) {}
void tearDown(void) {}

void test_ColorShouldBeConstructedProperly(void) {
    Color color = ColorConstructRGBA(0, 63, 127, 255);

    TEST_ASSERT_EQUAL(0, color.r);
    TEST_ASSERT_EQUAL(63, color.g);
    TEST_ASSERT_EQUAL(127, color.b);
    TEST_ASSERT_EQUAL(255, color.a);
}

void test_ColorGettersShouldReturnProperComponents(void) {
    Color color = ColorConstructRGBA(255, 127, 63, 0);

    TEST_ASSERT_EQUAL(255, ColorGetR(color));
    TEST_ASSERT_EQUAL(127, ColorGetG(color));
    TEST_ASSERT_EQUAL(63, ColorGetB(color));
    TEST_ASSERT_EQUAL(0, ColorGetA(color));
}

static void compareColors(Color expected, Color actual) {
    TEST_ASSERT_EQUAL(expected.r, actual.r);
    TEST_ASSERT_EQUAL(expected.g, actual.g);
    TEST_ASSERT_EQUAL(expected.b, actual.b);
    TEST_ASSERT_EQUAL(expected.a, actual.a);
}

void test_ColorConstantsShouldBeProperlyDefined(void) {
    compareColors(ColorConstructRGBA(0xFF, 0x00, 0x00, 0xFF), RED);
    compareColors(ColorConstructRGBA(0x00, 0xFF, 0x00, 0xFF), GREEN);
    compareColors(ColorConstructRGBA(0x00, 0x00, 0xFF, 0xFF), BLUE);
    compareColors(ColorConstructRGBA(0x00, 0xFF, 0xFF, 0xFF), CYAN);
    compareColors(ColorConstructRGBA(0xFF, 0x00, 0xFF, 0xFF), MAGENTA);
    compareColors(ColorConstructRGBA(0xFF, 0xFF, 0x00, 0xFF), YELLOW);
    compareColors(ColorConstructRGBA(0x00, 0x00, 0x00, 0xFF), BLACK);
    compareColors(ColorConstructRGBA(0xFF, 0xFF, 0xFF, 0xFF), WHITE);
}
