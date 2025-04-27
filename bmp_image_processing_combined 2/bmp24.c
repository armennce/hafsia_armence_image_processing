
#include <stdio.h>
#include <stdlib.h>
#include "bmp24.h"

t_pixel **bmp24_allocateDataPixels(int width, int height) {
    t_pixel **pixels = malloc(height * sizeof(t_pixel *));
    if (!pixels) return NULL;
    for (int i = 0; i < height; i++) {
        pixels[i] = malloc(width * sizeof(t_pixel));
        if (!pixels[i]) return NULL;
    }
    return pixels;
}

void bmp24_freeDataPixels(t_pixel **pixels, int height) {
    for (int i = 0; i < height; i++) {
        free(pixels[i]);
    }
    free(pixels);
}

t_bmp24 *bmp24_allocate(int width, int height, int colorDepth) {
    t_bmp24 *img = malloc(sizeof(t_bmp24));
    if (!img) return NULL;
    img->width = width;
    img->height = height;
    img->colorDepth = colorDepth;
    img->data = bmp24_allocateDataPixels(width, height);
    if (!img->data) {
        free(img);
        return NULL;
    }
    return img;
}

void bmp24_free(t_bmp24 *img) {
    if (img) {
        bmp24_freeDataPixels(img->data, img->height);
        free(img);
    }
}

t_bmp24 *bmp24_loadImage(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Error opening file");
        return NULL;
    }

    t_bmp24 *img = malloc(sizeof(t_bmp24));
    fread(&img->header, sizeof(t_bmp_header), 1, file);
    fread(&img->header_info, sizeof(t_bmp_info), 1, file);

    img->width = img->header_info.width;
    img->height = img->header_info.height;
    img->colorDepth = img->header_info.bits;

    if (img->colorDepth != 24) {
        fprintf(stderr, "Not a 24-bit BMP image.\n");
        fclose(file);
        free(img);
        return NULL;
    }

    img->data = bmp24_allocateDataPixels(img->width, img->height);
    fseek(file, img->header.offset, SEEK_SET);

    for (int i = img->height - 1; i >= 0; i--) {
        for (int j = 0; j < img->width; j++) {
            fread(&img->data[i][j], sizeof(t_pixel), 1, file);
        }
    }

    fclose(file);
    return img;
}

void bmp24_saveImage(t_bmp24 *img, const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("Error opening file");
        return;
    }

    fwrite(&img->header, sizeof(t_bmp_header), 1, file);
    fwrite(&img->header_info, sizeof(t_bmp_info), 1, file);

    for (int i = img->height - 1; i >= 0; i--) {
        for (int j = 0; j < img->width; j++) {
            fwrite(&img->data[i][j], sizeof(t_pixel), 1, file);
        }
    }

    fclose(file);
}

void bmp24_negative(t_bmp24 *img) {
    for (int i = 0; i < img->height; i++) {
        for (int j = 0; j < img->width; j++) {
            img->data[i][j].red = 255 - img->data[i][j].red;
            img->data[i][j].green = 255 - img->data[i][j].green;
            img->data[i][j].blue = 255 - img->data[i][j].blue;
        }
    }
}

void bmp24_grayscale(t_bmp24 *img) {
    for (int i = 0; i < img->height; i++) {
        for (int j = 0; j < img->width; j++) {
            uint8_t gray = (img->data[i][j].red + img->data[i][j].green + img->data[i][j].blue) / 3;
            img->data[i][j].red = gray;
            img->data[i][j].green = gray;
            img->data[i][j].blue = gray;
        }
    }
}

void bmp24_brightness(t_bmp24 *img, int value) {
    for (int i = 0; i < img->height; i++) {
        for (int j = 0; j < img->width; j++) {
            int r = img->data[i][j].red + value;
            int g = img->data[i][j].green + value;
            int b = img->data[i][j].blue + value;
            img->data[i][j].red = r > 255 ? 255 : (r < 0 ? 0 : r);
            img->data[i][j].green = g > 255 ? 255 : (g < 0 ? 0 : g);
            img->data[i][j].blue = b > 255 ? 255 : (b < 0 ? 0 : b);
        }
    }
}
