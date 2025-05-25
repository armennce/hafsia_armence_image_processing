#include "bmp24.h"

// --- Allocation and Deallocation Functions ---
t_rgb_pixel **bmp24_allocateDataPixels(int width, int height) {
    if (width <= 0 || height <= 0) return NULL;
    t_rgb_pixel **pixels = (t_rgb_pixel **)malloc(height * sizeof(t_rgb_pixel *));
    if (!pixels) {
        perror("Failed to allocate rows for pixel data");
        return NULL;
    }
    for (int i = 0; i < height; i++) {
        pixels[i] = (t_rgb_pixel *)malloc(width * sizeof(t_rgb_pixel));
        if (!pixels[i]) {
            perror("Failed to allocate columns for pixel data");
            // Free already allocated rows
            for (int j = 0; j < i; j++) free(pixels[j]);
            free(pixels);
            return NULL;
        }
        // Initialize to black
        memset(pixels[i], 0, width * sizeof(t_rgb_pixel));
    }
    return pixels;
}

void bmp24_freeDataPixels(t_rgb_pixel **pixels, int height) {
    if (!pixels) return;
    for (int i = 0; i < height; i++) {
        free(pixels[i]);
    }
    free(pixels);
}

t_bmp24 *bmp24_allocate(int width, int height, int colorDepth) {
    t_bmp24 *img = (t_bmp24 *)malloc(sizeof(t_bmp24));
    if (!img) {
        perror("Failed to allocate t_bmp24 struct");
        return NULL;
    }
    memset(img, 0, sizeof(t_bmp24)); // Zero out the structure

    img->data = bmp24_allocateDataPixels(width, height);
    if (!img->data) {
        free(img);
        return NULL;
    }

    // Initialize header and info (sensible defaults for a new 24-bit image)
    img->header.type = BMP_TYPE_MAGIC_VALUE; // 'BM'
    img->header.reserved1 = 0;
    img->header.reserved2 = 0;
    img->header.offset = DEFAULT_HEADER_SIZE_VALUE + DEFAULT_INFO_SIZE_VALUE; // 14 + 40 = 54

    img->info.size = DEFAULT_INFO_SIZE_VALUE; // 40
    img->info.width = width;
    img->info.height = height;
    img->info.planes = 1;
    img->info.bits = (uint16_t)colorDepth;
    img->info.compression = 0; // BI_RGB (no compression)

    // Calculate row_padded_size and imagesize
    // Each row must be a multiple of 4 bytes.
    // The PDF says: "you do not need to manage padding: ensure that the width and height of the image are multiples of 4."
    // This simplifies things. If width is multiple of 4, (width * 3) might be multiple of 4.
    // If width is NOT multiple of 4, the PDF's constraint means we don't handle such images.
    // For this project, we assume width * 3 is the raw row size, and if padding is needed beyond that,
    // it means the source image width isn't a multiple of 4 for its pixel data.
    // The PDF implies we should ensure image width itself is multiple of 4 (for example) for simplicity.
    // Let's assume source images WILL have dimensions that don't require complex padding calculations.
    int row_raw_size = width * sizeof(t_rgb_pixel); // Bytes for one row of pixels (R,G,B)
    int row_padded_size = (row_raw_size + 3) & (~3); // Padded to multiple of 4 bytes
    img->info.imagesize = row_padded_size * abs(height); // abs(height) because height can be negative

    img->header.size = img->header.offset + img->info.imagesize;

    img->info.xresolution = 0; // Typically 2835 (72 DPI)
    img->info.yresolution = 0; // Typically 2835 (72 DPI)
    img->info.ncolors = 0;       // 0 for 24-bit
    img->info.importantcolors = 0; // 0 for 24-bit

    return img;
}

void bmp24_free(t_bmp24 *img) {
    if (img) {
        if (img->data) {
            bmp24_freeDataPixels(img->data, abs(img->info.height));
            img->data = NULL;
        }
        free(img);
    }
}

// --- Loading and Saving ---

