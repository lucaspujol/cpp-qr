#pragma once

#include <string>

enum class EncodingMode {
    Numeric,
    Alphanumeric,
    Binary,
};

/**
 * Each correction level corresponds to a percentage of codewords
 * that can be restored:
 * - L: 7% 
 * - M: 15% 
 * - Q: 25% 
 * - H: 30% 
 * 
 * Be aware higher levels of error correction require more bytes,
 * so the higher the error correction level, the larger the QR code
 * will have to be.
 */
enum class CorrectionLevel {
    L, M, Q, H,
};

class QRCode {
    public:
        QRCode(const std::string &src, CorrectionLevel correctionLevel = CorrectionLevel::M);
        ~QRCode();

        void generate();
        void save(const std::string &filepath);

        EncodingMode identifyEncodingMode() const;
        int getVersion() const;

        std::string getData() const { return _src; }

    private:
        std::string _src;
        CorrectionLevel _correctionLevel;
        
        EncodingMode _mode;
        std::string EncodingModeToString(EncodingMode mode) const;
        bool isNumeric(const std::string &string) const;
        bool isAlphanumeric(const std::string &string) const;
        const std::string_view alphanumericChars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ $%*+-./:";
        
        int _version;
};