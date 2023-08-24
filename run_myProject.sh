#!/bin/bash

# Clear previous build artifacts
rm -f myProject

# Compile the program
g++ check_image.cpp read_mtl.cpp read_mtl.h -o myProject -lpng -ljpeg 

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation successful!"
    
    # Run the program
    ./myProject */*.obj #run all obj files
    #or ./myProject your_.obj_file_name

else
    echo "Compilation failed."
    exit 1
fi





