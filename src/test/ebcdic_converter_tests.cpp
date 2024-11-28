#include <catch2/catch_test_macros.hpp>
#include "ebcdic_converter.cpp"

SCENARIO("Converting between ASCII and EBCDIC characters", "[converter]") {
    GIVEN("An EBCDIC converter") {
        WHEN("converting basic ASCII letters to EBCDIC") {
            THEN("uppercase letters are converted correctly") {
                REQUIRE(EbcdicConverter::toEbcdic('A') == 0xC1);
                REQUIRE(EbcdicConverter::toEbcdic('B') == 0xC2);
                REQUIRE(EbcdicConverter::toEbcdic('Z') == 0xE9);
            }

            THEN("lowercase letters are converted correctly") {
                REQUIRE(EbcdicConverter::toEbcdic('a') == 0x81);
                REQUIRE(EbcdicConverter::toEbcdic('b') == 0x82);
                REQUIRE(EbcdicConverter::toEbcdic('z') == 0xA9);
            }
        }

        WHEN("converting basic EBCDIC letters to ASCII") {
            THEN("uppercase letters are converted correctly") {
                REQUIRE(EbcdicConverter::toAscii(0xC1) == 'A');
                REQUIRE(EbcdicConverter::toAscii(0xC2) == 'B');
                REQUIRE(EbcdicConverter::toAscii(0xE9) == 'Z');
            }

            THEN("lowercase letters are converted correctly") {
                REQUIRE(EbcdicConverter::toAscii(0x81) == 'a');
                REQUIRE(EbcdicConverter::toAscii(0x82) == 'b');
                REQUIRE(EbcdicConverter::toAscii(0xA9) == 'z');
            }
        }
    }
}

SCENARIO("Converting strings between ASCII and EBCDIC", "[converter]") {
    GIVEN("An ASCII string with mixed content") {
        std::string ascii_text = "Hello, World! 123";

        WHEN("converting to EBCDIC and back") {
            std::string ebcdic_text = EbcdicConverter::toEbcdic(ascii_text);
            std::string roundtrip_text = EbcdicConverter::toAscii(ebcdic_text);

            THEN("the roundtrip conversion matches the original") {
                REQUIRE(roundtrip_text == ascii_text);
            }

            THEN("the EBCDIC text differs from ASCII text") {
                REQUIRE(ebcdic_text != ascii_text);
                REQUIRE(ebcdic_text.size() == ascii_text.size());
            }
        }
    }

    GIVEN("Special characters in ASCII") {
        std::string special_chars = "!@#$%^&*()";

        WHEN("converting to EBCDIC and back") {
            auto ebcdic = EbcdicConverter::toEbcdic(special_chars);
            auto result = EbcdicConverter::toAscii(ebcdic);

            THEN("the roundtrip conversion preserves special characters") {
                REQUIRE(result == special_chars);
            }
        }
    }

    GIVEN("An empty string") {
        std::string empty_str;

        WHEN("converting to EBCDIC") {
            auto result = EbcdicConverter::toEbcdic(empty_str);

            THEN("the result is also empty") {
                REQUIRE(result.empty());
            }
        }
    }
}