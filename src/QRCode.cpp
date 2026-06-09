#include "QRCode.hpp"
#include <iostream>
#include <algorithm>
#include "QRCodeTables.hpp"
#include <bitset>

// ============================================================================
// lifecycle
// ============================================================================

QRCode::QRCode(std::string data, CorrectionLevel ec)
: _data(std::move(data)), _ec(ec) {
    _mode = selectMode(_data);
    _version = selectVersion(_data, _mode, _ec);
    if (_version != -1) {
        _bits = encodeData();
    }
}

void QRCode::generate() {
    if (!isValid()) {
        std::cerr << "Error: Input data exceeds maximum capacity for any QR code version." << std::endl;
        return;
    }

    _ecResult = errorCorrectionCoding();
    _finalMessage = structureFinalMessage(_ecResult);
    _matrix = buildMatrix();
}

void QRCode::save(const std::string &filepath) {
    if (!isValid()) {
        std::cerr << "Error: Cannot save invalid QR code" << std::endl;
        return;
    }
    std::cout << "saving qrcode into: " << filepath << std::endl;
}

// ============================================================================
// mode and version selection
// ============================================================================

EncodingMode QRCode::selectMode(std::string_view string) noexcept {
    if (isNumeric(string))        return EncodingMode::Numeric;
    if (isAlphanumeric(string))   return EncodingMode::Alphanumeric;
    return EncodingMode::Binary;
}

int QRCode::selectVersion(std::string_view string, EncodingMode mode, CorrectionLevel ec) {
    int charCount = string.size();
    for (int version = 0; version < 40; version++) {
        int modeIndex = static_cast<int>(mode);
        int ecIndex = static_cast<int>(ec);
        if (charCount <= CHAR_CAPACITIES[version][ecIndex][modeIndex]) {
            return version + 1;
        }
    }
    return -1;
}

bool QRCode::isAlphanumeric(std::string_view string) noexcept {
    if (string.empty()) return false;
    for (char c : string) {
        if (alphanumericChars.find(c) == std::string::npos)
            return false;
    }
    return true;
}

bool QRCode::isNumeric(std::string_view string) noexcept {
    return !string.empty() &&
        std::all_of(string.begin(), string.end(), ::isdigit);
}

// ============================================================================
// data encoding
// ============================================================================

std::string QRCode::encodeData() {
    std::string encoded;
    addModePrefix(encoded);
    addCharCountIndicator(encoded);
    addEncodedData(encoded);
    addPadding(encoded);
    return encoded;
}

void QRCode::addModePrefix(std::string &encoded) {
    switch (_mode) {
        case EncodingMode::Numeric:         encoded += "0001"; break;
        case EncodingMode::Alphanumeric:    encoded += "0010"; break;
        case EncodingMode::Binary:          encoded += "0100"; break;
    }
}

int QRCode::getCharCountBits(int version, EncodingMode mode) {
    if (version >= 1 && version <= 9) {
        switch (mode) {
            case EncodingMode::Numeric:         return 10;
            case EncodingMode::Alphanumeric:    return 9;
            case EncodingMode::Binary:          return 8;
        }
    } else if (version >= 10 && version <= 26) {
        switch (mode) {
            case EncodingMode::Numeric:         return 12;
            case EncodingMode::Alphanumeric:    return 11;
            case EncodingMode::Binary:          return 16;
        }
    } else if (version >= 27 && version <= 40) {
        switch (mode) {
            case EncodingMode::Numeric:         return 14;
            case EncodingMode::Alphanumeric:    return 13;
            case EncodingMode::Binary:          return 16;
        }
    }
    return -1;
}

void QRCode::addCharCountIndicator(std::string &encoded) {
    int charCount = _data.size();
    int charCountBits = getCharCountBits(_version, _mode);
    std::string charCountBinary = std::bitset<16>(charCount).to_string();
    encoded += charCountBinary.substr(16 - charCountBits);
}

void QRCode::addEncodedData(std::string &encoded) {
    switch (_mode) {
        case EncodingMode::Numeric:         encodeNumericData(encoded);         break;
        case EncodingMode::Alphanumeric:    encodeAlphanumericData(encoded);    break;
        case EncodingMode::Binary:          encodeBinaryData(encoded);          break;
    }
}

void QRCode::encodeBinaryData(std::string &encoded) {
    for (char c : _data) {
        encoded += std::bitset<8>(c).to_string();
    }
}

void QRCode::encodeAlphanumericData(std::string &encoded) {
    for (size_t i = 0; i < _data.size(); i += 2) {
        std::string group = _data.substr(i, 2);
        if (group.size() == 2) {
            int first  = alphanumericChars.find(group[0]);
            int second = alphanumericChars.find(group[1]);
            encoded += std::bitset<11>(45 * first + second).to_string();
        } else {
            encoded += std::bitset<6>(alphanumericChars.find(group[0])).to_string();
        }
    }
}

void QRCode::encodeNumericData(std::string &encoded) {
    for (size_t i = 0; i < _data.size(); i += 3) {
        std::string group = _data.substr(i, 3);
        int value = std::stoi(group);
        int bits = (group.size() == 3) ? 10 : (group.size() == 2) ? 7 : 4;
        encoded += std::bitset<10>(value).to_string().substr(10 - bits);
    }
}

void QRCode::addPadding(std::string &encoded) {
    int totalDataBytes = EC_TABLE[_version - 1][static_cast<int>(_ec)].total_data_codewords;
    int requiredBits = totalDataBytes * 8;
    int diff = requiredBits - encoded.size();

    if (diff > 4) {
        encoded += "0000";
        diff -= 4;
    } else if (diff > 0) {
        encoded += std::string(diff, '0');
        diff = 0;
    }

    if (encoded.size() % 8 != 0) {
        int paddingBits = 8 - (encoded.size() % 8);
        encoded += std::string(paddingBits, '0');
        diff -= paddingBits;
    }

    const std::string padBytes[] = {"11101100", "00010001"};
    int padIndex = 0;
    while (diff > 0) {
        encoded += padBytes[padIndex];
        padIndex = (padIndex + 1) % 2;
        diff -= 8;
    }
}

// ============================================================================
// error correction coding
// ============================================================================

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
    for (const auto &block : dataBlocks) {
        ecBlocks.push_back(generateECCodewords(block, generator));
    }

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
            for (size_t j = 0; j < generator.size(); j++) {
                working[i + j] ^= gf256Multiply(factor, generator[j]);
            }
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

// ============================================================================
// matrix construction
// ============================================================================

QRCode::Matrix QRCode::buildMatrix() {
    int size = 4 * _version + 17;
    _isFn = Matrix(size, std::vector<int8_t>(size, -1));
    return Matrix(size, std::vector<int8_t>(size, -1));
}
