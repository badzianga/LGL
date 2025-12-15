#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "Allocator.h"
#include "Error.h"
#include "Image.h"
#include "PixelFormat.h"

typedef struct __attribute__((packed)) {
    uint16_t type;      // header field used to identify file
    uint32_t fileSize;  // size of file in bytes
    uint32_t reserved;  // reserved
    uint32_t offset;    // starting address of the pixel array
} BitmapHeader;

typedef struct __attribute__((packed)) {
    uint32_t size;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bitCount;
    uint32_t compression;
    uint32_t imageSize;
    int32_t horizontalResolution;
    int32_t verticalResolution;
    uint32_t colorsUsed;
    uint32_t importantColorsUsed;
} BitmapInfo;

#define BMP_FILE_TYPE 0x4D42

typedef enum CompressionMethod {
    BI_RGB            = 0,
    BI_RLE8           = 1,
    BI_RLE4           = 2,
    BI_BITFIELDS      = 3,
    BI_JPEG           = 4,
    BI_PNG            = 5,
    BI_ALPHABITFIELDS = 6,
    BI_CMYK           = 11,
    BI_CMYKRLE8       = 12,
    BI_CMYKRLE4       = 13,
} CompressionMethod;

static Surface LoadRGB(FILE* f, const BitmapInfo* info, const BitmapHeader* header) {
    if ((info->bitCount != 24 && info->bitCount != 32)) {
        fclose(f);
        THROW_ERROR(ERR_INVALID_PARAMS);
        return (Surface){};
    }

    const int width = info->width;
    const int height = abs(info->height);

    Surface surface = SurfaceCreate(width, height, &FORMAT_ARGB8888);
    surface.flags = SURFACE_FLAG_NONE;

    fseek(f, header->offset, SEEK_SET);

    const int srcBpp = info->bitCount >> 3;
    const int srcStride = ((width * srcBpp + 3) & ~3);
    const int bottomUp = (info->height > 0);
    const int lastRow = height - 1;

    uint8_t* srcRow = AllocatorAlloc(srcStride);

    for (int y = 0; y < height; ++y) {
        fread(srcRow, srcStride, 1, f);

        const int dstY = bottomUp ? (lastRow - y) : y;
        uint8_t* dst = (uint8_t*)surface.pixels + dstY * surface.stride;

        for (int x = 0; x < width; x++) {
            const int thisX = x * srcBpp;
            const uint8_t b = srcRow[thisX + 0];
            const uint8_t g = srcRow[thisX + 1];
            const uint8_t r = srcRow[thisX + 2];
            const uint8_t a = 255;

            dst[(x << 2) + 0] = b;
            dst[(x << 2) + 1] = g;
            dst[(x << 2) + 2] = r;
            dst[(x << 2) + 3] = a;
        }
    }

    AllocatorFree(srcRow);
    fclose(f);

    return surface;
}

static inline uint8_t ExtractComponent(uint32_t pixel, uint32_t mask, uint8_t shift, uint8_t loss) {
    const uint32_t v = (pixel & mask) >> shift;
    return (uint8_t)(v << loss);
}

static Surface LoadBitfields(FILE* f, const BitmapInfo* info, const BitmapHeader* header) {
    uint32_t rMask = 0;
    uint32_t gMask = 0;
    uint32_t bMask = 0;
    uint32_t aMask = 0;

    fread(&rMask, sizeof(uint32_t), 1, f);
    fread(&gMask, sizeof(uint32_t), 1, f);
    fread(&bMask, sizeof(uint32_t), 1, f);
    if (info->bitCount == 32) {
        fread(&aMask, sizeof(uint32_t), 1, f);
    }

    const PixelFormat* format = FindPixelFormatByMasks(rMask, gMask, bMask, aMask);
    if (format == NULL) {
        format = FindPixelFormatByMasksExcludingAlpha(rMask, gMask, bMask);
        if (format == NULL) {
            fclose(f);
            THROW_ERROR(ERR_UNKNOWN_FORMAT);
            return (Surface){};
        }
    }

    const int width = info->width;
    const int height = abs(info->height);

    Surface surface = SurfaceCreate(width, height, format);
    surface.flags = SURFACE_FLAG_NONE;

    fseek(f, header->offset, SEEK_SET);

    const int srcBpp = info->bitCount >> 3;
    const int srcStride = ((width * srcBpp + 3) & ~3);  // align to 4 bytes
    const int bottomUp = (info->height > 0);
    const int lastRow = height - 1;
    const bool shouldAddAlpha = info->bitCount >= 24 && !aMask;

    uint8_t* srcRow = AllocatorAlloc(srcStride);

    for (int y = 0; y < height; ++y) {
        fread(srcRow, srcStride, 1, f);

        const int dstY = bottomUp ? (lastRow - y) : y;
        uint8_t* dst = (uint8_t*)surface.pixels + dstY * surface.stride;

        for (int x = 0; x < width; x++) {
            uint32_t srcPixel = *(uint32_t*)(srcRow + x * srcBpp);
            if (shouldAddAlpha) {
                srcPixel |= (0xFF << format->aShift);
            }
            else {
                const uint8_t a = ExtractComponent(srcPixel, format->aMask, format->aShift, format->aLoss);
                if (a < 255) {
                    surface.flags |= SURFACE_FLAG_HAS_ALPHA;
                }
            }

            switch (format->bytesPerPixel) {
                case 1: {
                    *(dst + x) = (uint8_t)srcPixel;
                } break;
                case 2: {
                    *(uint16_t*)(dst + (x << 1)) = (uint16_t)srcPixel;
                } break;
                case 4: {
                    *(uint32_t*)(dst + (x << 2)) = srcPixel;
                } break;
                default: break;
            }
        }
    }

    AllocatorFree(srcRow);
    fclose(f);

    return surface;
}

