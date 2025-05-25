//
// Created by Maxence on 25/05/2025.
//
#include "bmp8.h"

// Helper to extract metadata from header
void bmp8_extract_metadata(t_bmp8 *img) {
    // Width: offset 18 (0x12), 4 bytes
    memcpy(&img->width, img->header + BITMAP_WIDTH, sizeof(img->width));
    // Height: offset 22 (0x16), 4 bytes
    memcpy(&img->height, img->header + BITMAP_HEIGHT, sizeof(img->height));
    // Color Depth: offset 28 (0x1C), 2 bytes
    uint16_t depth_16bit; // BMP stores depth as 2 bytes
    memcpy(&depth_16bit, img->header + BITMAP_DEPTH, sizeof(depth_16bit));
    img->colorDepth = depth_16bit;
    // Data Size: offset 34 (0x22), 4 bytes
    memcpy(&img->dataSize, img->header + BITMAP_IMG_SIZE_RAW, sizeof(img->dataSize));

    // If dataSize is 0 (common for uncompressed), calculate it
    if (img->dataSize == 0 && img->colorDepth == 8) {
        img->dataSize = img->width * img->height; // 1 byte per pixel for 8-bit
    }
}

t_bmp8* bmp8_loadImage(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Error opening file");
        return NULL;
    }

    t_bmp8 *img = (t_bmp8 *)malloc(sizeof(t_bmp8));
    fread(img->header, sizeof(unsigned char), 54, file);
    fread(img->colorTable, sizeof(unsigned char), 1024, file);

    img->width = *(unsigned int *)&img->header[18];
    img->height = *(unsigned int *)&img->header[22];
    img->colorDepth = *(unsigned int *)&img->header[28];
    img->dataSize = img->width * img->height;

    if (img->colorDepth != 8) {
        fprintf(stderr, "Image is not 8-bit\n");
        free(img);
        fclose(file);
        return NULL;
    }

    img->data = (unsigned char *)malloc(img->dataSize);
    fread(img->data, sizeof(unsigned char), img->dataSize, file);
    fclose(file);
    return img;
}

void bmp8_saveImage(const char *filename, t_bmp8 *img) {
    if (!img) {
        fprintf(stderr, "Error: Image pointer is NULL in bmp8_saveImage.\n");
        return;
    }

    FILE *file = fopen(filename, "wb"); // Write binary
    if (!file) {
        perror("Error opening file for writing");
        return;
    }

    // Write header
    if (fwrite(img->header, 1, 54, file) != 54) {
        perror("Error writing BMP header");
        fclose(file);
        return;
    }

    // Write color table
    if (fwrite(img->colorTable, 1, 1024, file) != 1024) {
        perror("Error writing color table");
        fclose(file);
        return;
    }

    // Pixel data offset from header
    uint32_t pixelDataOffset;
    memcpy(&pixelDataOffset, img->header + BITMAP_OFFSET, sizeof(pixelDataOffset));

    // Ensure file pointer is at the correct position for pixel data
    // This is important if the offset isn't exactly 54 + 1024
    fseek(file, pixelDataOffset, SEEK_SET);

    // Write pixel data
    if (fwrite(img->data, 1, img->dataSize, file) != img->dataSize) {
        perror("Error writing pixel data");
        fclose(file);
        return;
    }

    fclose(file);
    printf("Image saved successfully as %s.\n", filename);
}

void bmp8_free(t_bmp8 *img) {
    if (img) {
        if (img->data) {
            free(img->data);
            img->data = NULL;
        }
        free(img);
        img = NULL;
    }
}

void bmp8_printInfo(t_bmp8 *img) {
    if (!img) {
        printf("Image Info: NULL image\n");
        return;
    }
    printf("Image Info:\n");
    printf("  Width: %u\n", img->width);
    printf("  Height: %u\n", img->height);
    printf("  Color Depth: %u\n", img->colorDepth);
    printf("  Data Size: %u bytes\n", img->dataSize);
}

// --- Image Processing Functions ---

void bmp8_negative(t_bmp8 *img) {
    if (!img || !img->data) return;
    for (unsigned int i = 0; i < img->dataSize; i++) {
        img->data[i] = 255 - img->data[i];
    }
    printf("Negative filter applied.\n");
}

void bmp8_brightness(t_bmp8 *img, int value) {
    if (!img || !img->data) return;
    for (unsigned int i = 0; i < img->dataSize; i++) {
        int new_val = (int)img->data[i] + value;
        img->data[i] = (unsigned char)clamp_int(new_val, 0, 255);
    }
    printf("Brightness filter applied (value: %d).\n", value);
}

