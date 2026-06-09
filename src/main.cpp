#include <iostream>
#include "CLI.hpp"
#include "QRCode.hpp"

int main(int argc, char *argv[]) {
    CLIArgs args = CLI::parse(argc, argv);
    QRCode qr(args.data, args.ec);
    qr.generate();
    qr.save(args.output);
    std::cout << "Saved: " << args.output << "\n";
    return 0;
}
