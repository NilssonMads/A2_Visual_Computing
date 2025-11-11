# A2 Visual Computing — Real-Time Video Processing
By Mads M. Nilsson

This project implements a real-time video processing application using **OpenCV**, **OpenGL**, and **GLFW**.  
It supports both **CPU** and **GPU (shader)**-based filters, live webcam input, and interactive geometric transformations.

---

## Features

- Real-time webcam feed using OpenCV  
- GPU and CPU processing modes  
- Implemented filters:
  - No Filter
  - Pixelation
  - Grayscale
- Interactive controls:
  - `1`, `2`, `3` – Switch filters
  - `C` – Toggle CPU/GPU mode
  - `Mouse Drag` – Translate image
  - `R + Drag` – Rotate image
  - `Scroll` – Scale image
  - `Space` – Reset transformations
  - `ESC` – Exit the application
- Automatic FPS tracking for performance analysis
---
## how to compile
### run in terminal 
cd .\build\

cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake

cmake --build . --config Release

.\videoprocessing.exe