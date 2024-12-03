#include <cstdint>
#include <string>
#include <nlohmann/json.hpp>
#include <string_view>
#include "converter/ebcdic_converter.cpp"  // Include the actual implementation instead of forward declaration

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

    nlohmann::json to_json(size_t offset) const {
        // Convert EBCDIC fields to ASCII
        std::string ascii_collection = EbcdicConverter::toAscii(std::string_view(collection_name, 44));
        std::string ascii_filename = EbcdicConverter::toAscii(std::string_view(file_name, 44));
        std::string ascii_oam = EbcdicConverter::toAscii(std::string_view(reinterpret_cast<const char*>(oam_tag), 28));

        // Trim right padding
        ascii_collection = ascii_collection.substr(0, ascii_collection.find_last_not_of(' ') + 1);
        ascii_filename = ascii_filename.substr(0, ascii_filename.find_last_not_of(' ') + 1);
        ascii_oam = ascii_oam.substr(0, ascii_oam.find_last_not_of('\0') + 1);

        return {
                {"header_offset", offset},
                // {"collection_name", ascii_collection},
                // {"file_name", ascii_filename},
                {"filesize", filesize},
                {"unknown", _unknown},
                {"segsize", segsize},
                {"oam_tag", ascii_oam}
        };
    }
};
static_assert(sizeof(header_t) <= 128, "Header must be less than or equal to 128 bytes");