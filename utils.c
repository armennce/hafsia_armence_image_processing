#include "utils.h"

// Predefined kernels
float **BOX_BLUR_KERNEL = NULL;
float **GAUSSIAN_BLUR_KERNEL = NULL;
float **OUTLINE_KERNEL = NULL;
float **EMBOSS_KERNEL = NULL;
float **SHARPEN_KERNEL = NULL;
const int KERNEL_SIZE_3x3 = 3;

// Helper function for reading raw data from a file at a specific position
void file_rawRead(uint32_t position, void *buffer, uint32_t size, size_t n, FILE *file) {
    if (fseek(file, position, SEEK_SET) != 0) {
        perror("Error seeking in file_rawRead");
        // Consider more robust error handling, e.g., returning an error code
        return;
    }
    if (fread(buffer, size, n, file) != n) {
        perror("Error reading in file_rawRead");
        // Consider more robust error handling
        if(feof(file)) {
            fprintf(stderr, "End of file reached prematurely.\n");
        }
        if(ferror(file)) {
            fprintf(stderr, "File error occurred.\n");
        }
    }
}

// Helper function for writing raw data to a file at a specific position
void file_rawWrite(uint32_t position, void *buffer, uint32_t size, size_t n, FILE *file) {
    if (fseek(file, position, SEEK_SET) != 0) {
        perror("Error seeking in file_rawWrite");
        return;
    }
    if (fwrite(buffer, size, n, file) != n) {
        perror("Error writing in file_rawWrite");
    }
}

float **allocate_kernel(int kernelSize) {
    float **kernel = (float **)malloc(kernelSize * sizeof(float *));
    if (!kernel) return NULL;
    for (int i = 0; i < kernelSize; i++) {
        kernel[i] = (float *)malloc(kernelSize * sizeof(float));
        if (!kernel[i]) {
            // Free already allocated rows
            for (int j = 0; j < i; j++) free(kernel[j]);
            free(kernel);
            return NULL;
        }
    }
    return kernel;
}

void free_kernel(float **kernel, int kernelSize) {
    if (!kernel) return;
    for (int i = 0; i < kernelSize; i++) {
        free(kernel[i]);
    }
    free(kernel);
}

void initialize_kernels() {
    // Box Blur
    BOX_BLUR_KERNEL = allocate_kernel(KERNEL_SIZE_3x3);
    float val_box = 1.0f / 9.0f;
    for (int i = 0; i < KERNEL_SIZE_3x3; i++) for (int j = 0; j < KERNEL_SIZE_3x3; j++) BOX_BLUR_KERNEL[i][j] = val_box;

    // Gaussian Blur
    GAUSSIAN_BLUR_KERNEL = allocate_kernel(KERNEL_SIZE_3x3);
    GAUSSIAN_BLUR_KERNEL[0][0] = 1.0f/16.0f; GAUSSIAN_BLUR_KERNEL[0][1] = 2.0f/16.0f; GAUSSIAN_BLUR_KERNEL[0][2] = 1.0f/16.0f;
    GAUSSIAN_BLUR_KERNEL[1][0] = 2.0f/16.0f; GAUSSIAN_BLUR_KERNEL[1][1] = 4.0f/16.0f; GAUSSIAN_BLUR_KERNEL[1][2] = 2.0f/16.0f;
    GAUSSIAN_BLUR_KERNEL[2][0] = 1.0f/16.0f; GAUSSIAN_BLUR_KERNEL[2][1] = 2.0f/16.0f; GAUSSIAN_BLUR_KERNEL[2][2] = 1.0f/16.0f;

    // Outline
    OUTLINE_KERNEL = allocate_kernel(KERNEL_SIZE_3x3);
    float outline_vals[3][3] = {
            {-1, -1, -1},
            {-1,  8, -1},
            {-1, -1, -1}
    };
    for (int i = 0; i < KERNEL_SIZE_3x3; i++) for (int j = 0; j < KERNEL_SIZE_3x3; j++) OUTLINE_KERNEL[i][j] = outline_vals[i][j];

    // Emboss
    EMBOSS_KERNEL = allocate_kernel(KERNEL_SIZE_3x3);
    float emboss_vals[3][3] = {
            {-2, -1,  0},
            {-1,  1,  1},
            { 0,  1,  2}
    };
    for (int i = 0; i < KERNEL_SIZE_3x3; i++) for (int j = 0; j < KERNEL_SIZE_3x3; j++) EMBOSS_KERNEL[i][j] = emboss_vals[i][j];

    // Sharpen
    SHARPEN_KERNEL = allocate_kernel(KERNEL_SIZE_3x3);
    float sharpen_vals[3][3] = {
            { 0, -1,  0},
            {-1,  5, -1},
            { 0, -1,  0}
    };
    for (int i = 0; i < KERNEL_SIZE_3x3; i++) for (int j = 0; j < KERNEL_SIZE_3x3; j++) SHARPEN_KERNEL[i][j] = sharpen_vals[i][j];
}

void cleanup_kernels() {
    free_kernel(BOX_BLUR_KERNEL, KERNEL_SIZE_3x3);
    free_kernel(GAUSSIAN_BLUR_KERNEL, KERNEL_SIZE_3x3);
    free_kernel(OUTLINE_KERNEL, KERNEL_SIZE_3x3);
    free_kernel(EMBOSS_KERNEL, KERNEL_SIZE_3x3);
    free_kernel(SHARPEN_KERNEL, KERNEL_SIZE_3x3);
}

int clamp_int(int value, int min_val, int max_val) {
    if (value < min_val) return min_val;
    if (value > max_val) return max_val;
    return value;
}

float clamp_float(float value, float min_val, float max_val) {
    if (value < min_val) return min_val;
    if (value > max_val) return max_val;
    return value;
}
