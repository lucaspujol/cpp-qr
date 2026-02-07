#pragma once

#include <string>

class QRCode {
    public:
        QRCode(const std::string &src);
        ~QRCode();

        void generate();
        void save(const std::string &filepath);

    private:
        std::string _src;
};