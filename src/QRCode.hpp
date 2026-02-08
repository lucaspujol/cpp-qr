#pragma once

#include <string>
#include <string_view>

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
        QRCode(std::string data, CorrectionLevel ec = CorrectionLevel::M);
        ~QRCode() = default;

        void generate();
        void save(const std::string &filepath);

        EncodingMode mode() const noexcept   { return _mode; }
        int version() const noexcept         { return _version; }
        std::string data() const noexcept    { return _data; }

    private:
        static constexpr std::string_view alphanumericChars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ $%*+-./:";
        static EncodingMode selectMode(std::string_view string) noexcept;
        static int selectVersion(
            std::string_view string,
            EncodingMode mode,
            CorrectionLevel ec
        );
        std::string encodeData();
        void addModePrefix(std::string &encoded);
        void addCharCountIndicator(std::string &encoded);
        void addEncodedData(std::string &encoded);
        void addPadding(std::string &encoded);

        void encodeBinaryData(std::string &encoded);
        void encodeAlphanumericData(std::string &encoded);
        void encodeNumericData(std::string &encoded);

        static bool isNumeric(std::string_view string) noexcept;
        static bool isAlphanumeric(std::string_view string) noexcept;
        static int getCharCountBits(int version, EncodingMode mode);

        std::string _data;
        CorrectionLevel _ec;
        EncodingMode _mode;
        int _version;
        std::string _bits;
};
