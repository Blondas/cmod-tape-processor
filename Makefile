CC=g++
CFLAGS=-std=c++20 -maix64

objects = main.cpp ebcdic_converter.cpp mmap_file_reader.cpp tape_slicer.cpp

tslicer: $(objects)
	$(CC) $(CFAGS) $^ -o $@