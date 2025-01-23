#include <iostream>
#include <filesystem>
#include <fstream>
#include <map>
#include <sstream>

#include "mmap_file_reader.cpp"
#include "tape_scanner.cpp"

class Application {
public:
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
        std::cout << "Starting tape header scan for file: " << filename
                  << ", collection: " << collection_name << std::endl;

        try {
            std::cout << "Opening file with memory mapping" << std::endl;
            MmapFileReader reader(filename);
            std::cout << "File mapped successfully, size: " << reader.size() << " bytes" << std::endl;

            std::cout << "Starting header scan" << std::endl;
            auto headers = TapeScanner::scanTape(reader, collection_name);
            std::cout << "Found " << headers.size() << " headers in the tape" << std::endl;

            // Create grouped JSON output
            std::string output = createGroupedJson(headers);

            // Print to console
            std::cout << "Converting headers to JSON" << std::endl;
            std::cout << output << std::endl;

            // Save to file
            std::string output_filename = filename + "_headers.json";
            std::cout << "Saving results to: " << output_filename << std::endl;

            std::ofstream out_file(output_filename);
            if (!out_file) {
                throw std::runtime_error("Failed to create output file: " + output_filename);
            }

            out_file << output;
            std::cout << "Scan completed successfully" << std::endl;

        } catch (const std::exception& e) {
            std::cerr << "Failed to scan tape headers: " << e.what() << std::endl;
            throw;
        }
    }

private:
    static std::string createGroupedJson(const std::vector<std::tuple<size_t, header_t>>& headers) {
        std::cout << "Creating grouped JSON output" << std::endl;

        // Map to group headers by collection_name + filename
        std::map<std::pair<std::string, std::string>, std::vector<std::string>> grouped_headers;

        for (const auto& [offset, header] : headers) {
            // Get ASCII versions for grouping
            std::string ascii_collection = EbcdicConverter::toAscii(std::string_view(header.collection_name, 44));
            std::string ascii_filename = EbcdicConverter::toAscii(std::string_view(header.file_name, 44));

            // Trim spaces for proper grouping
            ascii_collection = ascii_collection.substr(0, ascii_collection.find_last_not_of(' ') + 1);
            ascii_filename = ascii_filename.substr(0, ascii_filename.find_last_not_of(' ') + 1);

            auto key = std::make_pair(ascii_collection, ascii_filename);
            grouped_headers[key].push_back(header.to_string(offset));
        }

        // Create final JSON array
        std::stringstream output;
        output << "[\n";
        bool first_group = true;

        for (const auto& [key, segments] : grouped_headers) {
            const auto& [collection, filename] = key;

            if (!first_group) {
                output << ",\n";
            }
            first_group = false;

            output << "  {\n";
            output << "    \"collection_name\": \"" << collection << "\",\n";
            output << "    \"file_name\": \"" << filename << "\",\n";
            output << "    \"segment_number\": " << segments.size() << ",\n";
            output << "    \"segments\": [\n";

            // Add segments
            for (size_t i = 0; i < segments.size(); ++i) {
                output << "      " << segments[i];
                if (i < segments.size() - 1) {
                    output << ",";
                }
                output << "\n";
            }

            output << "    ]\n";
            output << "  }";
        }
        output << "\n]";

        std::cout << "JSON creation completed. Found " << grouped_headers.size()
                  << " unique collection/filename pairs" << std::endl;
        return output.str();
    }
};