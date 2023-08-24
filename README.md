# ObjTexture Optimizer

ObjTexture Optimizer is a tool designed to streamline the management of .obj 3D model files. It processes these files and consolidates associated images into a spritesheet, updating the vertex texture indices.

## Features

- **OBJ File Enhancement**: Provides efficient reading and processing of `.obj` files.

- **Image Verification**: Checks for the availability of associated images.

- **Texture Index Re-mapping**: Generates new vertex texture indices for the consolidated spritesheet.

- **MTL File Integration**: Combines images with analogous properties into an integrated spritesheet and outputs an optimized `.mtl` file.

## Prerequisites

Ensure your system has:

1. **C++ Compiler**: `g++` or similar. Windows users might consider [MinGW](http://www.mingw.org/).

2. **Shell Environment**: Scripts are compatible with Linux/Mac. Windows users should consider [Git Bash](https://gitforwindows.org/).

3. **LibJPEG and LibPNG Libraries**: For image processing.
   - Debian/Ubuntu: `sudo apt-get install libjpeg-dev libpng-dev`
   - Red Hat/Fedora: `sudo dnf install libjpeg-turbo-devel libpng-devel`

## Usage

1. Ensure your `.obj` files and their contents are within the designated directory. 

2. Start the optimization: `./run_myProject.sh`

3. Upon completion, find the optimized files as `New_OrginalFileName.obj`, `New_OrginalFileName.mtl`, and `spritesheet_#.png` in the directory.

## File Structure

### Source Files

- `main.cpp`: Core of ObjTexture Optimizer.
- `check_image.cpp`: Handles image validation, texture index remapping, and `.obj` file output.
- `read_mtl.cpp`: Processes `.mtl` files and outputs combined spritesheets.

### Header Files

- `check_image.h`: Complements `check_image.cpp`.
- `read_mtl.h`: Supports `read_mtl.cpp` with classes like `Coordinate`, `Face`, and more.

### Scripts

- `run_myProject.sh`: Compiles sources to an executable and runs it on `.obj` files in the directory.
