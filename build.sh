#!/bin/bash

# Set the name of the output executable
OUTPUT="words_counter"

# List of source files
SOURCES=(
    "concurrent_file_reader.cpp"
    "main.cpp"
    "word_counter.cpp"
    "timer.cpp"
    "concurrent_file_word_stream.cpp"
    "word_stream.cpp"
)

# Directory for object files
OBJDIR="obj"

# Create OBJDIR if it doesn't exist
mkdir -p $OBJDIR

# Compile each source file to an object file
OBJECTS=()
for SOURCE in "${SOURCES[@]}"; do
    OBJFILE="${OBJDIR}/$(basename ${SOURCE%.*}.o)"
    g++ -c "$SOURCE" -o "$OBJFILE" -std=c++17 -O3
    if [ $? -ne 0 ]; then
        echo "Compilation of $SOURCE failed"
        exit 1
    fi
    OBJECTS+=("$OBJFILE")
done

# Link all object files into the final executable
g++ "${OBJECTS[@]}" -lpthread -O3 -o $OUTPUT
if [ $? -eq 0 ]; then
    echo "Build successful"
else
    echo "Build failed"
fi
