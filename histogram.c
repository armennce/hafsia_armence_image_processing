#include "histogram.h"
#include <stdio.h>
// --- 8-bit Grayscale Histogram Equalization ---

unsigned int *bmp8_computeHistogram(t_bmp8 *img) {
    if (!img || !img->data) return NULL;

    unsigned int *hist = (unsigned int *)calloc(256, sizeof(unsigned int));
    if (!hist) {
        perror("Failed to allocate memory for histogram");
        return NULL;
    }

    for (unsigned int i = 0; i < img->dataSize; i++) {
        hist[img->data[i]]++;
    }
    return hist;
}

unsigned int *bmp8_computeAndNormalizeCDF(const unsigned int *hist, unsigned int N_pixels) {
    if (!hist || N_pixels == 0) return NULL;

    unsigned int *cdf = (unsigned int *)calloc(256, sizeof(unsigned int));
    unsigned int *hist_eq = (unsigned int *)calloc(256, sizeof(unsigned int));
    if (!cdf || !hist_eq) {
        perror("Failed to allocate memory for CDF/hist_eq");
        free(cdf); free(hist_eq);
        return NULL;
    }

    // Compute CDF
    cdf[0] = hist[0];
    for (int i = 1; i < 256; i++) {
        cdf[i] = cdf[i-1] + hist[i];
    }

    // Find cdf_min (smallest non-zero CDF value)
    // The PDF formula seems to use cdf_min from the cdf values directly.
    // Or, it could mean the first non-zero histogram bin's cdf value.
    // Let's find the cdf value of the first non-zero histogram bin.
    unsigned int cdf_min = 0;
    for (int i = 0; i < 256; i++) {
        if (hist[i] > 0) { // First non-zero intensity level
            cdf_min = cdf[i]; // The CDF up to and including this first level
            // The PDF says "smallest non-zero value of the cumulative histogram CDF"
            // This usually means the CDF value of the *lowest intensity level present in the image*.
            // If intensity 0 is present, cdf_min would be hist[0].
            // If intensities 0-k are not present, and k+1 is, cdf_min is cdf[k+1] (which is hist[k+1] if k= -1).
            // Example: if hist[0]=0, hist[1]=0, hist[2]=5, then cdf[0]=0, cdf[1]=0, cdf[2]=5. cdf_min should be 5.
            if (cdf[i] > 0) { // Find the first non-zero cdf value.
                cdf_min = cdf[i];
                break;
            }
        }
    }
    if (cdf_min == 0 && N_pixels > 0) { // Image is entirely one color or empty, handle this
        // If all pixels are the same color (e.g. all black), cdf_min might be N_pixels if only level 0.
        // If hist[0] = N_pixels, cdf[0]=N_pixels. cdf_min = N_pixels.
        // Then (N_pixels - N_pixels) / (N_pixels - N_pixels) is 0/0.
        // In this case, equalization does nothing. Map g to g.
        // The example in PDF has gray level 52 as min, cdf[52]=1, so cdf_min=1.
        // So, cdf_min is the cdf value of the *lowest intensity level actually present in the image*.
        // If hist[g_min] is the first non-zero count, cdf_min = cdf[g_min] = hist[g_min].
        // The document's example table: intensity 52, hist[52]=1, cdf[52]=1. So cdf_min = 1.
        // The numerator N - cdf_min implies total pixels minus count of pixels at lowest level.
        // If N_pixels == cdf_min, it means all pixels are of the cdf_min intensity level.
        // Denominator N - cdf_min
        // This should be total pixels (N) minus the CDF value of the lowest occurring intensity level,
        // *if that level is the only level present*. If multiple levels, N - cdf_min (where cdf_min is hist[lowest_intensity_present]).
        // The standard formula is (L-1) * (cdf[v] - cdf_min) / (N - cdf_min)
        // Here, L-1 is 255. N is total pixels.
        // cdf_min is the minimum value of the CDF (excluding entries for intensities not present).
        // More robustly, cdf_min is the number of pixels with the lowest intensity value present in the image.
        // Let's stick to "smallest non-zero value of CDF" for now.
        // The example has 154 as max gray level. CDF[154] = 64 (total pixels). N = 64.
        // cdf_min = 1 (for gray level 52).
        // For gray 52: round((cdf[52]-cdf_min)/(N-cdf_min) * 255) = round((1-1)/(64-1)*255) = round(0) = 0. Correct.
        // For gray 154: round((cdf[154]-cdf_min)/(N-cdf_min)*255) = round((64-1)/(64-1)*255) = round(255) = 255. Correct.
    }


    // Normalize CDF to create the equalization map
    for (int i = 0; i < 256; i++) {
        if (N_pixels == cdf_min) { // Avoid division by zero if all pixels are of the min intensity
            hist_eq[i] = i; // No change
        } else if (hist[i] == 0 && cdf[i] < cdf_min) { // Level not present and below min_cdf_level
            hist_eq[i] = 0; // Or map to itself: i
        }
        else {
            // The formula from PDF: hist_eq[i] = round( (cdf[i] - cdf_min) / (N - cdf_min) * 255 )
            // Ensure cdf[i] >= cdf_min for this formula to make sense,
            // or ensure hist[i] > 0 for this intensity level.
            if (cdf[i] < cdf_min && hist[i] > 0) { // Should not happen if cdf_min is correctly found
                // This case is problematic
            }
            double val = (double)(cdf[i] - cdf_min) / (N_pixels - cdf_min);
            hist_eq[i] = (unsigned int)roundf((float)val * 255.0f);
            hist_eq[i] = clamp_int(hist_eq[i], 0, 255);
        }
    }

    free(cdf);
    return hist_eq;
}

