#include "Image.h"
#include "PixelFormat.h"
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

Surface ImageLoad(const char* path) {
    int width, height, comp;

    uint8_t* data = stbi_load(
        path,
        &width,
        &height,
        &comp,
        STBI_rgb_alpha
    );

    if (data == NULL) {
        return (Surface){ 0 };
    }

    // auto convert to display format (ARGB) and set proper flags
    SurfaceFlags flags = 0;
    for (int i = 0; i < width * height; i++) {
        uint8_t* pixelStart = data + (i << 2);

        const uint8_t tmp = pixelStart[0];
        pixelStart[0] = pixelStart[2];
        pixelStart[2] = tmp;

        if (pixelStart[3] < 0xFF) {
            flags |= SURFACE_FLAG_HAS_ALPHA;
        }
    }

    const PixelFormat* format = &FORMAT_ARGB8888;
    return (Surface){ width, height, data, width * format->bytesPerPixel, flags, format };
}

void ImageSave(Surface image, const char* path) {
    if (!path || !image.pixels || !image.format) return;
    const PixelFormat* pngFormat = &FORMAT_ABGR8888;
    Surface output;
    if (image.format != pngFormat) {
        output = SurfaceConvert(image, pngFormat);
    }
    else {
        output = image;
    }
    stbi_write_png(path, output.width, output.height, 4, output.pixels, output.stride);
    if (image.format != pngFormat) {
        SurfaceDestroy(&output);
    }
}