Surface ImageLoadBMP(const char* path) {
    if (!path) {
        THROW_ERROR(ERR_INVALID_PARAMS);
        return (Surface){};
    }
    FILE* f = fopen(path, "rb");
    if (!f) {
        THROW_ERROR(ERR_FILE_NOT_FOUND);
        return (Surface){};
    }

    BitmapHeader header;
    fread(&header, sizeof(BitmapHeader), 1, f);

    if (header.type != BMP_FILE_TYPE) {
        fclose(f);
        THROW_ERROR(ERR_INVALID_PARAMS);
        return (Surface){};
    }

    BitmapInfo info;
    fread(&info, sizeof(BitmapInfo), 1, f);

    switch (info.compression) {
        case BI_RGB: return LoadRGB(f, &info, &header);
        case BI_BITFIELDS: return LoadBitfields(f, &info, &header);
        default: {
            fclose(f);
            THROW_ERROR(ERR_INTERNAL_ERROR);
            return (Surface){};
        }
    }
}

void ImageSaveBMP(Surface image, const char* path) {
    if (!image.pixels || !image.format || !path) {
        THROW_ERROR(ERR_INVALID_PARAMS);
        return;
    }

    const int width = image.width;
    const int height = image.height;

    const int bytesPP = image.format->bytesPerPixel;
    const int bitsPP = bytesPP << 3;

    const int dstStride = ((width * bytesPP + 3) & ~3);  // align to 4 bytes by applying necessary padding
    const uint32_t imageSize = dstStride * height;

    FILE* f = fopen(path, "wb");
    if (!f) {
        THROW_ERROR(ERR_INVALID_PARAMS);
        return;
    }

    BitmapHeader fileHeader = { 0 };
    fileHeader.type = BMP_FILE_TYPE;
    fileHeader.offset = sizeof(BitmapHeader) + sizeof(BitmapInfo) + 16;
    fileHeader.fileSize = fileHeader.offset + imageSize;
    fwrite(&fileHeader, sizeof(fileHeader), 1, f);

    BitmapInfo infoHeader = { 0 };
    infoHeader.size = sizeof(BitmapInfo);
    infoHeader.width = width;
    infoHeader.height = height;
    infoHeader.planes = 1;
    infoHeader.bitCount = bitsPP;
    infoHeader.compression = BI_BITFIELDS;
    infoHeader.imageSize = imageSize;
    infoHeader.horizontalResolution = 2835; // 72 DPI
    infoHeader.verticalResolution = 2835;
    fwrite(&infoHeader, sizeof(infoHeader), 1, f);

    const uint32_t rMask = image.format->rMask;
    const uint32_t gMask = image.format->gMask;
    const uint32_t bMask = image.format->bMask;
    const uint32_t aMask = image.format->aMask;

    fwrite(&rMask, sizeof(rMask), 1, f);
    fwrite(&gMask, sizeof(gMask), 1, f);
    fwrite(&bMask, sizeof(bMask), 1, f);
    fwrite(&aMask, sizeof(aMask), 1, f);

    uint8_t* row = AllocatorAlloc(dstStride);
    memset(row, 0, dstStride);

    for (int y = height - 1; y >= 0; y--) {
        const uint8_t* src = (uint8_t*)image.pixels + y * image.stride;
        memcpy(row, src, image.width * bytesPP);
        fwrite(row, dstStride, 1, f);
    }

    AllocatorFree(row);
    fclose(f);
}
