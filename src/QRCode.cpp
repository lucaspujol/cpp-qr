#include "QRCode.hpp"
#include <iostream>

QRCode::QRCode(const std::string &src) {
    _src = src;
}

QRCode::~QRCode() {}

void QRCode::generate() {
    std::cout << "generating qrcode for: " << _src << std::endl;
}

void QRCode::save(const std::string &filepath) {
    std::cout << "saving qrcode into: " << filepath << std::endl;
}
