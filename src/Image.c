#include "Image.h"
#include "PixelFormat.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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

    // TODO: on big-endian system it should probably be RGBA8888
    const PixelFormat* format = &FORMAT_ABGR8888;
    return (Surface){ width, height, data, width * format->bytesPerPixel, 0, format };
}
