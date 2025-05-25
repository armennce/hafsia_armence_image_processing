//
// Created by Maxence on 25/05/2025.
//

#ifndef IMAGE_PROCESSING_1_UTILS_H
#define IMAGE_PROCESSING_1_UTILS_H

#endif //IMAGE_PROCESSING_1_UTILS_H
#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdint.h> // For uint16_t, uint32_t, int32_t, uint8_t
#include <stdlib.h> // For malloc, free, exit
#include <string.h> // For memcpy, memset
#include <math.h>   // For round, fabs

// BMP Header Constants (mostly for Part 2, but good to have)
#define BITMAP_MAGIC        0x00 // Offset for 'BM' signature
#define BITMAP_FILE_SIZE    0x02 // Offset for file size
#define BITMAP_OFFSET       0x0A // Offset for pixel data offset

#define BITMAP_HEADER_SIZE  0x0E // Offset for DIB header size (after file header)
#define BITMAP_WIDTH        0x12 // Offset for image width
#define BITMAP_HEIGHT       0x16 // Offset for image height
#define BITMAP_PLANES       0x1A // Offset for color planes
#define BITMAP_DEPTH        0x1C // Offset for bits per pixel (color depth)
#define BITMAP_COMPRESSION  0x1E // Offset for compression type
#define BITMAP_IMG_SIZE_RAW 0x22 // Offset for image size in bytes (can be 0 for BI_RGB)
#define BITMAP_X_RES        0x26 // Offset for horizontal resolution
#define BITMAP_Y_RES        0x2A // Offset for vertical resolution
#define BITMAP_N_COLORS     0x2E // Offset for number of colors in palette
#define BITMAP_IMP_COLORS   0x32 // Offset for important colors

// Specific values
#define BMP_TYPE_MAGIC_VALUE 0x4D42 // 'BM' in little-endian (0x42 0x4D)
#define DEFAULT_HEADER_SIZE_VALUE 14 // Size of t_bmp_header (file header part)
#define DEFAULT_INFO_SIZE_VALUE 40   // Size of t_bmp_info (DIB header part, BITMAPINFOHEADER)
#define DEFAULT_DEPTH_24BIT 24
#define DEFAULT_DEPTH_8BIT  8

// Helper function for reading raw data from a file at a specific position
void file_rawRead(uint32_t position, void *buffer, uint32_t size, size_t n, FILE *file);

// Helper function for writing raw data to a file at a specific position
void file_rawWrite(uint32_t position, void *buffer, uint32_t size, size_t n, FILE *file);

// Function to allocate a 2D kernel matrix
float **allocate_kernel(int kernelSize);

// Function to free a 2D kernel matrix
void free_kernel(float **kernel, int kernelSize);

// Predefined kernels
extern float **BOX_BLUR_KERNEL;
extern float **GAUSSIAN_BLUR_KERNEL;
extern float **OUTLINE_KERNEL;
extern float **EMBOSS_KERNEL;
extern float **SHARPEN_KERNEL;
extern const int KERNEL_SIZE_3x3;

// Function to initialize global kernels
void initialize_kernels();

// Function to free global kernels
void cleanup_kernels();

// Clamp a value between min and max
int clamp_int(int value, int min_val, int max_val);
float clamp_float(float value, float min_val, float max_val);


#endif // UTILS_H
