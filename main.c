#include <stdio.h>
#include <string.h>
#include "bmp8.h"
#include "bmp24.h"
#include "histogram.h"
#include "utils.h"

void display_main_menu() {
    printf("\n===== Image Processing Menu =====\n");
    printf("1. Load 8-bit Grayscale Image (Part 1)\n");
    printf("2. Load 24-bit Color Image (Part 2)\n");
    printf("0. Exit\n");
    printf("=================================\n");
    printf(">>> Your choice: ");
}

void display_part1_menu() {
    printf("\n--- 8-bit Grayscale Operations ---\n");
    printf("1. Save Image\n");
    printf("2. Display Image Info\n");
    printf("3. Apply Negative Filter\n");
    printf("4. Adjust Brightness\n");
    printf("5. Apply Threshold\n");
    printf("6. Apply Box Blur Filter\n");
    printf("7. Apply Gaussian Blur Filter\n");
    printf("8. Apply Outline Filter\n");
    printf("9. Apply Emboss Filter\n");
    printf("10. Apply Sharpen Filter\n");
    printf("11. Apply Histogram Equalization (Part 3)\n");
    printf("0. Back to Main Menu\n");
    printf("----------------------------------\n");
    printf(">>> Your choice: ");
}

void display_part2_menu() {
    printf("\n---- 24-bit Color Operations ----\n");
    printf("1. Save Image\n");
    printf("2. Display Image Info\n");
    printf("3. Apply Negative Filter\n");
    printf("4. Convert to Grayscale\n");
    printf("5. Adjust Brightness\n");
    printf("6. Apply Box Blur Filter\n");
    printf("7. Apply Gaussian Blur Filter\n");
    printf("8. Apply Outline Filter\n");
    printf("9. Apply Emboss Filter\n");
    printf("10. Apply Sharpen Filter\n");
    printf("11. Apply Histogram Equalization (Part 3)\n");
    printf("0. Back to Main Menu\n");
    printf("----------------------------------\n");
    printf(">>> Your choice: ");
}

void handle_part1(t_bmp8 *img8) {
    int choice;
    char filename[256];
    int val;

    if (!img8) return;

    do {
        display_part1_menu();

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            while (getchar() != '\n'); // Clear input buffer
            choice = -1; // Force rerun of the menu
            continue;
        }
        while (getchar() != '\n'); // Always clear buffer after reading

        switch (choice) {
            case 1:
                printf("Enter filename to save (e.g., output_gray.bmp): ");
                scanf("%255s", filename);
                while (getchar() != '\n'); // Clear buffer
                bmp8_saveImage(filename, img8);
                break;
            case 2:
                bmp8_printInfo(img8);
                break;
            case 3:
                bmp8_negative(img8);
                break;
            case 4:
                printf("Enter brightness adjustment value (-255 to 255): ");
                scanf("%255d", &val);
                while (getchar() != '\n');
                bmp8_brightness(img8, val);
                break;
            case 5:
                printf("Enter threshold value (0 to 255): ");
                scanf("%d", &val);
                while (getchar() != '\n');
                bmp8_threshold(img8, val);
                break;
            case 6: bmp8_boxBlur(img8); break;
            case 7: bmp8_gaussianBlur(img8); break;
            case 8: bmp8_outline(img8); break;
            case 9: bmp8_emboss(img8); break;
            case 10: bmp8_sharpen(img8); break;
            case 11: bmp8_equalize(img8); break;
            case 0:
                printf("Returning to main menu...\n");
                break;
            default:
                printf("Invalid choice. Try again.\n");
        }
    } while (choice != 0);
}

void handle_part2(t_bmp24 *img24) {
    int choice;
    char filename[256];
    int val;

    if (!img24) return;

    do {
        display_part2_menu();

        if (scanf("%255d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            while (getchar() != '\n');
            choice = -1;
            continue;
        }
        while (getchar() != '\n');

        switch (choice) {
            case 1:
                printf("Enter filename to save (e.g., output_color.bmp): ");
                scanf("%255s", filename);
                while (getchar() != '\n');
                bmp24_saveImage(filename, img24);
                break;
            case 2:
                bmp24_printInfo(img24);
                break;
            case 3:
                bmp24_negative(img24);
                break;
            case 4:
                bmp24_grayscale(img24);
                break;
            case 5:
                printf("Enter brightness adjustment value (-255 to 255): ");
                scanf("%255d", &val);
                while (getchar() != '\n');
                bmp24_brightness(img24, val);
                break;
            case 6: bmp24_boxBlur(img24); break;
            case 7: bmp24_gaussianBlur(img24); break;
            case 8: bmp24_outline(img24); break;
            case 9: bmp24_emboss(img24); break;
            case 10: bmp24_sharpen(img24); break;
            case 11: bmp24_equalize(img24); break;
            case 0:
                printf("Returning to main menu...\n");
                break;
            default:
                printf("Invalid choice. Try again.\n");
        }
    } while (choice != 0);
}


int main() {
    int choice;
    char filename[256];
    t_bmp8 *current_img8 = NULL;
    t_bmp24 *current_img24 = NULL;

    initialize_kernels(); // Initialize global filter kernels

    do {
        // Free any previously loaded image if we are at the main menu
        if (current_img8) { bmp8_free(current_img8); current_img8 = NULL; }
        if (current_img24) { bmp24_free(current_img24); current_img24 = NULL; }

        display_main_menu();
        if (scanf("%255d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            while(getchar() != '\n');
            choice = -1;
            continue;
        }
        while(getchar() != '\n');

        switch (choice) {
            case 1:
                printf("Enter 8-bit grayscale BMP filename (e.g., barbara_gray.bmp): ");
                scanf("%255s", filename);
                current_img8 = bmp8_loadImage(filename);
                if (current_img8) {
                    handle_part1(current_img8);
                } else {
                    printf("Failed to load 8-bit image.\n");
                }
                break;
            case 2:
                printf("Enter 24-bit color BMP filename (e.g., flowers_color.bmp): ");
                scanf("%255s", filename);
                current_img24 = bmp24_loadImage(filename);
                if (current_img24) {
                    handle_part2(current_img24);
                } else {
                    printf("Failed to load 24-bit image.\n");
                }
                break;
            case 0:
                printf("Exiting program.\n");
                break;
            default:
                printf("Invalid choice. Try again.\n");
        }
    } while (choice != 0);

    // Final cleanup
    if (current_img8) bmp8_free(current_img8);
    if (current_img24) bmp24_free(current_img24);
    cleanup_kernels(); // Free global filter kernels

    return 0;
}


