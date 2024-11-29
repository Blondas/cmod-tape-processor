#include <iostream>
#include <file_reader/file_reader.hpp>
#include <spdlog/spdlog.h>
#include <__filesystem/path.h>
#include <fstream>

#include "file_reader/mmap_file_reader.cpp"
#include "converter/ebcdic_converter.cpp"

class Application {
public:
    static int read_file(const int argc, char* argv[]) {
        if (argc != 2) {
            spdlog::error("Invalid arguments, filename needed");
            return 1;
        }

        try {
            auto fileData = readFile(argv[1]);
        } catch (const std::exception& e) {
            spdlog::critical("Program failed: {}", e.what());
            return 1;
        }
        return 0;
    }

    // read ebcdic tape > open with mmap > read first 128 bytes > translate to ASCII > print and save
    static void convertEbcdicFileToAscii(const std::string& filename, const size_t bytesToConvert) {
        const MmapFileReader reader(filename);

        const std::string_view ebcdicView(reader.data(), bytesToConvert);
        const std::string asciiData = EbcdicConverter::toAscii(ebcdicView);

        const auto outputPath = std::filesystem::path("resources") / "TAPE1.ebcdic_converted_to_ascii";
        std::cout << "Absolute path: " << std::filesystem::absolute(outputPath) << std::endl;
        std::ofstream outFile(outputPath, std::ios::binary);
        if (!outFile) {
            throw std::runtime_error("Failed to create output file: " + outputPath.string());
        }

        outFile.write(asciiData.data(), asciiData.size());
        std::cout << asciiData << std::endl;
    }
};
