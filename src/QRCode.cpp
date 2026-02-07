#include "QRCode.hpp"
#include <iostream>
#include <algorithm>

QRCode::QRCode(const std::string &src) {
    _src = src;
}

QRCode::~QRCode() {}

void QRCode::generate() {
    std::cout << "generating qrcode for: " << _src << std::endl;
    _mode = identifyEncodingMode();
    std::cout << "encoding mode: " << EncodingModeToString(_mode) << std::endl;
}

void QRCode::save(const std::string &filepath) {
    std::cout << "saving qrcode into: " << filepath << std::endl;
}

EncodingMode QRCode::identifyEncodingMode() const {
    if (isNumeric(_src))        return EncodingMode::Numeric;
    if (isAlphanumeric(_src))   return EncodingMode::Alphanumeric;
    return EncodingMode::Binary;
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
