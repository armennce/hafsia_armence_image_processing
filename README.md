# 🖼️ Image Processing in C 

## 🧾 Project Description

This project aims to implement, in C language, a small image processing software capable of loading, modifying, and saving BMP images in both 8-bit grayscale and 24-bit color formats.

The main goals are to:

- Manipulate complex memory structures (matrices, nested structs),
- Understand the BMP format and its headers,
- Implement **basic image operations** (negative, brightness, thresholding),
- Apply **convolution filters** (blur, edge detection, sharpening),
- Provide a **command-line interface** with a user-friendly menu.

The project is divided into several progressive parts:
1. **Part 1**: Grayscale image processing (8 bits),
2. **Part 2**: Color image processing (24 bits),
3. **Part 3**: Advanced features (to be announced).

The user can load an image, apply one or more filters, and save the result in a new BMP file — all through a simple command-line menu interface.
We will carry out the tests with the following images: 

![flowers_color](https://github.com/user-attachments/assets/25b4a468-13dc-4c17-bc46-3d82e91612bc)
![barbara_gray](https://github.com/user-attachments/assets/22bb606c-e6e5-4dae-b5b2-0562b963a252)


---

## 📁 Project Structure

- `bmp8.c/h` – Functions for 8-bit grayscale image processing  
- `bmp24.c/h` – Functions for 24-bit color image processing  
- `filters.c/h` – Convolution filter implementations  
- `utils.c/h` – Utility functions (file reading, writing, allocations)  
- `main.c` – Command-line interface and menu  
- `images/` – Sample BMP images for testing  

.
├── Makefile
├── main.c
├── bmp8.h
├── bmp8.c
├── bmp24.h
├── bmp24.c
├── histogram.h
├── histogram.c
├── utils.h
├── utils.c
└── README.md (brief instructions)
---

## ✅ Main Features

### 🩶 Part 1 – Grayscale Images (8-bit)

- Load / save BMP images (`bmp8_loadImage`, `bmp8_saveImage`)
- Display image information (`bmp8_printInfo`)
- Apply filters:
  - Negative
  - Brightness adjustment
  - Thresholding (black & white)
  - Convolution filters (blur, sharpen, etc.)

### 🌈 Part 2 – Color Images (24-bit)

- Pixel structure with RGB channels (`t_pixel`)
- Load / save BMP color images (`bmp24_loadImage`, `bmp24_saveImage`)
- Apply filters:
  - Negative (per channel)
  - Grayscale conversion
  - Brightness adjustment
  - Convolution filters (applied to RGB)

---

## 🧪 Compilation

```bash
gcc -o image_processing main.c bmp8.c bmp24.c filters.c utils.c -lm
