#include "QRCode.hpp"
#include <iostream>

QRCode::QRCode(std::string data, CorrectionLevel ec)
: _data(std::move(data)), _ec(ec) {
    _mode = selectMode(_data);
    _version = selectVersion(_data, _mode, _ec);
    if (_version != -1)
        _bits = encodeData();
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
