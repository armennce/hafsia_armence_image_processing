
#ifndef BMP24_H
#define BMP24_H

#include <stdint.h>
#include <stdio.h>

typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} t_pixel;

typedef struct {
    uint16_t type;
    uint32_t size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offset;
} t_bmp_header;

typedef struct {
    uint32_t size;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bits;
    uint32_t compression;
    uint32_t imageSize;
    int32_t xResolution;
    int32_t yResolution;
    uint32_t nColors;
    uint32_t importantColors;
} t_bmp_info;

typedef struct {
    t_bmp_header header;
    t_bmp_info header_info;
    int width;
    int height;
    int colorDepth;
    t_pixel **data;
} t_bmp24;

t_pixel **bmp24_allocateDataPixels(int width, int height);
void bmp24_freeDataPixels(t_pixel **pixels, int height);
t_bmp24 *bmp24_allocate(int width, int height, int colorDepth);
void bmp24_free(t_bmp24 *img);

t_bmp24 *bmp24_loadImage(const char *filename);
void bmp24_saveImage(t_bmp24 *img, const char *filename);

void bmp24_negative(t_bmp24 *img);
void bmp24_grayscale(t_bmp24 *img);
void bmp24_brightness(t_bmp24 *img, int value);

#endif
