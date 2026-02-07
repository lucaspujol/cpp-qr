#include <criterion/criterion.h>
#include "../src/QRCode.hpp"

Test(VersionSelection, test_version_selection_numeric) {
    QRCode qr("123456789", CorrectionLevel::M);
    cr_assert_eq(qr.getVersion(), 1);
}

Test(VersionSelection, test_version_selection_alphanumeric) {
    QRCode qr("HELLO-WORLD", CorrectionLevel::M);
    cr_assert_eq(qr.getVersion(), 1);
}

Test(VersionSelection, test_version_selection_binary) {
    QRCode qr("Hello, World!", CorrectionLevel::M);
    cr_assert_eq(qr.getVersion(), 1);
}

Test(VersionSelection, test_version_selection_exceed_capacity) {
    std::string longString(5000, 'A');
    QRCode qr(longString, CorrectionLevel::M);
    cr_assert_eq(qr.getVersion(), -1);
}

Test(VersionSelection, test_version_selection_version_6_alphanumeric_q) {
    std::string longString(108, 'A');
    QRCode qr(longString, CorrectionLevel::Q);
    cr_assert_eq(qr.getVersion(), 6);
}

Test(VersionSelection, test_version_selection_maximum_version_numeric_L) {
    std::string longString(7089, '1');
    QRCode qr(longString, CorrectionLevel::L);
    cr_assert_eq(qr.getVersion(), 40);
}