#include <array>
#include <string>
#include <string_view>

class EbcdicConverter {
public:
    // Convert a single EBCDIC character to ASCII
    static unsigned char toAscii(unsigned char ebcdic) {
        return ebcdicToAsciiTable[ebcdic];
    }

    // Convert a string of EBCDIC characters to ASCII
    static std::string toAscii(std::string_view ebcdicStr) {
        std::string result;
        result.reserve(ebcdicStr.size());
        
        for (const unsigned char c : ebcdicStr) {
            result.push_back(static_cast<char>(toAscii(c)));
        }
        
        return result;
    }

    // Convert a single ASCII character to EBCDIC
    static unsigned char toEbcdic(const unsigned char ascii) {
        return asciiToEbcdicTable[ascii];
    }

    // Convert a string of ASCII characters to EBCDIC
    static std::string toEbcdic(std::string_view asciiStr) {
        std::string result;
        result.reserve(asciiStr.size());
        
        for (const unsigned char c : asciiStr) {
            result.push_back(static_cast<char>(toEbcdic(c)));
        }
        
        return result;
    }

private:
    // EBCDIC to ASCII conversion table - derived from the matrix
    static constexpr std::array<unsigned char, 256> ebcdicToAsciiTable = {
        0x00,0x01,0x02,0x03,0x9C,0x09,0x86,0x7F,0x97,0x8D,0x8E,0x0B,0x0C,0x0D,0x0E,0x0F,  // 0_
        0x10,0x11,0x12,0x13,0x9D,0x85,0x08,0x87,0x18,0x19,0x92,0x8F,0x1C,0x1D,0x1E,0x1F,  // 1_
        0x80,0x81,0x82,0x83,0x84,0x0A,0x17,0x1B,0x88,0x89,0x8A,0x8B,0x8C,0x05,0x06,0x07,  // 2_
        0x90,0x91,0x16,0x93,0x94,0x95,0x96,0x04,0x98,0x99,0x9A,0x9B,0x14,0x15,0x9E,0x1A,  // 3_
        0x20,0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0x5B,0x2E,0x3C,0x28,0x2B,0x21,  // 4_
        0x26,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0x5D,0x24,0x2A,0x29,0x3B,0x5E,  // 5_
        0x2D,0x2F,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0x7C,0x2C,0x25,0x5F,0x3E,0x3F,  // 6_
        0xB9,0xBB,0xBC,0xBD,0xBE,0xBF,0xC0,0xC1,0xC2,0x60,0x3A,0x23,0x40,0x27,0x3D,0x22,  // 7_
        0xC3,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,  // 8_
        0xCA,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,0x70,0x71,0x72,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,  // 9_
        0xD1,0x7E,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,  // A_
        0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF,0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,  // B_
        0x7B,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,  // C_
        0x7D,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,0x50,0x51,0x52,0xEE,0xEF,0xF0,0xF1,0xF2,0xF3,  // D_
        0x5C,0x9F,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,  // E_
        0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF   // F_
    };

    // ASCII to EBCDIC conversion table
    static constexpr std::array<unsigned char, 256> asciiToEbcdicTable = {
        0x00,0x01,0x02,0x03,0x37,0x2D,0x2E,0x2F,0x16,0x05,0x15,0x0B,0x0C,0x0D,0x0E,0x0F,
        0x10,0x11,0x12,0x13,0x3C,0x3D,0x32,0x26,0x18,0x19,0x3F,0x27,0x1C,0x1D,0x1E,0x1F,
        0x40,0x5A,0x7F,0x7B,0x5B,0x6C,0x50,0x7D,0x4D,0x5D,0x5C,0x4E,0x6B,0x60,0x4B,0x61,
        0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0x7A,0x5E,0x4C,0x7E,0x6E,0x6F,
        0x7C,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,
        0xD7,0xD8,0xD9,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xAD,0xE0,0xBD,0x5F,0x6D,
        0x79,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x91,0x92,0x93,0x94,0x95,0x96,
        0x97,0x98,0x99,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xC0,0x4F,0xD0,0xA1,0x07,
        0x20,0x21,0x22,0x23,0x24,0x25,0x06,0x17,0x28,0x29,0x2A,0x2B,0x2C,0x09,0x0A,0x1B,
        0x30,0x31,0x1A,0x33,0x34,0x35,0x36,0x08,0x38,0x39,0x3A,0x3B,0x04,0x14,0x3E,0xFF,
        0x41,0xAA,0x4A,0xB1,0x9F,0xB2,0x6A,0xB5,0xBB,0xB4,0x9A,0x8A,0xB0,0xCA,0xAF,0xBC,
        0x90,0x8F,0xEA,0xFA,0xBE,0xA0,0xB6,0xB3,0x9D,0xDA,0x9B,0x8B,0xB7,0xB8,0xB9,0xAB,
        0x64,0x65,0x62,0x66,0x63,0x67,0x9E,0x68,0x74,0x71,0x72,0x73,0x78,0x75,0x76,0x77,
        0xAC,0x69,0xED,0xEE,0xEB,0xEF,0xEC,0xBF,0x80,0xFD,0xFE,0xFB,0xFC,0xBA,0xAE,0x59,
        0x44,0x45,0x42,0x46,0x43,0x47,0x9C,0x48,0x54,0x51,0x52,0x53,0x58,0x55,0x56,0x57,
        0x8C,0x49,0xCD,0xCE,0xCB,0xCF,0xCC,0xE1,0x70,0xDD,0xDE,0xDB,0xDC,0x8D,0x8E,0xDF
    };
};
