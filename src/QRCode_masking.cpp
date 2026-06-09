#include "QRCode.hpp"
#include <limits>
#include <cmath>

// ============================================================================
// format info
// ============================================================================

void QRCode::placeFormatInfo(Matrix &m) {
    int ecBits;
    switch (_ec) {
        case CorrectionLevel::L: ecBits = 0b01; break;
        case CorrectionLevel::M: ecBits = 0b00; break;
        case CorrectionLevel::Q: ecBits = 0b11; break;
        case CorrectionLevel::H: ecBits = 0b10; break;
    }

    int data = (ecBits << 3) | _chosenMask;

    // BCH(15,5): generator = x^10+x^8+x^5+x^4+x^2+x+1 = 0x537
    int remainder = data << 10;
    for (int i = 14; i >= 10; i--)
        if (remainder & (1 << i))
            remainder ^= (0x537 << (i - 10));
    int formatStr = ((data << 10) | remainder) ^ 0x5412;

    int size = 4 * _version + 17;

    const int c1r[] = {8,8,8,8,8,8,8,8,7,5,4,3,2,1,0};
    const int c1c[] = {0,1,2,3,4,5,7,8,8,8,8,8,8,8,8};
    const int c2r[] = {size-1,size-2,size-3,size-4,size-5,size-6,size-7,8,8,8,8,8,8,8,8};
    const int c2c[] = {8,8,8,8,8,8,8,size-8,size-7,size-6,size-5,size-4,size-3,size-2,size-1};

    for (int i = 0; i < 15; i++) {
        int8_t bit = (formatStr >> (14 - i)) & 1;
        m[c1r[i]][c1c[i]] = bit; _isFn[c1r[i]][c1c[i]] = 1;
        m[c2r[i]][c2c[i]] = bit; _isFn[c2r[i]][c2c[i]] = 1;
    }
}

// ============================================================================
// masking
// ============================================================================

QRCode::Matrix QRCode::applyMask(const Matrix &m, int maskPattern) {
    int size = 4 * _version + 17;
    Matrix result = m;

    for (int row = 0; row < size; row++) {
        for (int col = 0; col < size; col++) {
            if (_isFn[row][col] == 1) continue;

            bool flip = false;
            switch (maskPattern) {
                case 0: flip = ((row + col) % 2 == 0); break;
                case 1: flip = (row % 2 == 0); break;
                case 2: flip = (col % 3 == 0); break;
                case 3: flip = ((row + col) % 3 == 0); break;
                case 4: flip = ((row / 2 + col / 3) % 2 == 0); break;
                case 5: flip = ((row * col) % 2 + (row * col) % 3 == 0); break;
                case 6: flip = (((row * col) % 2 + (row * col) % 3) % 2 == 0); break;
                case 7: flip = (((row + col) % 2 + (row * col) % 3) % 2 == 0); break;
            }
            if (flip) result[row][col] ^= 1;
        }
    }
    return result;
}

int QRCode::scoreRule1(const Matrix &m, int size) {
    int score = 0;
    for (int r = 0; r < size; r++) {
        for (int isCol = 0; isCol < 2; isCol++) {
            int run = 1;
            for (int i = 1; i < size; i++) {
                int8_t prev = isCol ? m[i-1][r] : m[r][i-1];
                int8_t curr = isCol ? m[i][r]   : m[r][i];
                if (curr == prev) {
                    run++;
                    if (run == 5)     score += 3;
                    else if (run > 5) score += 1;
                } else {
                    run = 1;
                }
            }
        }
    }
    return score;
}

int QRCode::scoreRule2(const Matrix &m, int size) {
    int score = 0;
    for (int r = 0; r < size - 1; r++)
        for (int c = 0; c < size - 1; c++)
            if (m[r][c] == m[r+1][c] && m[r][c] == m[r][c+1] && m[r][c] == m[r+1][c+1])
                score += 3;
    return score;
}

int QRCode::scoreRule3(const Matrix &m, int size) {
    int score = 0;
    const int pat1[] = {1,0,1,1,1,0,1,0,0,0,0};
    const int pat2[] = {0,0,0,0,1,0,1,1,1,0,1};

    for (int r = 0; r < size; r++) {
        for (int c = 0; c <= size - 11; c++) {
            bool match1 = true, match2 = true;
            for (int k = 0; k < 11; k++) {
                if (m[r][c+k] != pat1[k]) match1 = false;
                if (m[r][c+k] != pat2[k]) match2 = false;
            }
            if (match1 || match2) score += 40;
        }
    }
    for (int c = 0; c < size; c++) {
        for (int r = 0; r <= size - 11; r++) {
            bool match1 = true, match2 = true;
            for (int k = 0; k < 11; k++) {
                if (m[r+k][c] != pat1[k]) match1 = false;
                if (m[r+k][c] != pat2[k]) match2 = false;
            }
            if (match1 || match2) score += 40;
        }
    }
    return score;
}

int QRCode::scoreRule4(const Matrix &m, int size) {
    int dark = 0;
    for (int r = 0; r < size; r++)
        for (int c = 0; c < size; c++)
            if (m[r][c] == 1) dark++;
    int percent = dark * 100 / (size * size);
    return (std::abs(percent - 50) / 5) * 10;
}

int QRCode::scoreMask(const Matrix &m) {
    int size = 4 * _version + 17;
    return scoreRule1(m, size)
         + scoreRule2(m, size)
         + scoreRule3(m, size)
         + scoreRule4(m, size);
}

int QRCode::chooseMask(Matrix &m) {
    int bestMask = 0;
    int bestScore = std::numeric_limits<int>::max();
    for (int mask = 0; mask < 8; mask++) {
        Matrix candidate = applyMask(m, mask);
        int score = scoreMask(candidate);
        if (score < bestScore) {
            bestScore = score;
            bestMask = mask;
        }
    }
    m = applyMask(m, bestMask);
    return bestMask;
}
