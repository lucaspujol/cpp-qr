#pragma once

#include <string>
#include "QRCode.hpp"

struct CLIArgs {
    std::string data;
    std::string output = "qr.png";
    CorrectionLevel ec = CorrectionLevel::M;
};

class CLI {
public:
    static CLIArgs parse(int argc, char *argv[]);

private:
    static void printUsage(const char *prog);
    static CorrectionLevel parseEC(const char *s);
};
