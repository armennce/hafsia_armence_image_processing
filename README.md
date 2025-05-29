# 🖼️ Image Processing in C

The aim of this project is to implement a little software to process BMP images in C, covering both **8-bit grayscale** and **24-bit color** formats. It allows you to load<br>
images, apply filters, equalize histograms, and save the results.

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

![barbara_gray](https://github.com/user-attachments/assets/806d3f61-6a19-4295-b811-e3738fad7f58)
---

#### Color (Flowers)


---

### 🛠️ How to Build & Run

Compile the project using:

```bash
cc main.c bmp8.c bmp24.c histogram.c utils.c -o main
