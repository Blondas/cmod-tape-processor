#include <cstdint>
#include <string>
#include <sstream>
#include <string_view>
#include "ebcdic_converter.cpp"

struct header_t {
    char collection_name[44];  // right space padding
    char file_name[44];       // right space padding
    union {                   // big endian
        uint32_t filesize;    // represents total file size
        uint8_t filesize_b[4];
    };
    uint32_t _unknown;        // zero padding
    union {
        uint32_t segsize;     // big endian, represents this segment size
        uint8_t segsize_b[4];
    };
    uint8_t oam_tag[28];     // *OAM zero padded

    std::string to_string(size_t offset) const {
        // Convert EBCDIC fields to ASCII
        std::string ascii_collection = EbcdicConverter::toAscii(std::string_view(collection_name, 44));
        std::string ascii_filename = EbcdicConverter::toAscii(std::string_view(file_name, 44));
        std::string ascii_oam = EbcdicConverter::toAscii(std::string_view(reinterpret_cast<const char*>(oam_tag), 28));

        // Trim right padding
        ascii_collection = ascii_collection.substr(0, ascii_collection.find_last_not_of(' ') + 1);
        ascii_filename = ascii_filename.substr(0, ascii_filename.find_last_not_of(' ') + 1);
        ascii_oam = ascii_oam.substr(0, ascii_oam.find_last_not_of('\0') + 1);

        std::stringstream ss;
        ss << "{\n";
        ss << "  \"header_offset\": " << offset << ",\n";
        // ss << "  \"collection_name\": \"" << ascii_collection << "\",\n";
        // ss << "  \"file_name\": \"" << ascii_filename << "\",\n";
        ss << "  \"filesize\": " << filesize << ",\n";
        ss << "  \"unknown\": " << _unknown << ",\n";
        ss << "  \"segsize\": " << segsize << ",\n";
        ss << "  \"oam_tag\": \"" << ascii_oam << "\"\n";
        ss << "}";

        return ss.str();
    }
};
static_assert(sizeof(header_t) <= 128, "Header must be less than or equal to 128 bytes");