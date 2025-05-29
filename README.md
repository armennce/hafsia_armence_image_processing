# 🖼️ Image Processing in C

The aim of this project is to implement a little software to process BMP images in C, covering both **8-bit grayscale** and **24-bit color** formats. It allows you to load
images, apply filters, equalize histograms, and save the results. 

We will work with `barbara.bmp` for 8-bit grayscale and `flowers_color.bmp` for 24-bit color format →

![barbara_gray](https://github.com/user-attachments/assets/ac2bfa93-35a8-4d7c-996d-bf38e8596faf)
![flowers_color](https://github.com/user-attachments/assets/1be121ec-d444-4869-a62c-27eba8c9631c)

---

### 🩶 Part 1 – Grayscale Images (8-bit)

- Load / save BMP images  
  → `bmp8_loadImage`, `bmp8_saveImage`
- Display image information  
  → `bmp8_printInfo`
- Apply filters:
  - Negative  
    → `bmp8_negative`
  - Brightness adjustment  
    → `bmp8_brightness`
  - Thresholding (black & white)  
    → `bmp8_threshold`
  - Convolution filters:
    - Box Blur → `bmp8_boxBlur`
    - Gaussian Blur → `bmp8_gaussianBlur`
    - Outline → `bmp8_outline`
    - Emboss → `bmp8_emboss`
    - Sharpen → `bmp8_sharpen`

---

### 🌈 Part 2 – Color Images (24-bit)

- RGB pixel structure (`t_pixel`)
- Load / save BMP color images  
  → `bmp24_loadImage`, `bmp24_saveImage`
- Apply filters:
  - Negative (per channel)  
    → `bmp24_negative`
  - Grayscale conversion  
    → `bmp24_grayscale`
  - Brightness adjustment  
    → `bmp24_brightness`
  - Convolution filters (on RGB):
    - Box Blur → `bmp24_boxBlur`
    - Gaussian Blur → `bmp24_gaussianBlur`
    - Outline → `bmp24_outline`
    - Emboss → `bmp24_emboss`
    - Sharpen → `bmp24_sharpen`

---

### 📊 Part 3 – Histogram Equalization

- **For 8-bit Grayscale Images**:
  - Compute histogram → `bmp8_computeHistogram`
  - Compute CDF → `bmp8_computeCDF`
  - Apply equalization → `bmp8_equalize`
  - Enhances contrast by redistributing intensity values

- **For 24-bit Color Images**:
  - Convert to YUV color space
  - Equalize the luminance channel (Y)
  - Convert back to RGB
  - Apply equalization → `bmp24_equalize`

---

### 🖼️ Visual Results – Filter Examples

#### Grayscale (Barbara)

![out_barbara_brightness](https://github.com/user-attachments/assets/12b855ba-3177-42c3-902a-a37fdf44f1e9) 
brightness
![out_barbara_equalized](https://github.com/user-attachments/assets/8bf8089c-7846-4c31-a288-e98eed8a98a3) 
equalized

---

#### Color (Flowers)
![out_flowers_grayscale](https://github.com/user-attachments/assets/76ee7bb6-ae51-475c-8776-2ecf8edafff9) 
grayscale
![out_flowers_negative](https://github.com/user-attachments/assets/332eae51-ba7b-40a0-b21e-f3dfcea6940a) 
negative

---

#### Structure of the project 

image-processing/
├── main.c                  
├── bmp8.c / bmp8.h         
├── bmp24.c / bmp24.h       
├── histogram.c / histogram.h 
├── utils.c / utils.h       
├── images/                 
│   ├── barbara_original.bmp
│   ├── flowers_original.bmp
├── README.md               
└── Makefile                


### 🛠️ How to Build & Run

Compile the project using:

```bash
cc main.c bmp8.c bmp24.c histogram.c utils.c -o main
