#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>

const static unsigned char ctrlTag[4] = {0x5C, 0xD6, 0xC1, 0xD4};
const static unsigned char e2a_map[256] = {
0x00, 0x01, 0x02, 0x03, 0x1A, 0x09, 0x1A, 0x7F, 0x1A, 0x1A, 0x1A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
0x10, 0x11, 0x12, 0x13, 0x1A, 0x1A, 0x08, 0x1A, 0x18, 0x19, 0x1A, 0x1A, 0x1C, 0x1D, 0x1E, 0x1F,
0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x0A, 0x17, 0x1B, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x05, 0x06, 0x07,
0x1A, 0x1A, 0x16, 0x1A, 0x1A, 0x1A, 0x1A, 0x04, 0x1A, 0x1A, 0x1A, 0x1A, 0x14, 0x15, 0x1A, 0x1A,
0x20, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x5B, 0x2E, 0x3C, 0x28, 0x2B, 0x21,
0x26, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x5D, 0x24, 0x2A, 0x29, 0x3B, 0x5E,
0x2D, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x7C, 0x2C, 0x25, 0x5F, 0x3E, 0x3F,
0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x60, 0x3A, 0x23, 0x40, 0x27, 0x3D, 0x22,
0x1A, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A,
0x1A, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A,
0x1A, 0x7E, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A,
0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A,
0x7B, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A,
0x7D, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A,
0x5C, 0x1A, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A,
0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A};

void e2a44b(const unsigned char *in, char *out, size_t s) {
	if (!in || !out) return;  // Safety check

	for (size_t i=0; i < s; i++) {
		out[i] = e2a_map[in[i]];
	}
	out[s] = '\0';
}

void trim44c(char *in, size_t s) {
	size_t i;
	for (i = s-1; in[i] == 0x20 && i > 0; i--);
	in[i+1] = '\0';
}

uint32_t bytes_to_uint32(const unsigned char *bytes) {
    return ((uint32_t)bytes[0] << 24) |
           ((uint32_t)bytes[1] << 16) |
           ((uint32_t)bytes[2] << 8)  |
           ((uint32_t)bytes[3]);
}

size_t write_object(const char *outputDirectory, const char *fileName, const unsigned char *buffer, size_t bytes, FILE *logFile) {
	if (!outputDirectory || !fileName || !buffer || bytes == 0) {
		return 0; // Invalid input parameters
	}

	char filePath[512];
	snprintf(filePath, sizeof(filePath), "%s/%s", outputDirectory, fileName);

	FILE *file = fopen(filePath, "ab"); // Open in append binary mode
	if (!file) {
		perror("Failed to open file");
		fprintf(logFile, "Failed to open file: %s\n", filePath);
		return 0;
	}

	size_t written = fwrite(buffer, 1, bytes, file);
	if (written != bytes) {
		perror("Failed to write to file");
		fprintf(logFile, "Failed to write to file: %s\n", filePath);
	}

	fclose(file);
	return written;
}

void create_directory_if_not_exists(const char *path) {
	struct stat st = {0};

	if (stat(path, &st) == -1) {
		mkdir(path, 0700);
	}
}

int main(int argc, char *argv[]) {

    int fd;
    struct stat sb;
    unsigned char *mapped, *current;
	char collectionName[45];
	char objectName[45];
	uint32_t blockSize;
	FILE *logFile;
	//unsigned char data[4] = {0x12, 0x34, 0x56, 0x78};

	if (argc != 4) {
		fprintf(stderr, "Usage: %s vTapeFile outputDirectory logFile\nObjects will be created in the specified directory.\n", argv[0]);
		exit(1);
	}

	const char *outputDirectory = argv[2];
	const char *logFilePath = argv[3];

	create_directory_if_not_exists(outputDirectory);

	// Open the log file
	logFile = fopen(logFilePath, "a");
	if (!logFile) {
		perror("ERROR: Unable to open log file");
		exit(EXIT_FAILURE);
	}

    // Open the file
	fd = open(argv[1], O_RDONLY);
	if (fd == -1) {
		perror("ERROR: Unable to open file");
		fprintf(logFile, "ERROR: Unable to open file\n");
		fclose(logFile);
		exit(EXIT_FAILURE);
	}

	// Get the file size
	if (fstat(fd, &sb) == -1) {
		perror("ERROR: getting file size");
		fprintf(logFile, "ERROR: getting file size\n");
		close(fd);
		fclose(logFile);
		exit(EXIT_FAILURE);
	}

    // Memory-map the file
    mapped = (unsigned char *)mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (mapped == MAP_FAILED) {
    	perror("ERROR: Unable to map file");
    	fprintf(logFile, "ERROR: Unable to map file\n");
    	close(fd);
    	fclose(logFile);
    	exit(EXIT_FAILURE);
    }
	current = mapped;

	while (mapped + sb.st_size - current > 128) {
		fprintf(stderr, "INFO: 0x%lx:", (unsigned long)(current - mapped));
		fprintf(logFile, "INFO: 0x%lx:", (unsigned long)(current - mapped));

		// parse collection name, otherwise not used
		e2a44b(current, collectionName, 44);
		trim44c(collectionName, 44);
		current += 44;

		// parse object name, used as output file name
		e2a44b(current, objectName, 44);
		trim44c(objectName, 44);
		current += 44;

		// skip 2x 32bit int
		current += 8;

		// parse block size as 32bit uint
		blockSize = bytes_to_uint32(current);
		current += 4;

		fprintf(stderr, "%s, %s, %u, ",collectionName, objectName, blockSize);
		fprintf(logFile, "%s, %s, %u, ", collectionName, objectName, blockSize);

		if (memcmp(current, ctrlTag, 4) != 0) {
			fprintf(stderr, "FAILED\nERROR: Out of sync!\n");
			fprintf(logFile, "FAILED\nERROR: Out of sync!\n");
			close(fd);
			fclose(logFile);
			exit(EXIT_FAILURE);
		}
		current += 28; // At the end of header

		current += write_object(outputDirectory, objectName, current, blockSize, logFile);
		fprintf(stderr, "OK\n");
		fprintf(logFile, "OK\n");
	}

    // Unmap memory
    if (munmap(mapped, sb.st_size) == -1) {
        perror("ERROR: While unmapping file");
    	fprintf(logFile, "ERROR: While unmapping file\n");
    }

    // Close the file
    close(fd);

    fprintf(stderr, "Done\n");
	fprintf(logFile, "Done\n");

    return 0;
}