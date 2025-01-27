#pragma once
#include <string>
#include <stdexcept>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

class MmapFileReader {
public:
   MmapFileReader(const std::string& filename) {
        fd_ = ::open(filename.c_str(), O_RDONLY);
        if (fd_ == -1) {
            throw std::runtime_error("Cannot open file: " + filename);
        }

        // Get file size
        struct stat sb;
        if (fstat(fd_, &sb) == -1) {
            ::close(fd_);
            throw std::runtime_error("Cannot get file size");
        }
        size_ = sb.st_size;

        // Map the file
        data_ = mmap(nullptr, size_, PROT_READ, MAP_PRIVATE, fd_, 0);
        if (data_ == MAP_FAILED) {
            ::close(fd_);
            throw std::runtime_error("Cannot map file");
        }
    }

    ~MmapFileReader() {
        if (data_ != MAP_FAILED) {
            munmap(data_, size_);
        }
        if (fd_ != -1) {
            ::close(fd_);
        }
    }

    // No copying
    MmapFileReader(const MmapFileReader&) = delete;
    MmapFileReader& operator=(const MmapFileReader&) = delete;

    const char* data() const { return static_cast<const char*>(data_); }
    size_t size() const { return size_; }

private:
    void* data_ = MAP_FAILED;
    int fd_ = -1;
    size_t size_ = 0;
};
