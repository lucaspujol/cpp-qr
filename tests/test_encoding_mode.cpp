#include <criterion/criterion.h>
#include "../src/QRCode.hpp"

Test(EncodingMode, numeric_pure_digits) {
    QRCode qr("123456789");
    cr_assert_eq(qr.mode(), EncodingMode::Numeric);
}

Test(EncodingMode, numeric_single_digit) {
    QRCode qr("0");
    cr_assert_eq(qr.mode(), EncodingMode::Numeric);
}

Test(EncodingMode, alphanumeric_uppercase_with_special) {
    QRCode qr("HELLO-WORLD");
    cr_assert_eq(qr.mode(), EncodingMode::Alphanumeric);
}

Test(EncodingMode, alphanumeric_with_space) {
    QRCode qr("HELLO WORLD");
    cr_assert_eq(qr.mode(), EncodingMode::Alphanumeric);
}

Test(EncodingMode, alphanumeric_special_chars) {
    QRCode qr("$%*+-./:");
    cr_assert_eq(qr.mode(), EncodingMode::Alphanumeric);
}

Test(EncodingMode, binary_lowercase_forces_byte) {
    QRCode qr("Hello, World!");
    cr_assert_eq(qr.mode(), EncodingMode::Binary);
}

Test(EncodingMode, binary_single_lowercase) {
    QRCode qr("a");
    cr_assert_eq(qr.mode(), EncodingMode::Binary);
}

Test(EncodingMode, numeric_preferred_over_alphanumeric) {
    QRCode qr("999");
    cr_assert_eq(qr.mode(), EncodingMode::Numeric);
}
