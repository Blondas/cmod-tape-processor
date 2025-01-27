#include <vector>
#include <string>
#include <string_view>
#include <cstring>
#include <ostream>
#include <filesystem>
#include <fstream>
#include <unordered_map>
#include <iostream>
#include "mmap_file_reader.cpp"
#include "ebcdic_converter.cpp"

// Represents a segment of data on the tape
struct TapeSegment {
    size_t segment_id;
    size_t header_offset;
    std::string collection_name;
    std::string file_name;
    size_t data_offset;
    size_t data_size;
};

class TapeSlicer {
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
        size_t current_id = 0;
        while (current_pos + HEADER_SIZE <= file_size) {
            // Check if we have a matching header at current position
            if (std::memcmp(data + current_pos, ebcdic_collection.data(), 44) == 0) {
                // If we have a previous segment, set its end offset to one byte before current header
                if (!segments.empty()) {
                    segments.back().data_size = current_pos - segments.back().data_offset;
                }

                // Create new segment
                TapeSegment segment{
                    .segment_id = current_id++,
                    .header_offset = current_pos,
                    .collection_name = EbcdicConverter::toAscii(std::string_view(data + current_pos, 44)),
                    .file_name = EbcdicConverter::toAscii(std::string_view(data + current_pos + 44, 44)),
                    .data_offset = current_pos + HEADER_SIZE,
                    .data_size = file_size - current_pos + HEADER_SIZE + 1 // Read until the actual last byte of the file
                };

                segments.push_back(std::move(segment));

                // Move to the end of current header
                current_pos += HEADER_SIZE;
            } else {
                // Move to next byte
                ++current_pos;
            }
        }

        return segments;
    }

    static bool saveSegments(const MmapFileReader& reader,
                           const std::vector<TapeSegment>& segments,
                           const std::filesystem::path& output_dir) {
        try {
            std::filesystem::create_directories(output_dir);

            // Group segments by trimmed filename
            std::unordered_map<std::string, std::vector<const TapeSegment*>> segments_by_file;
            for (const auto& segment : segments) {
                auto trimmed_name = trimSpaces(segment.file_name);
                segments_by_file[trimmed_name].push_back(&segment);
            }

            const char* tape_data = reader.data();

            // Process each unique file
            for (const auto& [filename, file_segments] : segments_by_file) {
                auto output_path = output_dir / std::filesystem::path(filename);
                std::ofstream output_file(output_path, std::ios::binary);

                if (!output_file) {
                    throw std::runtime_error("Failed to create output file: " + filename);
                }

                // Write each segment's data range
                for (const TapeSegment* segment : file_segments) {

                    // Write the segment data
                    output_file.write(
                        tape_data + segment->data_offset,
                        segment->data_size
                    );

                    if (!output_file) {
                        throw std::runtime_error("Failed writing to file: " + filename);
                    }
                }
            }

            return true;
        }
        catch (const std::exception& e) {
            // Log error or handle as needed
            return false;
        }
    }

    static void writeSegmentsCsv(const std::vector<TapeSegment>& segments, std::ostream& out) {
        out << "segment_id,header_offset,collection_name,file_name,data_offset,data_size\n";

        for (const auto& segment : segments) {
            // Remove trailing spaces from names
            auto trim_spaces = [](std::string str) {
                if (auto pos = str.find_last_not_of(' '); pos != std::string::npos) {
                    str.erase(pos + 1);
                }
                return str;
            };

            out << segment.segment_id << ","
                << segment.header_offset << ","
                << trim_spaces(segment.collection_name) << ","
                << trim_spaces(segment.file_name) << ","
                << segment.data_offset << ","
                << segment.data_size << "\n";
        }
    }

    static std::string trimSpaces(std::string str) {
        if (auto pos = str.find_last_not_of(' '); pos != std::string::npos) {
            str.erase(pos + 1);
        }
        return str;
    }

private:
    static constexpr size_t HEADER_SIZE = 128;
};