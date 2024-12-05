#include <vector>
#include <string>
#include <string_view>
#include <cstring>
#include <ostream>

#include "mmap_file_reader.cpp"
#include "ebcdic_converter.cpp"

struct TapeSegment {
    size_t header_offset;
    std::string collection_name;
    std::string file_name;
    size_t data_start_offset;
    size_t data_end_offset;
};

class TapeScanner {
public:
    static std::vector<TapeSegment> scanTape(const MmapFileReader& reader, std::string_view collection_name) {
        std::vector<TapeSegment> segments;
        const char* data = reader.data();
        const size_t file_size = reader.size();

        // Prepare EBCDIC collection name for comparison
        std::string collection_padded = std::string(collection_name);
        collection_padded.resize(44, ' ');
        std::string ebcdic_collection = EbcdicConverter::toEbcdic(collection_padded);

        size_t current_pos = 0;
        while (current_pos + HEADER_SIZE <= file_size) {
            if (std::memcmp(data + current_pos, ebcdic_collection.data(), 44) == 0) {
                // If we have a previous segment, set its end offset
                if (!segments.empty()) {
                    segments.back().data_end_offset = current_pos;
                }

                // Create new segment
                TapeSegment segment;
                segment.header_offset = current_pos;
                segment.collection_name = EbcdicConverter::toAscii(std::string_view(data + current_pos, 44));
                segment.file_name = EbcdicConverter::toAscii(std::string_view(data + current_pos + 44, 44));
                segment.data_start_offset = current_pos + HEADER_SIZE;
                segment.data_end_offset = file_size;  // Will be updated when we find next header

                segments.push_back(segment);
            }
            current_pos += HEADER_SIZE;
        }

        return segments;
    }

    static void writeCsv(const std::vector<TapeSegment>& segments, std::ostream& out) {
        // Write CSV header
        out << "header_offset,collection_name,file_name,data_start_offset,data_end_offset\n";

        // Write each segment
        for (const auto& segment : segments) {
            // Remove trailing spaces from names
            std::string collection = segment.collection_name;
            std::string filename = segment.file_name;

            // Simple right trim
            while (!collection.empty() && collection.back() == ' ') {
                collection.pop_back();
            }
            while (!filename.empty() && filename.back() == ' ') {
                filename.pop_back();
            }

            out << segment.header_offset << ","
                << collection << ","
                << filename << ","
                << segment.data_start_offset << ","
                << segment.data_end_offset << "\n";
        }
    }

private:
    static constexpr size_t HEADER_SIZE = 128;
};