#include <criterion/criterion.h>
#include "../src/QRCode.hpp"

Test(VersionSelection, smallest_version_numeric_M) {
    QRCode qr("123456789", CorrectionLevel::M);
    cr_assert_eq(qr.version(), 1);
}

Test(VersionSelection, smallest_version_alphanumeric_M) {
    QRCode qr("HELLO-WORLD", CorrectionLevel::M);
    cr_assert_eq(qr.version(), 1);
}

Test(VersionSelection, smallest_version_binary_M) {
    QRCode qr("Hello, World!", CorrectionLevel::M);
    cr_assert_eq(qr.version(), 1);
}

Test(VersionSelection, exceed_capacity_returns_minus_1) {
    std::string longString(5000, 'A');
    QRCode qr(longString, CorrectionLevel::M);
    cr_assert_eq(qr.version(), -1);
}

Test(VersionSelection, version_6_alphanumeric_Q) {
    std::string longString(108, 'A');
    QRCode qr(longString, CorrectionLevel::Q);
    cr_assert_eq(qr.version(), 6);
}

Test(VersionSelection, version_40_numeric_L) {
    std::string longString(7089, '1');
    QRCode qr(longString, CorrectionLevel::L);
    cr_assert_eq(qr.version(), 40);
}

Test(VersionSelection, higher_ec_bumps_version) {
    QRCode qrM("HELLO WORLD", CorrectionLevel::M);
    QRCode qrH("HELLO WORLD", CorrectionLevel::H);
    cr_assert_eq(qrM.version(), 1);
    cr_assert_eq(qrH.version(), 2);
}

Test(VersionSelection, exceed_capacity_empty_bits) {
    std::string longString(5000, 'A');
    QRCode qr(longString, CorrectionLevel::M);
    cr_assert(qr.bits().empty());
}