#!/bin/bash

# Check if program name was provided as an argument
if [ $# -ne 1 ]; then
    echo "Usage: $0 <program_name>"
    echo "Example: $0 myprogram"
    exit 1
fi

# Store the program name from the first argument
PROGRAM_NAME="cmod-tape-processor-$1"
OUTPUT_DIR="./output"


# Get the directory where the script is located
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# Change to the script's directory
cd "$SCRIPT_DIR" || exit

mkdir $OUTPUT_DIR

g++ -std=c++20 \
  -maix64 \
  main.cpp \
  ebcdic_converter.cpp \
  mmap_file_reader.cpp \
  tape_slicer.cpp \
  -o "$OUTPUT_DIR/$PROGRAM_NAME" 2>&1 | tee "$OUTPUT_DIR/$PROGRAM_NAME-compilation.log"

if [ "${PIPESTATUS[0]}" -eq 0 ]; then
  chmod +x "$OUTPUT_DIR/$PROGRAM_NAME"
  echo "Compilation successful! The executable 'program' has been created."
else
  echo "Compilation failed! Check compilation log for details."
fi