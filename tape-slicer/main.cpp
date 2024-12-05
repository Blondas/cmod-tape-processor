#include <vector>
#include <string>
#include <string_view>
#include <cstring>
#include <sstream>
#include <iostream>
#include "tape_slicer.cpp"
#include <fstream>
#include <chrono>
#include <format>
#include <unordered_set>

int main(const int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <tape-location> <collection-name>\n";
        std::cerr << "Example: " << argv[0] << " resources/TAPE1.ebcdic ZIA6.DATA\n";
        return 1;
    }

    const std::string tape_location = argv[1];
    const std::string collection_name = argv[2];

    try {
        // 1. Create output directory with datetime and file
        auto now = std::chrono::system_clock::now();
        auto datetime_str = std::format("{:%Y-%m-%d_%H-%M-%S}", now);
        std::filesystem::path output_dir = std::filesystem::path("output") / datetime_str;
        std::filesystem::create_directories(output_dir);

        // 2. Create a memory-mapped reader for your tape file
        MmapFileReader reader(tape_location);

        // 3. Scan the tape for segments with a specific collection name
        auto segments = TapeSlicer::scanTape(reader, collection_name);

        std::cout << "Found " << segments.size() << " segments\n";

        std::unordered_set<std::string> unique_files;
        for (const auto& segment : segments) {
            unique_files.insert(TapeSlicer::trimSpaces(segment.file_name));
        }
        std::cout << "Containing " << unique_files.size() << " unique files\n";

        // 4. Save the concatenated segments to an output directory
        bool success = TapeSlicer::saveSegments(reader, segments, output_dir);

        if (!success) {
            std::cerr << "Failed to save segments data to files\n";
            return 1;
        } else {
          std::cout << "Successfully saved segments data to files\n";
        }


        // 5. Save the segments metadata
        std::ofstream file_output(output_dir / "segments.csv");

        TapeSlicer::writeSegmentsCsv(segments, file_output);

        std::cout << "Segments metadata saved to segments.csv\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
}