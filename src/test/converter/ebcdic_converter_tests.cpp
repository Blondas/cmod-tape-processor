#include <catch2/catch_test_macros.hpp>
#include "converter/ebcdic_converter.cpp"

SCENARIO("ASCII to EBCDIC conversion validation", "[ebcdic]") {
    GIVEN("the ASCII to EBCDIC conversion table") {
        const auto& asciiToEbcdicTable = EbcdicConverter::asciiToEbcdicTable;

        WHEN("converting each value from the original table") {
            THEN("converted values should match the table exactly") {
                for (unsigned i = 0; i < 256; ++i) {
                    auto ascii = static_cast<unsigned char>(i);
                    auto converted = EbcdicConverter::toEbcdic(ascii);
                    auto expected = asciiToEbcdicTable[i];

                    INFO("ASCII value: 0x" << std::hex << static_cast<int>(ascii));
                    INFO("Converted: 0x" << std::hex << static_cast<int>(converted));
                    INFO("Expected: 0x" << std::hex << static_cast<int>(expected));

                    REQUIRE(converted == expected);
                }
            }
        }
    }
}

SCENARIO("ASCII to EBCDIC conversion validation using known pairs", "[ebcdic]") {
    WHEN("converting each ASCII value to EBCDIC") {
        THEN("conversions should match known EBCDIC values") {
            for (unsigned i = 0; i < 256; ++i) {
                auto ascii = static_cast<unsigned char>(i);
                auto ebcdic = EbcdicConverter::toEbcdic(ascii);
                auto backToAscii = EbcdicConverter::toAscii(ebcdic);

                INFO("ASCII: 0x" << std::hex << static_cast<int>(ascii));
                INFO("EBCDIC: 0x" << std::hex << static_cast<int>(ebcdic));
                INFO("Back to ASCII: 0x" << std::hex << static_cast<int>(backToAscii));

                REQUIRE(backToAscii == ascii);
            }
        }
    }
}