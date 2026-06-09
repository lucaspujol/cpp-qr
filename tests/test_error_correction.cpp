#include <criterion/criterion.h>
#include <vector>
#include "../src/QRCode.hpp"
#include "../src/QRCodeTables.hpp"

// Reference: https://www.thonky.com/qr-code-tutorial/error-correction-coding
// "HELLO WORLD" version 1-M EC codewords are the thonky golden reference.

Test(ErrorCorrection, hello_world_M_block_count) {
    QRCode qr("HELLO WORLD", CorrectionLevel::M);
    qr.generate();
    // Version 1-M has 1 block in group 1, 0 in group 2
    cr_assert_eq(qr.dataBlockCount(), 1);
    cr_assert_eq(qr.ecBlockCount(), 1);
}

Test(ErrorCorrection, hello_world_M_ec_codewords) {
    // Thonky reference: https://www.thonky.com/qr-code-tutorial/error-correction-coding
    QRCode qr("HELLO WORLD", CorrectionLevel::M);
    qr.generate();
    std::vector<uint8_t> expected = {196, 35, 39, 119, 235, 215, 231, 226, 93, 23};
    cr_assert(qr.ecBlock(0) == expected, "EC codewords mismatch for HELLO WORLD M");
}

Test(ErrorCorrection, hello_world_M_ec_block_length) {
    QRCode qr("HELLO WORLD", CorrectionLevel::M);
    qr.generate();
    // Version 1-M: 10 EC codewords per block
    cr_assert_eq(qr.ecBlock(0).size(), (size_t)EC_TABLE[0][1].ec_codewords_per_block);
}

Test(ErrorCorrection, hello_world_M_data_block_content) {
    QRCode qr("HELLO WORLD", CorrectionLevel::M);
    qr.generate();
    // Data block must match the 16 bytes from the encoded bit string
    std::string bits = qr.bits();
    std::vector<uint8_t> expectedData;
    for (size_t i = 0; i < bits.size(); i += 8) {
        expectedData.push_back(static_cast<uint8_t>(std::stoi(bits.substr(i, 8), nullptr, 2)));
    }
    cr_assert(qr.dataBlock(0) == expectedData, "Data block content mismatch");
}

Test(ErrorCorrection, version5_Q_block_count) {
    std::string data = "Hello World Hello World Hello World Hello World Hello World";
    QRCode qr(data, CorrectionLevel::Q);
    qr.generate();
    // Version 5-Q: 2 group1 blocks + 2 group2 blocks = 4 total
    cr_assert_eq(qr.dataBlockCount(), 4);
    cr_assert_eq(qr.ecBlockCount(), 4);
}

Test(ErrorCorrection, version5_Q_ec_block_lengths) {
    std::string data = "Hello World Hello World Hello World Hello World Hello World";
    QRCode qr(data, CorrectionLevel::Q);
    qr.generate();
    int expectedECLen = EC_TABLE[4][2].ec_codewords_per_block;
    for (int i = 0; i < qr.ecBlockCount(); i++) {
        cr_assert_eq((int)qr.ecBlock(i).size(), expectedECLen);
    }
}

Test(ErrorCorrection, version5_Q_data_block_lengths) {
    std::string data = "Hello World Hello World Hello World Hello World Hello World";
    QRCode qr(data, CorrectionLevel::Q);
    qr.generate();
    const ECBlockInfo &info = EC_TABLE[4][2];
    // group1 blocks (first 2): info.group1_data_codewords each
    for (int i = 0; i < (int)info.group1_blocks; i++) {
        cr_assert_eq((int)qr.dataBlock(i).size(), (int)info.group1_data_codewords);
    }
    // group2 blocks (last 2): info.group2_data_codewords each
    for (int i = 0; i < (int)info.group2_blocks; i++) {
        cr_assert_eq((int)qr.dataBlock(info.group1_blocks + i).size(), (int)info.group2_data_codewords);
    }
}
