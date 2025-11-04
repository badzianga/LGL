#include "image.h"
#include "color.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

Surface ImageLoad(const char* path) {
    int format;
    Surface surface;
    surface.pixels = (Color*)stbi_load(
        path,
        &surface.width,
        &surface.height,
        &format,
        sizeof(Color));
    const Color* end = &surface.pixels[surface.width * surface.height];
    for (Color* pixel = surface.pixels; pixel != end; ++pixel) {
        Color new = { pixel->b, pixel->g, pixel->r, pixel->a };
        *pixel = new;
    }

    return surface;
}

void ImageSave(Surface image, const char* path) {
    const Color* end = &image.pixels[image.width * image.height];
    for (Color* pixel = image.pixels; pixel != end; ++pixel) {
        Color new = { pixel->b, pixel->g, pixel->r, pixel->a };
        *pixel = new;
    }

    stbi_write_png(
        path,
        image.width,
        image.height,
        sizeof(Color),
        image.pixels,
        image.width * (int)sizeof(Color));
}
