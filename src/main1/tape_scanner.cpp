#include <vector>
#include <tuple>
#include <span>
#include <cstring>
#include <spdlog/spdlog.h>
#include "mmap_file_reader.cpp"
#include "header_t.cpp"

class TapeScanner {
private:
    static std::string toHex(const std::string& input) {
        std::ostringstream hex;
        hex << std::hex << std::setfill('0');

        for (unsigned char c : input) {
            hex << std::setw(2) << static_cast<int>(c) << ' ';
        }

        return hex.str();
    }
public:
    using ScanResult = std::vector<std::tuple<size_t, header_t>>;
    static constexpr size_t HEADER_SIZE = 128;

    static ScanResult scanTape(const MmapFileReader& reader, std::string_view collection_name) {
        ScanResult results;
        const char* data = reader.data();
        const size_t file_size = reader.size();

        spdlog::debug("Starting tape scan. File size: {} bytes", file_size);

        // Convert collection name to EBCDIC for comparison
        std::string collection_padded = std::string(collection_name);
        collection_padded.resize(44, ' ');  // Pad with ASCII spaces to full length

        // Convert the padded string to EBCDIC
        std::string ebcdic_collection = EbcdicConverter::toEbcdic(collection_padded);
        spdlog::debug("Collection name converted to EBCDIC and padded: {}", toHex(ebcdic_collection));

        size_t current_pos = 0;
        while (current_pos + HEADER_SIZE <= file_size) {
            // Check if we have a matching collection name
            if (std::memcmp(data + current_pos, ebcdic_collection.data(), 44) == 0) {
                spdlog::debug("Found matching collection name at position {}", current_pos);

                header_t header;
                std::memcpy(&header, data + current_pos, sizeof(header_t));

                // Set sizes to 0 instead of converting endianness
                header.filesize = 0;
                header.segsize = 0;

                spdlog::debug("Header values at {}: filesize: {}, segsize: {}",
                    current_pos, header.filesize, header.segsize);

                results.emplace_back(current_pos, header);

                // Move to next segment using the HEADER_SIZE since we're not using segsize
                current_pos += HEADER_SIZE;
                spdlog::debug("Moving to next segment at position {}", current_pos);
            } else {
                // If no match, move to next potential header
                current_pos += HEADER_SIZE;
                spdlog::trace("No match at {}, moving to next header position", current_pos - HEADER_SIZE);
            }
        }

        spdlog::info("Scan complete. Found {} headers", results.size());
        return results;
    }
};