#include <spdlog/spdlog.h>
#include "file_reader.hpp"

#ifndef UNIT_TESTS  // Add this guard
int main(const int argc, char* argv[]) {
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
}
#endif  // UNIT_TESTS