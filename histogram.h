#ifndef IMAGE_PROCESSING_1_HISTOGRAM_H
#define IMAGE_PROCESSING_1_HISTOGRAM_H

#endif //IMAGE_PROCESSING_1_HISTOGRAM_H
#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include "bmp8.h"  // For t_bmp8 and 8-bit operations
#include "bmp24.h" // For t_bmp24 and 24-bit operations
#include "utils.h"

// --- 8-bit Grayscale Histogram Equalization ---

// Computes the histogram of an 8-bit grayscale image.
// Returns an array of 256 integers. Caller must free.
unsigned int *bmp8_computeHistogram(t_bmp8 *img);

// Computes the cumulative distribution function (CDF) and normalizes it
// to get the equalized histogram mapping.
// hist: input histogram (array of 256 unsigned ints)
// N_pixels: total number of pixels in the image
// Returns an array of 256 unsigned ints (the mapping). Caller must free.
unsigned int *bmp8_computeAndNormalizeCDF(const unsigned int *hist, unsigned int N_pixels);

// Applies histogram equalization to an 8-bit grayscale image.
void bmp8_equalize(t_bmp8 *img);


// --- 24-bit Color Histogram Equalization ---

// Structure for YUV color space (floating point)
typedef struct {
    float y;
    float u;
    float v;
} t_yuv_pixel;

// Converts an RGB pixel to YUV
t_yuv_pixel rgb_to_yuv(t_rgb_pixel rgb);

// Converts a YUV pixel to RGB
t_rgb_pixel yuv_to_rgb(t_yuv_pixel yuv);

// Computes histogram for the Y component of a YUV image representation
// y_channel_data: 1D array of Y component values (scaled to 0-255 uint8_t)
// num_pixels: total number of pixels
// Returns an array of 256 integers. Caller must free.
unsigned int *compute_y_channel_histogram(const uint8_t *y_channel_data, unsigned int num_pixels);

// Applies histogram equalization to a 24-bit color image.
void bmp24_equalize(t_bmp24 *img);


#endif // HISTOGRAM_H