void bmp24_readPixelData(t_bmp24 *image, FILE *file) {
    if (!image || !file || !image->data) return;

    int width = image->info.width;
    int height = abs(image->info.height); // Use absolute height for iteration
    int original_height_sign = (image->info.height > 0) ? 1 : -1; // To know if top-down or bottom-up

    // BMP stores rows from bottom to top if height is positive.
    // Top to bottom if height is negative.
    // We will always store in our `data` array from top to bottom (row 0 is top row).

    // Calculate padding for each row
    // Each row in file is padded to be a multiple of 4 bytes
    int row_byte_width_unpadded = width * sizeof(t_pixel); // Using t_pixel (BGR) for file reading
    int padding = (4 - (row_byte_width_unpadded % 4)) % 4;

    // Seek to the beginning of pixel data
    fseek(file, image->header.offset, SEEK_SET);

    for (int y_file = 0; y_file < height; y_file++) {
        // Determine which row in our memory buffer this corresponds to
        int y_mem = (original_height_sign > 0) ? (height - 1 - y_file) : y_file;

        for (int x = 0; x < width; x++) {
            t_pixel bgr_pixel; // Temporary to read BGR
            if (fread(&bgr_pixel, sizeof(t_pixel), 1, file) != 1) {
                perror("Error reading pixel data");
                return;
            }
            // Convert BGR to RGB for our t_rgb_pixel structure
            image->data[y_mem][x].red = bgr_pixel.red;     // PDF uses R,G,B order in t_pixel struct
            image->data[y_mem][x].green = bgr_pixel.green; // but BMP file is B,G,R
            image->data[y_mem][x].blue = bgr_pixel.blue;   // Re-check PDF section 2.2.2
            // PDF section 2.2.2: t_pixel has red, green, blue
            // PDF section 2.4.2 remarks: "Pixels are stored in BGR order... reverse order of color channels"
            // So, when reading: file_blue -> our_blue, file_green -> our_green, file_red -> our_red
            // IF t_pixel in header is BGR:
            image->data[y_mem][x].red = bgr_pixel.red;     // file.blue  -> our.red
            image->data[y_mem][x].green = bgr_pixel.green; // file.green -> our.green
            image->data[y_mem][x].blue = bgr_pixel.blue;   // file.red   -> our.blue

            // Let's assume t_pixel defined above is BGR (blue, green, red in memory)
            // And t_rgb_pixel is R,G,B
            // So we map:
            // image->data[y_mem][x].red = bgr_pixel.red; // if bgr_pixel was (r,g,b)
            // image->data[y_mem][x].green = bgr_pixel.green;
            // image->data[y_mem][x].blue = bgr_pixel.blue;

            // Let's use the actual t_pixel from PDF for file reading (uint8_t blue, green, red)
            // And our internal t_rgb_pixel is (uint8_t red, green, blue)
            t_pixel file_px; // This is (blue, green, red) as it's read from file
            // rewind a bit
            fseek(file, -sizeof(t_pixel), SEEK_CUR);
            fread(&file_px, sizeof(t_pixel), 1, file);

            image->data[y_mem][x].red = file_px.red;
            image->data[y_mem][x].green = file_px.green;
            image->data[y_mem][x].blue = file_px.blue;

        }
        // Skip padding bytes
        if (padding > 0) {
            fseek(file, padding, SEEK_CUR);
        }
    }
}


void bmp24_writePixelData(t_bmp24 *image, FILE *file) {
    if (!image || !file || !image->data) return;

    int width = image->info.width;
    int height = abs(image->info.height);
    int original_height_sign = (image->info.height > 0) ? 1 : -1;

    // Calculate padding
    int row_byte_width_unpadded = width * sizeof(t_pixel); // Using t_pixel (BGR) for file writing
    int padding = (4 - (row_byte_width_unpadded % 4)) % 4;
    unsigned char pad_bytes[3] = {0,0,0};

    // Seek to the beginning of pixel data
    fseek(file, image->header.offset, SEEK_SET);

    for (int y_file = 0; y_file < height; y_file++) {
        int y_mem = (original_height_sign > 0) ? (height - 1 - y_file) : y_file;

        for (int x = 0; x < width; x++) {
            // Our t_rgb_pixel is (red, green, blue)
            // File needs BGR
            t_pixel file_px; // This should be (blue, green, red) for file
            file_px.blue  = image->data[y_mem][x].blue;
            file_px.green = image->data[y_mem][x].green;
            file_px.red   = image->data[y_mem][x].red;

            if (fwrite(&file_px, sizeof(t_pixel), 1, file) != 1) {
                perror("Error writing pixel data");
                return;
            }
        }
        // Write padding bytes
        if (padding > 0) {
            if (fwrite(pad_bytes, 1, padding, file) != (size_t)padding) {
                perror("Error writing padding");
                return;
            }
        }
    }
}


