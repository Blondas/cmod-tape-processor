#include <vector>
#include <string>
#include <string_view>
#include <cstring>
#include <sstream>
#include <iostream>
#include "tape_scanner.cpp"
#include <fstream>
#include <chrono>
#include <format>

int main(const int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <tape-location> <collection-name>\n";
        std::cerr << "Example: " << argv[0] << " resources/TAPE1.ebcdic ZIA6.DATA\n";
        return 1;
    }

    const std::string tape_location = argv[1];
    const std::string collection_name = argv[2];

    try {
        MmapFileReader reader(tape_location);
        auto segments = TapeScanner::scanTape(reader, collection_name);

        // Create output directory with datetime and file
        auto now = std::chrono::system_clock::now();
        auto datetime_str = std::format("{:%Y-%m-%d_%H-%M-%S}", now);
        std::filesystem::path output_dir = std::filesystem::path("output") / datetime_str;
        std::filesystem::create_directories(output_dir);

        std::ofstream file_output(output_dir / "output.csv");
        if (!file_output) {
            throw std::runtime_error("Cannot create output.csv file");
        }

        // Write to both console and file
        TapeScanner::writeCsv(segments, std::cout);  // Console output
        TapeScanner::writeCsv(segments, file_output); // File output

        std::cout << "\nResults have been saved to output.csv\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
}