void bmp8_threshold(t_bmp8 *img, int threshold_val) {
    if (!img || !img->data) return;
    threshold_val = clamp_int(threshold_val, 0, 255); // Ensure threshold is valid
    for (unsigned int i = 0; i < img->dataSize; i++) {
        img->data[i] = (img->data[i] >= threshold_val) ? 255 : 0;
    }
    printf("Threshold filter applied (threshold: %d).\n", threshold_val);
}

void bmp8_applyFilter(t_bmp8 *img, float **kernel, int kernelSize) {
    if (!img || !img->data || !kernel || kernelSize <= 0 || kernelSize % 2 == 0) {
        fprintf(stderr, "Error: Invalid arguments for bmp8_applyFilter.\n");
        return;
    }

    unsigned int width = img->width;
    unsigned int height = img->height;
    unsigned char *original_data = (unsigned char *)malloc(img->dataSize);
    if (!original_data) {
        perror("Error allocating memory for original data in applyFilter");
        return;
    }
    memcpy(original_data, img->data, img->dataSize);

    int n = kernelSize / 2; // e.g., for 3x3 kernel, n = 1

    // Iterate through pixels, excluding borders (as per PDF)
    for (unsigned int y = (unsigned int)n; y < height - n; y++) {
        for (unsigned int x = (unsigned int)n; x < width - n; x++) {
            float sum = 0.0f;
            // Apply kernel
            // The PDF's example: I'_xy = I_{x+1,y+1}*K_{-1,-1} + ...
            // This means kernel[0][0] maps to relative (-n, -n) from center pixel
            // And original image I_{x_img, y_img}
            // Kernel indices k_row, k_col range from 0 to kernelSize-1
            // Image indices relative to center: (k_col - n), (k_row - n)
            for (int ky = 0; ky < kernelSize; ky++) { // kernel row
                for (int kx = 0; kx < kernelSize; kx++) { // kernel col
                    // PDF example: I_xy' = sum sum I_{x-i, y-j} K_{i,j} where i,j go from -n to n
                    // To match this, if kernel indices are 0..kernelSize-1
                    // Kernel K[ky][kx]
                    // Image pixel at (x + (kx-n), y + (ky-n))
                    // unsigned int img_x = x + (kx - n);
                    // unsigned int img_y = y + (ky - n);

                    // PDF second example: I_xy = I_{x+1,y+1}*K_{-1,-1} + ... + I_{x-1,y-1}*K_{1,1}
                    // This means the kernel is effectively flipped for convolution.
                    // Or, we interpret K_{i,j} from PDF where i,j = -n..n
                    // Our kernel is indexed kernel[row_k][col_k] from 0..kernelSize-1
                    // Let's use the typical convolution way: flip kernel or adjust image indices
                    // Sum( I[x - kx_centered, y - ky_centered] * K[kx_centered, ky_centered] )
                    // where kx_centered, ky_centered are -n to n
                    // So, K[ky][kx] is K_{ky-n, kx-n} in PDF notation
                    // Image pixel is I_{x - (kx-n), y - (ky-n)}
                    unsigned int img_x = x - (kx - n);
                    unsigned int img_y = y - (ky - n);

                    // Boundary check (though we skip borders, this is for correctness if we didn't)
                    if (img_x < width && img_y < height) { // Ensure within bounds
                        sum += original_data[img_y * width + img_x] * kernel[ky][kx];
                    }
                }
            }
            img->data[y * width + x] = (unsigned char)clamp_int((int)roundf(sum), 0, 255);
        }
    }
    free(original_data);
    //printf("Convolution filter applied.\n"); // Generic, specific functions will print
}


void bmp8_boxBlur(t_bmp8 *img) {
    bmp8_applyFilter(img, BOX_BLUR_KERNEL, KERNEL_SIZE_3x3);
    printf("Box blur filter applied.\n");
}

void bmp8_gaussianBlur(t_bmp8 *img) {
    bmp8_applyFilter(img, GAUSSIAN_BLUR_KERNEL, KERNEL_SIZE_3x3);
    printf("Gaussian blur filter applied.\n");
}

void bmp8_outline(t_bmp8 *img) {
    bmp8_applyFilter(img, OUTLINE_KERNEL, KERNEL_SIZE_3x3);
    printf("Outline filter applied.\n");
}

void bmp8_emboss(t_bmp8 *img) {
    bmp8_applyFilter(img, EMBOSS_KERNEL, KERNEL_SIZE_3x3);
    printf("Emboss filter applied.\n");
}

void bmp8_sharpen(t_bmp8 *img) {
    bmp8_applyFilter(img, SHARPEN_KERNEL, KERNEL_SIZE_3x3);
    printf("Sharpen filter applied.\n");
}
