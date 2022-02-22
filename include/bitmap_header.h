#ifndef __BITMAP_HEADER_H__
#define __BITMAP_HEADER_H__

#include <cinttypes>

namespace dng {

#pragma pack(1)
union BitmapHeader {
    char rawData[54];
    struct {
        struct {
            char signature[2];
            uint32_t fileSize;
            uint8_t reserved[4];
            uint32_t dataOffset;
        } header;
        struct {
            uint32_t size;
            uint32_t width;
            uint32_t height;
            uint16_t planes;
            uint16_t bpp;
            uint32_t compression;
            uint32_t imageSize;
            uint32_t xPixelsPerM;
            uint32_t yPixelsPerM;
            uint32_t colorsUsed;
            uint32_t importantColors;
        } dib;
    };

    BitmapHeader(size_t width = 0, size_t height = 0, size_t bpp = 24) {
        header.signature[0] = 'B';
        header.signature[1] = 'M';
        header.fileSize = sizeof(BitmapHeader) + (width * height * (bpp / 8));
        header.dataOffset = sizeof(BitmapHeader);
        dib.size = sizeof(dib);
        dib.width = width;
        dib.height = height;
        dib.planes = 1;
        dib.bpp = bpp;
        dib.compression = 0;
        dib.imageSize = 0;
        dib.xPixelsPerM = 0;
        dib.yPixelsPerM = 0;
        dib.colorsUsed = 0;
        dib.importantColors = 0;
    }
};

}  // namespace dng

#endif  // __BITMAP_HEADER_H__
