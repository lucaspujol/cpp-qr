#include <criterion/criterion.h>
#include <string>
#include "../src/QRCode.hpp"

// Reference: https://www.thonky.com/qr-code-tutorial/data-encoding
// "HELLO WORLD" version 1-M is the thonky tutorial golden reference.

Test(DataEncoding, hello_world_alphanumeric_M) {
    QRCode qr("HELLO WORLD", CorrectionLevel::M);
    std::string expected =
        "0010000001011011000010110111100011010001011100101101110001001101"
        "0100001101000000111011000001000111101100000100011110110000010001";
    cr_assert(qr.bits() == expected, "bits mismatch for HELLO WORLD M");
}

Test(DataEncoding, hello_world_bits_length_matches_version_1_M) {
    QRCode qr("HELLO WORLD", CorrectionLevel::M);
    // Version 1-M has 16 data codewords = 128 bits
    cr_assert_eq(qr.bits().size(), 128);
}

Test(DataEncoding, hello_world_Q_shorter_capacity) {
    QRCode qr("HELLO WORLD", CorrectionLevel::Q);
    // Version 1-Q has 13 data codewords = 104 bits
    cr_assert_eq(qr.bits().size(), 104);
}

Test(DataEncoding, hello_world_L_larger_capacity) {
    QRCode qr("HELLO WORLD", CorrectionLevel::L);
    // Version 1-L has 19 data codewords = 152 bits
    cr_assert_eq(qr.bits().size(), 152);
}

Test(DataEncoding, numeric_01234567_M) {
    QRCode qr("01234567", CorrectionLevel::M);
    std::string expected =
        "0001000000100000000011000101011001100001100000001110110000010001"
        "1110110000010001111011000001000111101100000100011110110000010001";
    cr_assert(qr.bits() == expected, "bits mismatch for 01234567 M");
}

Test(DataEncoding, numeric_group_remainder_1) {
    // "1234" = group "123" (10 bits) + "4" (4 bits)
    QRCode qr("1234", CorrectionLevel::M);
    // Mode 0001 + count (10 bits) + 123 in 10 bits + 4 in 4 bits + padding
    cr_assert_eq(qr.bits().size(), 128);
    // Starts with mode indicator 0001
    cr_assert(qr.bits().substr(0, 4) == "0001");
}

Test(DataEncoding, numeric_group_remainder_2) {
    // "12345" = group "123" (10 bits) + "45" (7 bits)
    QRCode qr("12345", CorrectionLevel::M);
    cr_assert_eq(qr.bits().size(), 128);
    cr_assert(qr.bits().substr(0, 4) == "0001");
}

Test(DataEncoding, binary_mode_prefix) {
    QRCode qr("Hello", CorrectionLevel::M);
    // Binary mode indicator is 0100
    cr_assert(qr.bits().substr(0, 4) == "0100");
}

Test(DataEncoding, alphanumeric_odd_length) {
    // "ABC" = pair "AB" (11 bits) + single "C" (6 bits)
    QRCode qr("ABC", CorrectionLevel::M);
    cr_assert_eq(qr.bits().size(), 128);
    // Mode 0010
    cr_assert(qr.bits().substr(0, 4) == "0010");
}

Test(DataEncoding, bits_always_multiple_of_8) {
    QRCode qr1("HELLO WORLD", CorrectionLevel::M);
    cr_assert_eq(qr1.bits().size() % 8, 0);

    QRCode qr2("12345", CorrectionLevel::L);
    cr_assert_eq(qr2.bits().size() % 8, 0);

    QRCode qr3("Hello", CorrectionLevel::Q);
    cr_assert_eq(qr3.bits().size() % 8, 0);
}

Test(DataEncoding, padding_bytes_alternate) {
    // After data + terminator + byte alignment, padding should alternate
    // 0xEC (11101100) and 0x11 (00010001)
    QRCode qr("HELLO WORLD", CorrectionLevel::M);
    std::string bits = qr.bits();
    // The encoded data before padding is 78 bits (mode+count+data),
    // then terminator 0000, then byte-align to 80 bits,
    // then pad bytes start at bit 80
    // Bits 80-87 should be 11101100, bits 88-95 should be 00010001, etc.
    cr_assert(bits.substr(80, 8) == "11101100");
    cr_assert(bits.substr(88, 8) == "00010001");
    cr_assert(bits.substr(96, 8) == "11101100");
}
