#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include "QRCode.hpp"
#include <iostream>
#include <vector>

void QRCode::save(const std::string &filepath) {
    if (!isValid()) {
        std::cerr << "Error: Cannot save invalid QR code" << std::endl;
        return;
    }
    if (_matrix.empty()) {
        std::cerr << "Error: Call generate() before save()" << std::endl;
        return;
    }

    const int scale = 10;
    const int quiet = 4;

    int size = 4 * _version + 17;
    int imgSize = (size + 2 * quiet) * scale;

    std::vector<uint8_t> pixels(imgSize * imgSize, 255);

    for (int r = 0; r < size; r++) {
        for (int c = 0; c < size; c++) {
            uint8_t color = (_matrix[r][c] == 1) ? 0 : 255;
            int pr = (r + quiet) * scale;
            int pc = (c + quiet) * scale;
            for (int dy = 0; dy < scale; dy++)
                for (int dx = 0; dx < scale; dx++)
                    pixels[(pr + dy) * imgSize + (pc + dx)] = color;
        }
    }

    stbi_write_png(filepath.c_str(), imgSize, imgSize, 1, pixels.data(), imgSize);
}
