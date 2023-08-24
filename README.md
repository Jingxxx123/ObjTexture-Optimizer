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

## File Structure

### Source Files
  
- `check_image.cpp`: Dedicated to image verification, texture index remapping, and outputting the updated `.obj` file.
  
- `read_mtl.cpp`: Specializes in parsing and refining `.mtl` files, culminating in an optimized `.mtl` file with consolidated .png spritesheets.

### Header Files

- `check_image.h`: Houses function declarations pertinent to `check_image.cpp`.
  
- `read_mtl.h`: Complements `read_mtl.cpp`, encapsulating various classes and methods including `Coordinate`, `Face`, `ImageInfo`, `Material`, `Group`, `Property`, and `Properties`.

### Scripts

- `run_myProject.sh`: A crafty script that undertakes the task of compiling all source files into the `myProject` executable. Following that, it runs this application on all `.obj` files located within the specified directory.

## Usage

1. Ensure all your `.obj` files along with their associated content are placed in folders within the specified directory.  

2. Start the optimization: `./run_myProject.sh`

3.  Once completed, the directory will be enriched with refined files named `New_OrginalFileName.obj`, `New_OrginalFileName.mtl`, and multiple `spritesheet_#.png` files, all of which encapsulate the enhanced texture data.



- `run_myProject.sh`: Compiles sources to an executable and runs it on `.obj` files in the directory.
