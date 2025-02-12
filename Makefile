CC=g++
CFLAGS=-std=c++20 -maix64

objects = slicer.c

slicer: $(objects)
	$(CC) $(CFAGS) $^ -o $@