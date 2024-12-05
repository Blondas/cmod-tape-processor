#!/bin/bash

# Get the directory where the script is located
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# Change to the script's directory
cd "$SCRIPT_DIR" || exit

if g++ -std=c++20 \
  main.cpp \
  application.cpp \
  ebcdic_converter.cpp \
  header_t.cpp \
  mmap_file_reader.cpp \
  tape_scanner.cpp \
  -o program 2>&1 | tee compilation.log; then
  echo "Compilation successful! The executable 'program' has been created."
else
  echo "Compilation failed! Check compilation log for details."
fi