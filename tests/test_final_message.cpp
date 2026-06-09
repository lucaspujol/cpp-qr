#include <criterion/criterion.h>
#include <string>
#include "../src/QRCode.hpp"
#include "../src/QRCodeTables.hpp"

static constexpr int REMAINDER_BITS[40] = {
    0, 7, 7, 7, 7, 7, 0, 0, 0, 0, 0, 0, 0,
    3, 3, 3, 3, 3, 3, 3,
    4, 4, 4, 4, 4, 4, 4,
    3, 3, 3, 3, 3, 3, 3,
    0, 0, 0, 0, 0, 0
};

static int expectedFinalBits(int version, int ecIndex) {
    const ECBlockInfo &info = EC_TABLE[version - 1][ecIndex];
    int totalBlocks = info.group1_blocks + info.group2_blocks;
    int totalBytes = info.total_data_codewords + totalBlocks * info.ec_codewords_per_block;
    return totalBytes * 8 + REMAINDER_BITS[version - 1];
}

Test(FinalMessage, version1_M_bit_length) {
    QRCode qr("HELLO WORLD", CorrectionLevel::M);
    qr.generate();
    // Version 1-M: 16 data + 10 EC = 26 bytes, 0 remainder = 208 bits
    cr_assert_eq(qr.finalMessage().size(), (size_t)expectedFinalBits(1, 1));
}

Test(FinalMessage, version1_M_data_prefix_matches_bits) {
    // Single block: first 128 bits of finalMessage = the 16 data codewords = _bits
    QRCode qr("HELLO WORLD", CorrectionLevel::M);
    qr.generate();
    cr_assert(qr.finalMessage().substr(0, 128) == qr.bits());
}

Test(FinalMessage, version1_no_remainder_bits) {
    QRCode qr("HELLO WORLD", CorrectionLevel::M);
    qr.generate();
    // Version 1 has 0 remainder bits, length divisible by 8
    cr_assert_eq(qr.finalMessage().size() % 8, 0);
}

Test(FinalMessage, version5_remainder_bits_are_zeros) {
    std::string data = "Hello World Hello World Hello World Hello World Hello World";
    QRCode qr(data, CorrectionLevel::Q);
    qr.generate();
    cr_assert_eq(qr.version(), 5);
    std::string fm = qr.finalMessage();
    // Last 7 bits must be zeros (version 5 remainder)
    cr_assert(fm.substr(fm.size() - 7) == "0000000");
}

Test(FinalMessage, version5_Q_total_bit_length) {
    std::string data = "Hello World Hello World Hello World Hello World Hello World";
    QRCode qr(data, CorrectionLevel::Q);
    qr.generate();
    cr_assert_eq(qr.finalMessage().size(), (size_t)expectedFinalBits(5, 2));
}

Test(FinalMessage, only_zeros_and_ones) {
    QRCode qr("HELLO WORLD", CorrectionLevel::M);
    qr.generate();
    for (char c : qr.finalMessage()) {
        cr_assert(c == '0' || c == '1', "final message must only contain '0' and '1'");
    }
}

Test(FinalMessage, invalid_qr_has_empty_final_message) {
    std::string longStr(5000, 'A');
    QRCode qr(longStr, CorrectionLevel::M);
    qr.generate();
    cr_assert(qr.finalMessage().empty());
}
