//
// Created by Maxence on 25/05/2025.
//

#ifndef IMAGE_PROCESSING_1_BMP24_H
#define IMAGE_PROCESSING_1_BMP24_H

#endif //IMAGE_PROCESSING_1_BMP24_H
#ifndef BMP24_H
#define BMP24_H

#include "utils.h" // For common utilities and standard headers

// Structure for BMP file header (14 bytes)
#pragma pack(push, 1) // Exact memory layout, no padding
typedef struct {
    uint16_t type;              // File type, must be "BM" (0x4D42)
    uint32_t size;              // Size of the BMP file in bytes
    uint16_t reserved1;         // Reserved; must be 0
    uint16_t reserved2;         // Reserved; must be 0
    uint32_t offset;            // Offset to start of pixel data
} t_bmp_header;

// Structure for BMP info header (DIB header - BITMAPINFOHEADER, 40 bytes)
typedef struct {
    uint32_t size;              // Size of this header (40 bytes)
    int32_t  width;             // Image width in pixels
    int32_t  height;            // Image height in pixels
    uint16_t planes;            // Number of color planes (must be 1)
    uint16_t bits;              // Bits per pixel (1, 4, 8, 16, 24, or 32)
    uint32_t compression;       // Compression type (0 for BI_RGB, no compression)
    uint32_t imagesize;         // Image size in bytes (can be 0 for BI_RGB)
    int32_t  xresolution;       // Horizontal resolution (pixels per meter)
    int32_t  yresolution;       // Vertical resolution (pixels per meter)
    uint32_t ncolors;           // Number of colors in color palette (0 for 24-bit)
    uint32_t importantcolors;   // Number of important colors (0 when all are important)
} t_bmp_info;
#pragma pack(pop)

// Structure for a single pixel (RGB)
typedef struct {
    uint8_t blue;  // BMP stores in BGR order, but we might use RGB internally
    uint8_t green;
    uint8_t red;
} t_pixel; // Note: The PDF defines red, green, blue order. Let's stick to that.

// Re-defining t_pixel as per PDF for clarity (red, green, blue)
typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} t_rgb_pixel;


// Structure for a 24-bit BMP image
typedef struct {
    t_bmp_header header;    // File header
    t_bmp_info info;        // Info header (DIB)

    // Extracted/convenience fields (redundant but useful)
    // int width; (use info.width)
    // int height; (use info.height)
    // int colorDepth; (use info.bits)

    t_rgb_pixel **data;      // Pixel data (2D array of pixels)
} t_bmp24;


// --- Allocation and Deallocation Functions ---
t_rgb_pixel **bmp24_allocateDataPixels(int width, int height);
void bmp24_freeDataPixels(t_rgb_pixel **pixels, int height);
t_bmp24 *bmp24_allocate(int width, int height, int colorDepth);
void bmp24_free(t_bmp24 *img);

// --- Loading and Saving 24-bit Images ---
void bmp24_readPixelData(t_bmp24 *image, FILE *file); // Reads all pixel data
void bmp24_writePixelData(t_bmp24 *image, FILE *file); // Writes all pixel data

t_bmp24 *bmp24_loadImage(const char *filename);
void bmp24_saveImage(const char *filename, t_bmp24 *img);
void bmp24_printInfo(t_bmp24 *img);


// --- Image Processing Functions (24-bit) ---
void bmp24_negative(t_bmp24 *img);
void bmp24_grayscale(t_bmp24 *img);
void bmp24_brightness(t_bmp24 *img, int value);

// Convolution for a single pixel (returns new pixel value)
t_rgb_pixel bmp24_convolution_pixel(t_bmp24 *img, int x, int y, float **kernel, int kernelSize, t_rgb_pixel **temp_data);

// Functions to apply filters by calling bmp24_convolution_pixel for each pixel
void bmp24_boxBlur(t_bmp24 *img);
void bmp24_gaussianBlur(t_bmp24 *img);
void bmp24_outline(t_bmp24 *img);
void bmp24_emboss(t_bmp24 *img);
void bmp24_sharpen(t_bmp24 *img);


#endif // BMP24_H