void bmp8_equalize(t_bmp8 *img) {
    if (!img || !img->data) return;

    unsigned int *hist = bmp8_computeHistogram(img);
    if (!hist) return;

    unsigned int N_pixels = img->dataSize; // For 8-bit, dataSize is number of pixels
    unsigned int *hist_eq_map = bmp8_computeAndNormalizeCDF(hist, N_pixels);
    if (!hist_eq_map) {
        free(hist);
        return;
    }

    // Apply the equalization map
    for (unsigned int i = 0; i < img->dataSize; i++) {
        img->data[i] = (unsigned char)hist_eq_map[img->data[i]];
    }

    free(hist);
    free(hist_eq_map);
    printf("8-bit Histogram equalization applied.\n");
}


// --- 24-bit Color Histogram Equalization ---

t_yuv_pixel rgb_to_yuv(t_rgb_pixel rgb) {
    t_yuv_pixel yuv;
    float r = rgb.red;
    float g = rgb.green;
    float b = rgb.blue;

    yuv.y = 0.299f * r + 0.587f * g + 0.114f * b;
    yuv.u = -0.14713f * r - 0.28886f * g + 0.436f * b;
    yuv.v = 0.615f * r - 0.51499f * g - 0.10001f * b;
    return yuv;
}

t_rgb_pixel yuv_to_rgb(t_yuv_pixel yuv) {
    t_rgb_pixel rgb;
    float y_val = yuv.y;
    float u_val = yuv.u;
    float v_val = yuv.v;

    float r = y_val + 1.13983f * v_val;
    float g = y_val - 0.39465f * u_val - 0.58060f * v_val;
    float b = y_val + 2.03211f * u_val;

    rgb.red = (uint8_t)clamp_int((int)roundf(r), 0, 255);
    rgb.green = (uint8_t)clamp_int((int)roundf(g), 0, 255);
    rgb.blue = (uint8_t)clamp_int((int)roundf(b), 0, 255);
    return rgb;
}


unsigned int *compute_y_channel_histogram(const uint8_t *y_channel_data, unsigned int num_pixels) {
    if (!y_channel_data) return NULL;
    unsigned int *hist = (unsigned int *)calloc(256, sizeof(unsigned int));
    if (!hist) {
        perror("Failed to allocate Y-channel histogram");
        return NULL;
    }
    for (unsigned int i = 0; i < num_pixels; i++) {
        hist[y_channel_data[i]]++;
    }
    return hist;
}


void bmp24_equalize(t_bmp24 *img) {
    if (!img || !img->data) return;

    int width = img->info.width;
    int height = abs(img->info.height);
    unsigned int num_pixels = width * height;

    // 1. Convert RGB to YUV and store Y channel (as uint8_t) and U,V (as float)
    t_yuv_pixel **yuv_image = (t_yuv_pixel **)malloc(height * sizeof(t_yuv_pixel *));
    uint8_t *y_channel_data_for_hist = (uint8_t *)malloc(num_pixels * sizeof(uint8_t));

    if (!yuv_image || !y_channel_data_for_hist) {
        perror("Failed to allocate memory for YUV conversion");
        free(yuv_image); // One might be non-NULL
        free(y_channel_data_for_hist);
        return;
    }
    for(int i=0; i<height; ++i) {
        yuv_image[i] = (t_yuv_pixel*)malloc(width * sizeof(t_yuv_pixel));
        if(!yuv_image[i]){
            perror("Failed to allocate memory for YUV row");
            for(int k=0; k<i; ++k) free(yuv_image[k]);
            free(yuv_image);
            free(y_channel_data_for_hist);
            return;
        }
    }


    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            yuv_image[y][x] = rgb_to_yuv(img->data[y][x]);
            // Clamp Y for histogram, but keep original float Y for reconstruction
            y_channel_data_for_hist[y * width + x] = (uint8_t)clamp_int((int)roundf(yuv_image[y][x].y), 0, 255);
        }
    }

    // 2. Calculate histogram of the Y component
    unsigned int *y_hist = compute_y_channel_histogram(y_channel_data_for_hist, num_pixels);
    if (!y_hist) {
        // Free YUV image data
        for(int i=0; i<height; ++i) free(yuv_image[i]);
        free(yuv_image);
        free(y_channel_data_for_hist);
        return;
    }

    // 3. Calculate cumulative histogram (CDF) and normalize for Y
    unsigned int *y_hist_eq_map = bmp8_computeAndNormalizeCDF(y_hist, num_pixels); // Re-use 8-bit CDF logic
    if (!y_hist_eq_map) {
        free(y_hist);
        for(int i=0; i<height; ++i) free(yuv_image[i]);
        free(yuv_image);
        free(y_channel_data_for_hist);
        return;
    }

    // 4. Apply histogram equalization to the Y component (using the original float Y values)
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            uint8_t original_y_clamped = (uint8_t)clamp_int((int)roundf(yuv_image[y][x].y), 0, 255);
            yuv_image[y][x].y = (float)y_hist_eq_map[original_y_clamped];
            // Ensure Y is still in a valid float range if needed, though map should keep it 0-255
        }
    }

    // 5. Convert Y'UV back to RGB
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            img->data[y][x] = yuv_to_rgb(yuv_image[y][x]);
        }
    }

    // Cleanup
    free(y_hist);
    free(y_hist_eq_map);
    for(int i=0; i<height; ++i) free(yuv_image[i]);
    free(yuv_image);
    free(y_channel_data_for_hist);

    printf("24-bit Color Histogram equalization (on Y channel) applied.\n");
}
