#include "Image.h"
#include "PixelFormat.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
// #define STB_IMAGE_WRITE_IMPLEMENTATION
// #include "stb_image_write.h"

Surface ImageLoad(const char* path) {
    int format;
    Surface surface;

    void* data = stbi_load(
        path,
        &surface.width,
        &surface.height,
        &format,
        STBI_rgb_alpha
    );

    if (data == NULL) {
        return (Surface){ 0 };
    }

    surface.pixels = data;
    surface.format = &FORMAT_RGBA8888;

    return surface;
}

void ImageSave(Surface image, const char* path) {
    (void) image;
    (void) path;
    assert(0 && "TODO: not implemented");
}
