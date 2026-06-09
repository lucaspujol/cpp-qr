#include "QRCode.hpp"
#include "QRCodeTables.hpp"
#include <algorithm>
#include <bitset>

// ============================================================================
// mode and version selection
// ============================================================================

EncodingMode QRCode::selectMode(std::string_view string) noexcept {
    if (isNumeric(string))      return EncodingMode::Numeric;
    if (isAlphanumeric(string)) return EncodingMode::Alphanumeric;
    return EncodingMode::Binary;
}

int QRCode::selectVersion(std::string_view string, EncodingMode mode, CorrectionLevel ec) {
    int charCount = string.size();
    for (int version = 0; version < 40; version++) {
        int modeIndex = static_cast<int>(mode);
        int ecIndex = static_cast<int>(ec);
        if (charCount <= CHAR_CAPACITIES[version][ecIndex][modeIndex])
            return version + 1;
    }
    return -1;
}

bool QRCode::isAlphanumeric(std::string_view string) noexcept {
    if (string.empty()) return false;
    for (char c : string)
        if (alphanumericChars.find(c) == std::string::npos)
            return false;
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
        case EncodingMode::Numeric:      encoded += "0001"; break;
        case EncodingMode::Alphanumeric: encoded += "0010"; break;
        case EncodingMode::Binary:       encoded += "0100"; break;
    }
}

int QRCode::getCharCountBits(int version, EncodingMode mode) {
    if (version >= 1 && version <= 9) {
        switch (mode) {
            case EncodingMode::Numeric:      return 10;
            case EncodingMode::Alphanumeric: return 9;
            case EncodingMode::Binary:       return 8;
        }
    } else if (version >= 10 && version <= 26) {
        switch (mode) {
            case EncodingMode::Numeric:      return 12;
            case EncodingMode::Alphanumeric: return 11;
            case EncodingMode::Binary:       return 16;
        }
    } else if (version >= 27 && version <= 40) {
        switch (mode) {
            case EncodingMode::Numeric:      return 14;
            case EncodingMode::Alphanumeric: return 13;
            case EncodingMode::Binary:       return 16;
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
        case EncodingMode::Numeric:      encodeNumericData(encoded);      break;
        case EncodingMode::Alphanumeric: encodeAlphanumericData(encoded); break;
        case EncodingMode::Binary:       encodeBinaryData(encoded);       break;
    }
}

void QRCode::encodeBinaryData(std::string &encoded) {
    for (char c : _data)
        encoded += std::bitset<8>(c).to_string();
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
