#!/bin/bash

g++ -std=c++20 \
  -I/opt/homebrew/include \
  -L/opt/homebrew/lib \
  -lfmt \
  -lspdlog \
  main.cpp \
  application.cpp \
  ebcdic_converter.cpp \
  header_t.cpp \
  mmap_file_reader.cpp \
  tape_scanner.cpp \
  -o program

echo "Compilation successful! The executable 'program' has been created."