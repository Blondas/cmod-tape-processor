#include <iostream>
#include <file_reader/file_reader.hpp>
#include <spdlog/spdlog.h>
#include <__filesystem/path.h>
#include <fstream>

#include "file_reader/mmap_file_reader.cpp"
#include "tape_scanner/tape_scanner.cpp"

class Application {
public:
    static void init() {
        // Set global log pattern
        spdlog::set_pattern("%Y-%m-%d %H:%M:%S.%e [%l] %v");
        // Set log level to debug to see all logs
        spdlog::set_level(spdlog::level::info);
    }

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

    static void scanTapeHeaders(const std::string& filename, const std::string& collection_name) {
        spdlog::info("Starting tape header scan for file: {}, collection: {}", filename, collection_name);

        try {
            spdlog::debug("Opening file with memory mapping");
            MmapFileReader reader(filename);
            spdlog::debug("File mapped successfully, size: {} bytes", reader.size());

            spdlog::debug("Starting header scan");
            auto headers = TapeScanner::scanTape(reader, collection_name);
            spdlog::info("Found {} headers in the tape", headers.size());

            // Create grouped JSON output
            nlohmann::json output = createGroupedJson(headers);

            // Print to console
            spdlog::debug("Converting headers to JSON");
            std::string json_output = output.dump(2);
            std::cout << json_output << std::endl;

            // Save to file
            std::string output_filename = filename + "_headers.json";
            spdlog::info("Saving results to: {}", output_filename);

            std::ofstream out_file(output_filename);
            if (!out_file) {
                throw std::runtime_error("Failed to create output file: " + output_filename);
            }

            out_file << json_output;
            spdlog::info("Scan completed successfully");

        } catch (const std::exception& e) {
            spdlog::error("Failed to scan tape headers: {}", e.what());
            throw;
        }
    }

private:
    static nlohmann::json createGroupedJson(const std::vector<std::tuple<size_t, header_t>>& headers) {
        spdlog::debug("Creating grouped JSON output");

        // Map to group headers by collection_name + filename
        std::map<std::pair<std::string, std::string>, std::vector<nlohmann::json>> grouped_headers;

        for (const auto& [offset, header] : headers) {
            // Get ASCII versions for grouping
            std::string ascii_collection = EbcdicConverter::toAscii(std::string_view(header.collection_name, 44));
            std::string ascii_filename = EbcdicConverter::toAscii(std::string_view(header.file_name, 44));

            // Trim spaces for proper grouping
            ascii_collection = ascii_collection.substr(0, ascii_collection.find_last_not_of(' ') + 1);
            ascii_filename = ascii_filename.substr(0, ascii_filename.find_last_not_of(' ') + 1);

            auto key = std::make_pair(ascii_collection, ascii_filename);
            grouped_headers[key].push_back(header.to_json(offset));
        }

        // Create final JSON array
        nlohmann::json output = nlohmann::json::array();

        for (const auto& [key, segments] : grouped_headers) {
            const auto& [collection, filename] = key;

            nlohmann::json group = {
                {"collection_name", collection},
                {"file_name", filename},
                {"segment_number", segments.size()},
                {"segments", segments}
            };

            output.push_back(group);
        }

        spdlog::debug("JSON creation completed. Found {} unique collection/filename pairs", output.size());
        return output;
    }
};