t_bmp24 *bmp24_loadImage(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Error opening file for reading");
        return NULL;
    }

    t_bmp_header bmpHeader;
    if (fread(&bmpHeader, sizeof(t_bmp_header), 1, file) != 1) {
        fprintf(stderr, "Error reading BMP file header from %s\n", filename);
        fclose(file);
        return NULL;
    }

    if (bmpHeader.type != BMP_TYPE_MAGIC_VALUE) { // 'BM'
        fprintf(stderr, "%s is not a valid BMP file (signature: 0x%X).\n", filename, bmpHeader.type);
        fclose(file);
        return NULL;
    }

    t_bmp_info bmpInfo;
    if (fread(&bmpInfo, sizeof(t_bmp_info), 1, file) != 1) {
        fprintf(stderr, "Error reading BMP info header from %s\n", filename);
        fclose(file);
        return NULL;
    }

    if (bmpInfo.bits != DEFAULT_DEPTH_24BIT) {
        fprintf(stderr, "%s is not a 24-bit image (depth: %d bits).\n", filename, bmpInfo.bits);
        fclose(file);
        return NULL;
    }
    if (bmpInfo.compression != 0) {
        fprintf(stderr, "%s uses compression, which is not supported.\n", filename);
        fclose(file);
        return NULL;
    }
    // PDF: "ensure that the width and height of the image are multiples of 4."
    // This is a strong assumption about input images for this project.
    // if (bmpInfo.width % 4 != 0 || abs(bmpInfo.height) % 4 != 0) {
    //    fprintf(stderr, "Warning: Image dimensions (%d x %d) are not multiples of 4. Padding behavior might be simplified.\n", bmpInfo.width, abs(bmpInfo.height));
    // }


    t_bmp24 *img = bmp24_allocate(bmpInfo.width, abs(bmpInfo.height), bmpInfo.bits);
    if (!img) {
        fclose(file);
        return NULL;
    }

    // Copy loaded headers to the image structure
    img->header = bmpHeader;
    img->info = bmpInfo;
    // Re-calculate image size for safety based on actual width/height/depth (already done in allocate)
    // The `imagesize` in bmpInfo can be 0 for uncompressed images, so recalculate.
    int row_raw_size = img->info.width * (img->info.bits / 8);
    int row_padded_size = (row_raw_size + 3) & (~3);
    img->info.imagesize = row_padded_size * abs(img->info.height);
    img->header.size = img->header.offset + img->info.imagesize;


    // Read pixel data
    bmp24_readPixelData(img, file);

    fclose(file);
    printf("24-bit image %s loaded successfully.\n", filename);
    return img;
}

void bmp24_saveImage(const char *filename, t_bmp24 *img) {
    if (!img) {
        fprintf(stderr, "Error: Image is NULL in bmp24_saveImage.\n");
        return;
    }

    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("Error opening file for writing");
        return;
    }

    // Write BMP file header
    if (fwrite(&img->header, sizeof(t_bmp_header), 1, file) != 1) {
        perror("Error writing BMP file header");
        fclose(file);
        return;
    }

    // Write BMP info header
    if (fwrite(&img->info, sizeof(t_bmp_info), 1, file) != 1) {
        perror("Error writing BMP info header");
        fclose(file);
        return;
    }

    // Write pixel data
    bmp24_writePixelData(img, file);

    fclose(file);
    printf("24-bit image saved successfully as %s.\n", filename);
}

void bmp24_printInfo(t_bmp24 *img) {
    if (!img) {
        printf("Image Info (24-bit): NULL image\n");
        return;
    }
    printf("Image Info (24-bit):\n");
    printf("  File Header:\n");
    printf("    Type: 0x%X (%c%c)\n", img->header.type, (img->header.type & 0xFF), (img->header.type >> 8));
    printf("    Size: %u bytes\n", img->header.size);
    printf("    Pixel Data Offset: %u\n", img->header.offset);
    printf("  Info Header (DIB):\n");
    printf("    Header Size: %u\n", img->info.size);
    printf("    Width: %d pixels\n", img->info.width);
    printf("    Height: %d pixels\n", img->info.height);
    printf("    Planes: %u\n", img->info.planes);
    printf("    Bits per Pixel: %u\n", img->info.bits);
    printf("    Compression: %u\n", img->info.compression);
    printf("    Image Size (raw data): %u bytes\n", img->info.imagesize);
    printf("    X Pixels/Meter: %d\n", img->info.xresolution);
    printf("    Y Pixels/Meter: %d\n", img->info.yresolution);
    printf("    Colors in Palette: %u\n", img->info.ncolors);
    printf("    Important Colors: %u\n", img->info.importantcolors);
}


// --- Image Processing Functions (24-bit) ---
void bmp24_negative(t_bmp24 *img) {
    if (!img || !img->data) return;
    int height = abs(img->info.height);
    int width = img->info.width;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            img->data[y][x].red = 255 - img->data[y][x].red;
            img->data[y][x].green = 255 - img->data[y][x].green;
            img->data[y][x].blue = 255 - img->data[y][x].blue;
        }
    }
    printf("24-bit Negative filter applied.\n");
}

void bmp24_grayscale(t_bmp24 *img) {
    if (!img || !img->data) return;
    int height = abs(img->info.height);
    int width = img->info.width;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Average method for grayscale
            uint8_t gray = (uint8_t)roundf(
                    (img->data[y][x].red + img->data[y][x].green + img->data[y][x].blue) / 3.0f
            );
            img->data[y][x].red = gray;
            img->data[y][x].green = gray;
            img->data[y][x].blue = gray;
        }
    }
    printf("24-bit Grayscale filter applied.\n");
}

