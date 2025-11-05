#include "Image.h"
#include "Color.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
// #define STB_IMAGE_WRITE_IMPLEMENTATION
// #include "stb_image_write.h"

Surface ImageLoad(const char* path) {
    int format;
    Surface surface;

    surface.pixels = (Color*)stbi_load(
        path,
        &surface.width,
        &surface.height,
        &format,
        sizeof(Color));

    surface.format = &FORMAT_RGBA8888;

    return surface;
}

void ImageSave(Surface image, const char* path) {
    (void) image;
    (void) path;
    assert(0 && "TODO: not implemented");
}
