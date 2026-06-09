#include "CLI.hpp"
#include <iostream>
#include <cstring>
#include <cstdlib>

void CLI::printUsage(const char *prog) {
    std::cout << "Usage: " << prog << " <data> [OPTIONS]\n"
              << "\nOptions:\n"
              << "  -o, --output <file>   Output PNG file (default: qr.png)\n"
              << "  -e, --ec <level>      Error correction level: L, M, Q, H (default: M)\n"
              << "  -h, --help            Show this help\n"
              << "\nExamples:\n"
              << "  " << prog << " \"https://example.com\"\n"
              << "  " << prog << " \"Hello world\" -o hello.png -e H\n";
}

CorrectionLevel CLI::parseEC(const char *s) {
    if (strcmp(s, "L") == 0) return CorrectionLevel::L;
    if (strcmp(s, "M") == 0) return CorrectionLevel::M;
    if (strcmp(s, "Q") == 0) return CorrectionLevel::Q;
    if (strcmp(s, "H") == 0) return CorrectionLevel::H;

    std::cerr << "Error: invalid EC level '" << s << "' — must be L, M, Q, or H\n";
    std::exit(1);
}

CLIArgs CLI::parse(int argc, char *argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        std::exit(1);
    }

    CLIArgs args;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            printUsage(argv[0]);
            std::exit(0);
        } else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
            if (++i >= argc) { std::cerr << "Error: --output requires a filename\n"; std::exit(1); }
            args.output = argv[i];
        } else if (strcmp(argv[i], "-e") == 0 || strcmp(argv[i], "--ec") == 0) {
            if (++i >= argc) { std::cerr << "Error: --ec requires a level (L/M/Q/H)\n"; std::exit(1); }
            args.ec = parseEC(argv[i]);
        } else if (argv[i][0] == '-') {
            std::cerr << "Error: unknown option '" << argv[i] << "'\n";
            printUsage(argv[0]);
            std::exit(1);
        } else {
            if (!args.data.empty()) {
                std::cerr << "Error: multiple data arguments - wrap data in quotes\n";
                std::exit(1);
            }
            args.data = argv[i];
        }
    }

    if (args.data.empty()) {
        std::cerr << "Error: no data provided\n";
        printUsage(argv[0]);
        std::exit(1);
    }

    return args;
}
