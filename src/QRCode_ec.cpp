#include "QRCode.hpp"
#include "QRCodeTables.hpp"
#include <algorithm>
#include <bitset>

QRCode::ECResult QRCode::errorCorrectionCoding() {
    std::vector<uint8_t> dataCodewords;
    for (size_t i = 0; i < _bits.size(); i += 8) {
        uint8_t byte = static_cast<uint8_t>(std::stoi(_bits.substr(i, 8), nullptr, 2));
        dataCodewords.push_back(byte);
    }

    const ECBlockInfo &blockInfo = EC_TABLE[_version - 1][static_cast<int>(_ec)];
    std::vector<std::vector<uint8_t>> dataBlocks;
    size_t offset = 0;

    for (size_t i = 0; i < blockInfo.group1_blocks; i++) {
        dataBlocks.push_back(std::vector<uint8_t>(
            dataCodewords.begin() + offset,
            dataCodewords.begin() + offset + blockInfo.group1_data_codewords
        ));
        offset += blockInfo.group1_data_codewords;
    }
    for (size_t i = 0; i < blockInfo.group2_blocks; i++) {
        dataBlocks.push_back(std::vector<uint8_t>(
            dataCodewords.begin() + offset,
            dataCodewords.begin() + offset + blockInfo.group2_data_codewords
        ));
        offset += blockInfo.group2_data_codewords;
    }

    std::vector<uint8_t> generator = buildGeneratorPolynomial(blockInfo.ec_codewords_per_block);
    std::vector<std::vector<uint8_t>> ecBlocks;
    for (const auto &block : dataBlocks)
        ecBlocks.push_back(generateECCodewords(block, generator));

    return ECResult{dataBlocks, ecBlocks};
}

uint8_t QRCode::gf256Multiply(uint8_t a, uint8_t b) {
    if (a == 0 || b == 0) return 0;
    return GF_EXP[(GF_LOG[a] + GF_LOG[b]) % 255];
}

std::vector<uint8_t> QRCode::buildGeneratorPolynomial(int n) {
    std::vector<uint8_t> generator = {1};
    for (int i = 0; i < n; i++) {
        std::vector<uint8_t> next(generator.size() + 1, 0);
        for (size_t j = 0; j < generator.size(); j++) {
            next[j]     ^= generator[j];
            next[j + 1] ^= gf256Multiply(generator[j], GF_EXP[i]);
        }
        generator = next;
    }
    return generator;
}

std::vector<uint8_t> QRCode::generateECCodewords(
    const std::vector<uint8_t> &dataCodewords,
    const std::vector<uint8_t> &generator
) {
    int ecCount = generator.size() - 1;
    std::vector<uint8_t> working(dataCodewords.size() + ecCount, 0);
    std::copy(dataCodewords.begin(), dataCodewords.end(), working.begin());

    for (size_t i = 0; i < dataCodewords.size(); i++) {
        uint8_t factor = working[i];
        if (factor != 0) {
            for (size_t j = 0; j < generator.size(); j++)
                working[i + j] ^= gf256Multiply(factor, generator[j]);
        }
    }
    return std::vector<uint8_t>(working.end() - ecCount, working.end());
}

std::string QRCode::structureFinalMessage(const ECResult &ecResult) {
    std::vector<uint8_t> finalMessage;
    size_t maxDataLen = 0;
    for (const auto &block : ecResult.dataBlocks)
        maxDataLen = std::max(maxDataLen, block.size());

    for (size_t i = 0; i < maxDataLen; i++)
        for (const auto &block : ecResult.dataBlocks)
            if (i < block.size())
                finalMessage.push_back(block[i]);

    for (size_t i = 0; i < ecResult.ecBlocks[0].size(); i++)
        for (const auto &block : ecResult.ecBlocks)
            if (i < block.size())
                finalMessage.push_back(block[i]);

    std::string bitString;
    for (uint8_t byte : finalMessage)
        bitString += std::bitset<8>(byte).to_string();

    bitString += std::string(REMAINDER_BITS[_version - 1], '0');
    return bitString;
}
