
#include <stdio.h>
#include "bmp8.h"
#include "bmp24.h"

int main() {
    // === PARTIE 1 : Image en niveaux de gris (8 bits) ===
    printf("PARTIE 1 : Traitement de l'image 8 bits (grayscale)\n");

    t_bmp8 *img8 = bmp8_loadImage("barbara_gray.bmp");
    if (!img8) {
        printf("Erreur de chargement de l'image 8 bits.\n");
    } else {
        bmp8_printInfo(img8);

        bmp8_negative(img8);
        bmp8_saveImage("barbara_negative.bmp", img8);

        bmp8_brightness(img8, 50);
        bmp8_saveImage("barbara_brightness.bmp", img8);

        bmp8_threshold(img8, 128);
        bmp8_saveImage("barbara_threshold.bmp", img8);

        bmp8_free(img8);
    }

    // === PARTIE 2 : Image couleur (24 bits) ===
    printf("\nPARTIE 2 : Traitement de l'image 24 bits (couleur)\n");

    t_bmp24 *img24 = bmp24_loadImage("flowers_color.bmp");
    if (!img24) {
        printf("Erreur de chargement de l'image 24 bits.\n");
    } else {
        bmp24_negative(img24);
        bmp24_saveImage(img24, "flowers_negative.bmp");

        bmp24_grayscale(img24);
        bmp24_saveImage(img24, "flowers_grayscale.bmp");

        bmp24_brightness(img24, 50);
        bmp24_saveImage(img24, "flowers_brightness.bmp");

        bmp24_free(img24);
    }

    return 0;
}
