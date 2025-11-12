#include "BitmapFont.h"
#include "FillRect.h"
#include "PixelFormat.h"
#include "Surface.h"
#include "unity.h"

#define SURF_W 8
#define SURF_H 8

static Surface surface;

void setUp(void) {
    surface = SurfaceCreate(8, 8, &FORMAT_RGB332);
    FillRect(surface, 0, 0, SURF_W, SURF_H, BLACK);
}

void tearDown(void) {
    SurfaceDestroy(&surface);
}

void test_InvalidCharShouldNotBeDrawn() {
    const char c = 10;
    DrawCharBitmapFont(surface, 0, 0, c, &DEFAULT_BITMAP_FONT, WHITE);

    const uint8_t expected[SURF_W * SURF_H] = { 0 };

    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, surface.pixels, SURF_W * SURF_H);
}

void test_CharOutOfSurfaceBoundariesShouldNotBeDrawn() {
    const char c = 'X';
    DrawCharBitmapFont(surface, -10, 0, c, &DEFAULT_BITMAP_FONT, WHITE);
    DrawCharBitmapFont(surface, 10, 0, c, &DEFAULT_BITMAP_FONT, WHITE);
    DrawCharBitmapFont(surface, 0, -10, c, &DEFAULT_BITMAP_FONT, WHITE);
    DrawCharBitmapFont(surface, 0, 10, c, &DEFAULT_BITMAP_FONT, WHITE);

    const uint8_t expected[SURF_W * SURF_H] = { 0 };

    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, surface.pixels, SURF_W * SURF_H);
}

void test_WholeCharShouldBeDrawn() {
    DrawCharBitmapFont(surface, 0, 0, 'A', &DEFAULT_BITMAP_FONT, BLUE);
    
    const uint8_t X = 0x03;

    const uint8_t expected[SURF_W * SURF_H] = {
        0, 0, X, X, 0, 0, 0, 0,
        0, X, X, X, X, 0, 0, 0,
        X, X, 0, 0, X, X, 0, 0,
        X, X, 0, 0, X, X, 0, 0,
        X, X, X, X, X, X, 0, 0,
        X, X, 0, 0, X, X, 0, 0,
        X, X, 0, 0, X, X, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
    };

    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, surface.pixels, SURF_W * SURF_H);
}

void test_ClippedCharShouldBeDrawn() {
    DrawCharBitmapFont(surface, -3, 0, 'A', &DEFAULT_BITMAP_FONT, BLUE);
    
    const uint8_t X = 0x03;

    const uint8_t expected[SURF_W * SURF_H] = {
        X, 0, 0, 0, 0, 0, 0, 0,
        X, X, 0, 0, 0, 0, 0, 0,
        0, X, X, 0, 0, 0, 0, 0,
        0, X, X, 0, 0, 0, 0, 0,
        X, X, X, 0, 0, 0, 0, 0,
        0, X, X, 0, 0, 0, 0, 0,
        0, X, X, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
    };

    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, surface.pixels, SURF_W * SURF_H);
}

void test_TextShouldBeDrawnEvenWhenClipped() {
    DrawTextBitmapFont(surface, -3, 0, "AB", &DEFAULT_BITMAP_FONT, BLUE);
    
    const uint8_t X = 0x03;

    const uint8_t expected[SURF_W * SURF_H] = {
        X, 0, 0, 0, 0, X, X, X,
        X, X, 0, 0, 0, X, X, 0,
        0, X, X, 0, 0, X, X, 0,
        0, X, X, 0, 0, X, X, X,
        X, X, X, 0, 0, X, X, 0,
        0, X, X, 0, 0, X, X, 0,
        0, X, X, 0, 0, X, X, X,
        0, 0, 0, 0, 0, 0, 0, 0,
    };

    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, surface.pixels, SURF_W * SURF_H);
}

void test_TextWithNewlineShouldBeDrawnEventWhenClipped() {
    DrawTextBitmapFont(surface, 0, -4, "a\nb", &DEFAULT_BITMAP_FONT, BLUE);

    const uint8_t X = 0x03;
    const uint8_t expected[SURF_W * SURF_H] = {
        0, X, X, X, X, X, 0, 0,
        X, X, 0, 0, X, X, 0, 0,
        0, X, X, X, X, X, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        X, X, 0, 0, 0, 0, 0, 0,
        X, X, 0, 0, 0, 0, 0, 0,
        X, X, X, X, X, 0, 0, 0,
        X, X, 0, 0, X, X, 0, 0,
    };

    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, surface.pixels, SURF_W * SURF_H);
}
