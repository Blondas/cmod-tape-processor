
# Tape slicer

## Description
Processes a virtual tape file and extracts objects into a specified output directory.

## Prerequisites
- GCC

## Compilation

### Using GCC
```sh
gcc -o program slicer.c
```

## Running the Script
```sh
./program vTapeFile outputDirectory logFile
```

## Example
```sh
./program TAPE1.ebcdic output logs.txt
```