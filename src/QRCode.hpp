#pragma once

#include <string>

enum class EncodingMode {
    Numeric,
    Alphanumeric,
    Binary,
};

enum class CorrectionLevel {
    L = 7,
    M = 15,
    Q = 25,
    H = 30,
};

class QRCode {
    public:
        QRCode(const std::string &src);
        ~QRCode();

        void generate();
        void save(const std::string &filepath);
        EncodingMode identifyEncodingMode() const;

    private:
        std::string _src;
        EncodingMode _mode;
        CorrectionLevel _correctionLevel;
        int _version;

        std::string EncodingModeToString(EncodingMode mode) const;
        bool isNumeric(const std::string &string) const;
        bool isAlphanumeric(const std::string &string) const;
        
        const std::string_view alphanumericChars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ $%*+-./:";
};