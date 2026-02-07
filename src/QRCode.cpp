#include "QRCode.hpp"
#include <iostream>
#include <algorithm>
#include "QRCodeTables.hpp"

QRCode::QRCode(std::string data, CorrectionLevel ec)
: _data(std::move(data)), _ec(ec) {
    _mode = selectMode(_data);
    _version = selectVersion(_data, _mode, _ec);
}

void QRCode::generate() {
    std::cout << "generating qrcode for: " << _data << std::endl;
    if (_version == -1)
        std::cerr << "Error: Input data exceeds maximum capacity for any QR code version." << std::endl;
    else
        std::cout << "selected version: " << _version << std::endl;
}

void QRCode::save(const std::string &filepath) {
    std::cout << "saving qrcode into: " << filepath << std::endl;
}

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
