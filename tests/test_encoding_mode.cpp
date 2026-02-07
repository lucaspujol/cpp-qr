#include <criterion/criterion.h>
#include "../src/QRCode.hpp"

Test(EncodingMode, test_encoding_mode_numeric_value) {
    QRCode qr("123456789");
    cr_assert_eq(qr.identifyEncodingMode(), EncodingMode::Numeric);
}

Test(EncodingMode, test_encoding_mode_alphanumeric_value) {
    QRCode qr("HELLO-WORLD");
    cr_assert_eq(qr.identifyEncodingMode(), EncodingMode::Alphanumeric);
}

Test(EncodingMode, test_encoding_mode_binary_value) {
    QRCode qr("Hello, World!");
    cr_assert_eq(qr.identifyEncodingMode(), EncodingMode::Binary);
}
