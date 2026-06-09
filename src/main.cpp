#include <iostream>
#include "QRCode.hpp"

int main() {
    std::string data = "https://www.thonky.com/qr-code-tutorial/introduction";
    QRCode qr(data);
    qr.generate();
    qr.save("thonky.png");
    return 0;
}
