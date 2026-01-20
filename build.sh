#!/bin/bash

# Build script for PETSCII digital clock on Commodore 64 using cc65

# Check if cc65 is in the expected location
if [ ! -d "../cc65" ]; then
    echo "Error: cc65 directory not found at ../cc65"
    exit 1
fi

# Add cc65 to PATH
export PATH="../cc65/bin:$PATH"

# Compile the program
cl65 -t c64 -o clock.prg clock.c

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "Build successful: clock.prg created"
else
    echo "Build failed"
    exit 1
fi
