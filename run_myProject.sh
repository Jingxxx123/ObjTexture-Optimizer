#!/bin/bash

# Clear previous build artifacts
rm -f my_program

# Compile the program
g++ check_image.cpp read_mtl.cpp read_mtl.h -o myProject -lpng -ljpeg 

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation successful!"
    
    # Run the program
    myproject */*.obj #run all obj files
    #or myproject your_.obj_file_name

else
    echo "Compilation failed."
    exit 1
fi





