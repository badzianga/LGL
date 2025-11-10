#include "Image.h"
#include "PixelFormat.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
// #define STB_IMAGE_WRITE_IMPLEMENTATION
// #include "stb_image_write.h"

Surface ImageLoad(const char* path) {
    int width, height, comp;

    void* data = stbi_load(
        path,
        &width,
        &height,
        &comp,
        STBI_rgb_alpha
    );

    if (data == NULL) {
        return (Surface){ 0 };
    }

    // TODO: on big-endian system it should probabyl be RGBA8888
    return (Surface){ data, width, height, &FORMAT_ABGR8888 };
}

void ImageSave(Surface image, const char* path) {
    (void) image;
    (void) path;
    assert(0 && "TODO: not implemented");
}
