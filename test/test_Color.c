#include "Color.h"
#include "unity.h"

void setUp(void) {}
void tearDown(void) {}

void test_ColorStructShouldHaveProperSize() {
    TEST_ASSERT_EQUAL(4, sizeof(Color));
}

void test_ColorShouldBeConstructedProperly(void) {
    Color color = { 0, 63, 127, 255 };

    TEST_ASSERT_EQUAL(  0, color.r);
    TEST_ASSERT_EQUAL( 63, color.g);
    TEST_ASSERT_EQUAL(127, color.b);
    TEST_ASSERT_EQUAL(255, color.a);
}

void test_ConstantRedShouldBeProperlyDefined() {
    Color color = RED;

    TEST_ASSERT_EQUAL(255, color.r);
    TEST_ASSERT_EQUAL(  0, color.g);
    TEST_ASSERT_EQUAL(  0, color.b);
    TEST_ASSERT_EQUAL(255, color.a);
}

void test_ConstantGreenShouldBeProperlyDefined() {
    Color color = GREEN;

    TEST_ASSERT_EQUAL(  0, color.r);
    TEST_ASSERT_EQUAL(255, color.g);
    TEST_ASSERT_EQUAL(  0, color.b);
    TEST_ASSERT_EQUAL(255, color.a);
}

void test_ConstantBlueShouldBeProperlyDefined() {
    Color color = BLUE;

    TEST_ASSERT_EQUAL(  0, color.r);
    TEST_ASSERT_EQUAL(  0, color.g);
    TEST_ASSERT_EQUAL(255, color.b);
    TEST_ASSERT_EQUAL(255, color.a);
}

void test_ConstantCyanShouldBeProperlyDefined() {
    Color color = CYAN;

    TEST_ASSERT_EQUAL(  0, color.r);
    TEST_ASSERT_EQUAL(255, color.g);
    TEST_ASSERT_EQUAL(255, color.b);
    TEST_ASSERT_EQUAL(255, color.a);
}

void test_ConstantMagentaShouldBeProperlyDefined() {
    Color color = MAGENTA;

    TEST_ASSERT_EQUAL(255, color.r);
    TEST_ASSERT_EQUAL(  0, color.g);
    TEST_ASSERT_EQUAL(255, color.b);
    TEST_ASSERT_EQUAL(255, color.a);
}

void test_ConstantYellowShouldBeProperlyDefined() {
    Color color = YELLOW;

    TEST_ASSERT_EQUAL(255, color.r);
    TEST_ASSERT_EQUAL(255, color.g);
    TEST_ASSERT_EQUAL(  0, color.b);
    TEST_ASSERT_EQUAL(255, color.a);
}

void test_ConstantBlackShouldBeProperlyDefined() {
    Color color = BLACK;

    TEST_ASSERT_EQUAL(  0, color.r);
    TEST_ASSERT_EQUAL(  0, color.g);
    TEST_ASSERT_EQUAL(  0, color.b);
    TEST_ASSERT_EQUAL(255, color.a);
}

void test_ConstantWhiteShouldBeProperlyDefined() {
    Color color = WHITE;

    TEST_ASSERT_EQUAL(255, color.r);
    TEST_ASSERT_EQUAL(255, color.g);
    TEST_ASSERT_EQUAL(255, color.b);
    TEST_ASSERT_EQUAL(255, color.a);
}
