#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <cstdint>

enum class EncodingMode {
    Numeric,
    Alphanumeric,
    Binary,
};

// L=7%, M=15%, Q=25%, H=30% of codewords recoverable. Higher = larger QR.
enum class CorrectionLevel {
    L, M, Q, H,
};

class QRCode {
    public:
        QRCode(std::string data, CorrectionLevel ec = CorrectionLevel::M);
        ~QRCode() = default;

        void generate();
        void save(const std::string &filepath);

        // Getters
        bool isValid() const noexcept         { return _version != -1; }
        EncodingMode mode() const noexcept    { return _mode; }
        int version() const noexcept          { return _version; }
        std::string data() const noexcept     { return _data; }
        std::string bits() const noexcept     { return _bits; }
        std::string finalMessage() const noexcept { return _finalMessage; }
        int dataBlockCount() const noexcept   { return _ecResult.dataBlocks.size(); }
        int ecBlockCount() const noexcept     { return _ecResult.ecBlocks.size(); }
        std::vector<uint8_t> ecBlock(int i) const noexcept   { return _ecResult.ecBlocks[i]; }
        std::vector<uint8_t> dataBlock(int i) const noexcept { return _ecResult.dataBlocks[i]; }

    private:
        // types

        using Matrix = std::vector<std::vector<int8_t>>;

        struct ECResult {
            std::vector<std::vector<uint8_t>> dataBlocks;
            std::vector<std::vector<uint8_t>> ecBlocks;
        };

        // constants

        static constexpr std::string_view alphanumericChars =
            "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ $%*+-./:";

        static constexpr int REMAINDER_BITS[40] = {
            0, 7, 7, 7, 7, 7, 0, 0, 0, 0, 0, 0, 0,
            3, 3, 3, 3, 3, 3, 3,
            4, 4, 4, 4, 4, 4, 4,
            3, 3, 3, 3, 3, 3, 3,
            0, 0, 0, 0, 0, 0
        };

        // mode and version selection

        static EncodingMode selectMode(std::string_view string) noexcept;
        static int selectVersion(std::string_view string, EncodingMode mode, CorrectionLevel ec);
        static bool isNumeric(std::string_view string) noexcept;
        static bool isAlphanumeric(std::string_view string) noexcept;

        // data encoding

        std::string encodeData();
        void addModePrefix(std::string &encoded);
        void addCharCountIndicator(std::string &encoded);
        void addEncodedData(std::string &encoded);
        void addPadding(std::string &encoded);
        void encodeBinaryData(std::string &encoded);
        void encodeAlphanumericData(std::string &encoded);
        void encodeNumericData(std::string &encoded);
        static int getCharCountBits(int version, EncodingMode mode);

        // error correction coding

        ECResult errorCorrectionCoding();
        uint8_t gf256Multiply(uint8_t a, uint8_t b);
        std::vector<uint8_t> buildGeneratorPolynomial(int n);
        std::vector<uint8_t> generateECCodewords(
            const std::vector<uint8_t> &dataCodewords,
            const std::vector<uint8_t> &generator
        );
        std::string structureFinalMessage(const ECResult &ecResult);

        // matrix construction

        Matrix buildMatrix();

        // fields

        std::string     _data;
        CorrectionLevel _ec;
        EncodingMode    _mode;
        int             _version;
        std::string     _bits;
        ECResult        _ecResult;
        std::string     _finalMessage;
        Matrix          _matrix;
        Matrix          _isFn;
};
