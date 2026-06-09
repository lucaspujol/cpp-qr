#include "QRCode.hpp"
#include "QRCodeTables.hpp"

QRCode::Matrix QRCode::buildMatrix() {
    int size = 4 * _version + 17;
    _isFn = Matrix(size, std::vector<int8_t>(size, -1));
    Matrix m(size, std::vector<int8_t>(size, -1));

    placeFinderPattern(m, 0, 0);
    placeFinderPattern(m, 0, size - 7);
    placeFinderPattern(m, size - 7, 0);
    placeSeparators(m, size);
    placeTimingPatterns(m, size);
    placeAlignmentPatterns(m);
    placeDarkModule(m);
    reserveFormatInfo(m, size);
    placeData(m);
    _chosenMask = chooseMask(m);
    placeFormatInfo(m);

    return m;
}

void QRCode::placeFinderPattern(Matrix &m, int row, int col) {
    for (int r = 0; r < 7; r++) {
        for (int c = 0; c < 7; c++) {
            bool isBorder = (r == 0 || r == 6 || c == 0 || c == 6);
            bool isCenter = (r >= 2 && r <= 4 && c >= 2 && c <= 4);
            m[row + r][col + c]     = (isBorder || isCenter) ? 1 : 0;
            _isFn[row + r][col + c] = 1;
        }
    }
}

void QRCode::placeSeparators(Matrix &m, int size) {
    for (int i = 0; i < 8; i++) {
        if (_isFn[7][i] == -1)        { m[7][i] = 0;        _isFn[7][i] = 1; }
        if (_isFn[i][7] == -1)        { m[i][7] = 0;        _isFn[i][7] = 1; }
        if (_isFn[size-8][i] == -1)   { m[size-8][i] = 0;   _isFn[size-8][i] = 1; }
        if (_isFn[size-8+i][7] == -1) { m[size-8+i][7] = 0; _isFn[size-8+i][7] = 1; }
        if (_isFn[i][size-8] == -1)   { m[i][size-8] = 0;   _isFn[i][size-8] = 1; }
        if (_isFn[7][size-8+i] == -1) { m[7][size-8+i] = 0; _isFn[7][size-8+i] = 1; }
    }
}

void QRCode::placeTimingPatterns(Matrix &m, int size) {
    for (int i = 8; i < size - 8; i++) {
        int8_t value = (i % 2 == 0) ? 1 : 0;
        m[6][i] = value; _isFn[6][i] = 1;
        m[i][6] = value; _isFn[i][6] = 1;
    }
}

void QRCode::placeAlignmentPatterns(Matrix &m) {
    const int16_t *positions = ALIGNMENT_PATTERN_POSITIONS[_version - 1];

    std::vector<int> coords;
    for (int i = 0; i < 7 && positions[i] != -1; i++)
        coords.push_back(positions[i]);

    for (int r : coords) {
        for (int c : coords) {
            if (_isFn[r][c] != -1) continue;
            for (int dr = -2; dr <= 2; dr++) {
                for (int dc = -2; dc <= 2; dc++) {
                    bool isBorder = (dr == -2 || dr == 2 || dc == -2 || dc == 2);
                    bool isCenter = (dr == 0 && dc == 0);
                    m[r + dr][c + dc]     = (isBorder || isCenter) ? 1 : 0;
                    _isFn[r + dr][c + dc] = 1;
                }
            }
        }
    }
}

void QRCode::placeDarkModule(Matrix &m) {
    m[4 * _version + 9][8]     = 1;
    _isFn[4 * _version + 9][8] = 1;
}

void QRCode::reserveFormatInfo(Matrix &m, int size) {
    for (int i = 0; i <= 8; i++) {
        if (_isFn[8][i] == -1) { m[8][i] = 0; _isFn[8][i] = 1; }
        if (_isFn[i][8] == -1) { m[i][8] = 0; _isFn[i][8] = 1; }
    }
    for (int i = size - 8; i < size; i++) {
        if (_isFn[8][i] == -1) { m[8][i] = 0; _isFn[8][i] = 1; }
    }
    for (int i = size - 7; i < size; i++) {
        if (_isFn[i][8] == -1) { m[i][8] = 0; _isFn[i][8] = 1; }
    }
}

void QRCode::placeData(Matrix &m) {
    int size = 4 * _version + 17;
    int bitIndex = 0;
    bool goingUp = true;

    for (int right = size - 1; right >= 1; right -= 2) {
        int col = right;
        if (col == 6) col--;

        for (int i = 0; i < size; i++) {
            int row = goingUp ? (size - 1 - i) : i;

            if (_isFn[row][col] == -1 && bitIndex < static_cast<int>(_finalMessage.size())) {
                m[row][col] = (_finalMessage[bitIndex++] == '1') ? 1 : 0;
            }
            if (_isFn[row][col - 1] == -1 && bitIndex < static_cast<int>(_finalMessage.size())) {
                m[row][col - 1] = (_finalMessage[bitIndex++] == '1') ? 1 : 0;
            }
        }
        goingUp = !goingUp;
    }
}
