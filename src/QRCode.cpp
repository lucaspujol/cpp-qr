#include "QRCode.hpp"
#include <iostream>
#include <algorithm>
#include "QRCodeTables.hpp"

QRCode::QRCode(const std::string &src, CorrectionLevel correctionLevel)
: _src(src), _correctionLevel(correctionLevel) {
    _mode = identifyEncodingMode();
    _version = getVersion();
}

QRCode::~QRCode() {}

void QRCode::generate() {
    std::cout << "generating qrcode for: " << _src << std::endl;
    std::cout << "encoding mode: " << EncodingModeToString(_mode) << std::endl;
    if (_version == -1)
        std::cerr << "Error: Input data exceeds maximum capacity for any QR code version." << std::endl;
    else
        std::cout << "selected version: " << _version << std::endl;
}

void QRCode::save(const std::string &filepath) {
    std::cout << "saving qrcode into: " << filepath << std::endl;
}

EncodingMode QRCode::identifyEncodingMode() const {
    if (isNumeric(_src))        return EncodingMode::Numeric;
    if (isAlphanumeric(_src))   return EncodingMode::Alphanumeric;
    return EncodingMode::Binary;
}

int QRCode::getVersion() const {
    int charCount = _src.size();
    for (int version = 0; version < 40; version++) {
        int modeIndex = static_cast<int>(_mode);
        int ecIndex = static_cast<int>(_correctionLevel);

        if (charCount <= CHAR_CAPACITIES[version][ecIndex][modeIndex]) {
            return version + 1;
        }
    }
    return -1;
}

bool QRCode::isAlphanumeric(const std::string &string) const {
    if (string.empty()) return false;
    for (char c : string) {
        if (alphanumericChars.find(c) == std::string::npos)
            return false;
    }
    return true;
}

bool QRCode::isNumeric(const std::string &string) const {
    return !string.empty() &&
        std::all_of(string.begin(), string.end(), ::isdigit);
}

std::string QRCode::EncodingModeToString(EncodingMode mode) const {
    switch (mode) {
        case EncodingMode::Numeric: return "Numeric";
        case EncodingMode::Alphanumeric: return "Alphanumeric";
        case EncodingMode::Binary: return "Binary";
        default: return "Unknown";
    }
}
