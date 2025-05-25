//
// Created by Maxence on 25/05/2025.
//
#ifndef BMP8_H
#define BMP8_H

#include "utils.h" // For common utilities and standard headers

// Structure to represent an 8-bit BMP image
typedef struct {
    unsigned char header[54];       // BMP file header (54 bytes)
    unsigned char colorTable[1024]; // Color table (256 colors * 4 bytes/color)
    unsigned char *data;            // Pixel data (grayscale values)

    unsigned int width;             // Image width in pixels
    unsigned int height;            // Image height in pixels
    unsigned int colorDepth;        // Bits per pixel (should be 8)
    unsigned int dataSize;          // Size of pixel data in bytes
} t_bmp8;

// Function to load an 8-bit grayscale BMP image from a file
// Returns a pointer to t_bmp8 structure or NULL on error.
t_bmp8 *bmp8_loadImage(const char *filename);

// Function to save an 8-bit grayscale BMP image to a file
void bmp8_saveImage(const char *filename, t_bmp8 *img);

// Function to free the memory allocated for a t_bmp8 image
void bmp8_free(t_bmp8 *img);

// Function to print information about the t_bmp8 image
void bmp8_printInfo(t_bmp8 *img);

// --- Image Processing Functions ---

// Inverts the colors of the image (negative)
void bmp8_negative(t_bmp8 *img);

// Adjusts the brightness of the image
// value: positive to brighten, negative to darken
void bmp8_brightness(t_bmp8 *img, int value);

// Converts the image to black and white based on a threshold
// Pixels >= threshold become white (255), others black (0)
void bmp8_threshold(t_bmp8 *img, int threshold_val);

// Applies a generic convolution filter to the image
// kernel: a 2D array representing the filter kernel
// kernelSize: the dimension of the square kernel (e.g., 3 for a 3x3 kernel)
void bmp8_applyFilter(t_bmp8 *img, float **kernel, int kernelSize);

// Specific filter functions (will call bmp8_applyFilter)
void bmp8_boxBlur(t_bmp8 *img);
void bmp8_gaussianBlur(t_bmp8 *img);
void bmp8_outline(t_bmp8 *img);
void bmp8_emboss(t_bmp8 *img);
void bmp8_sharpen(t_bmp8 *img);


#endif // BMP8_H
