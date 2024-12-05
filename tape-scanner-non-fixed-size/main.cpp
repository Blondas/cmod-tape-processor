#include <vector>
#include <string>
#include <string_view>
#include <cstring>
#include <sstream>
#include <iostream>
#include "tape_scanner.cpp"
#include <fstream>

int main(const int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <tape-location> <collection-name>\n";
        std::cerr << "Example: " << argv[0] << " resources/TAPE1.ebcdic ZIA6.DATA\n";
        return 1;
    }

    const std::string tape_location = argv[1];
    const std::string collection_name = arggit restv[2];

    try {
        MmapFileReader reader(tape_location);
        auto segments = TapeScanner::scanTape(reader, collection_name);

        // Create output file
        std::ofstream file_output("output.csv");
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