void bmp24_brightness(t_bmp24 *img, int value) {
    if (!img || !img->data) return;
    int height = abs(img->info.height);
    int width = img->info.width;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            img->data[y][x].red = (uint8_t)clamp_int((int)img->data[y][x].red + value, 0, 255);
            img->data[y][x].green = (uint8_t)clamp_int((int)img->data[y][x].green + value, 0, 255);
            img->data[y][x].blue = (uint8_t)clamp_int((int)img->data[y][x].blue + value, 0, 255);
        }
    }
    printf("24-bit Brightness filter applied (value: %d).\n", value);
}

// Convolution for a single pixel
t_rgb_pixel bmp24_convolution_pixel(t_bmp24 *img, int x_center, int y_center, float **kernel, int kernelSize, t_rgb_pixel **temp_data) {
    t_rgb_pixel new_pixel = {0, 0, 0};
    float sum_r = 0.0f, sum_g = 0.0f, sum_b = 0.0f;
    int n = kernelSize / 2; // e.g., for 3x3 kernel, n=1

    for (int ky = 0; ky < kernelSize; ky++) { // kernel row
        for (int kx = 0; kx < kernelSize; kx++) { // kernel col
            // Image pixel corresponding to K[ky][kx] when K is centered
            // int img_x = x_center + (kx - n);
            // int img_y = y_center + (ky - n);

            // For convolution (kernel effectively flipped)
            // Image pixel is I_{x_center - (kx-n), y_center - (ky-n)}
            int img_x = x_center - (kx - n);
            int img_y = y_center - (ky - n);

            // Boundary check (already handled by iterating from n to width/height - n)
            // if (img_x >= 0 && img_x < img->info.width && img_y >= 0 && img_y < abs(img->info.height)) {
            sum_r += temp_data[img_y][img_x].red * kernel[ky][kx];
            sum_g += temp_data[img_y][img_x].green * kernel[ky][kx];
            sum_b += temp_data[img_y][img_x].blue * kernel[ky][kx];
            // }
        }
    }

    new_pixel.red = (uint8_t)clamp_int((int)roundf(sum_r), 0, 255);
    new_pixel.green = (uint8_t)clamp_int((int)roundf(sum_g), 0, 255);
    new_pixel.blue = (uint8_t)clamp_int((int)roundf(sum_b), 0, 255);
    return new_pixel;
}

// Generic function to apply convolution using the pixel-wise function
void bmp24_apply_convolution_filter(t_bmp24 *img, float **kernel, int kernelSize, const char* filterName) {
    if (!img || !img->data || !kernel || kernelSize <= 0 || kernelSize % 2 == 0) {
        fprintf(stderr, "Error: Invalid arguments for %s.\n", filterName);
        return;
    }

    int width = img->info.width;
    int height = abs(img->info.height);

    // Create a temporary copy of pixel data to read from
    t_rgb_pixel **temp_data = bmp24_allocateDataPixels(width, height);
    if (!temp_data) {
        perror("Error allocating temp data for convolution");
        return;
    }
    for(int y=0; y<height; ++y) {
        memcpy(temp_data[y], img->data[y], width * sizeof(t_rgb_pixel));
    }

    int n = kernelSize / 2;

    // Iterate through pixels, excluding borders
    for (int y = n; y < height - n; y++) {
        for (int x = n; x < width - n; x++) {
            img->data[y][x] = bmp24_convolution_pixel(img, x, y, kernel, kernelSize, temp_data);
        }
    }

    bmp24_freeDataPixels(temp_data, height);
    printf("24-bit %s filter applied.\n", filterName);
}


void bmp24_boxBlur(t_bmp24 *img) {
    bmp24_apply_convolution_filter(img, BOX_BLUR_KERNEL, KERNEL_SIZE_3x3, "Box Blur");
}

void bmp24_gaussianBlur(t_bmp24 *img) {
    bmp24_apply_convolution_filter(img, GAUSSIAN_BLUR_KERNEL, KERNEL_SIZE_3x3, "Gaussian Blur");
}

void bmp24_outline(t_bmp24 *img) {
    bmp24_apply_convolution_filter(img, OUTLINE_KERNEL, KERNEL_SIZE_3x3, "Outline");
}

void bmp24_emboss(t_bmp24 *img) {
    bmp24_apply_convolution_filter(img, EMBOSS_KERNEL, KERNEL_SIZE_3x3, "Emboss");
}

void bmp24_sharpen(t_bmp24 *img) {
    bmp24_apply_convolution_filter(img, SHARPEN_KERNEL, KERNEL_SIZE_3x3, "Sharpen");
}

