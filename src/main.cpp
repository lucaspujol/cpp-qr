#include <iostream>
#include "QRCode.hpp"

int main() {
    QRCode qr("Hello, World!");
    qr.generate();
    return 0;
}
