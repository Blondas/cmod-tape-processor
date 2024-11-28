#include <file_reader.hpp>
#include <spdlog/spdlog.h>
#include <fstream>
#include <filesystem>

std::vector<char> readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        spdlog::error("Failed to open file: {}", filename);
        throw std::runtime_error("Cannot open file: " + filename);
    }
    
    // Get file size
    const auto size = std::filesystem::file_size(filename);
    
    // Reserve space and read
    std::vector<char> data(size);
    file.read(data.data(), size);
    
    spdlog::info("File read: {}, size: {} bytes", filename, data.size());
    return data;
}