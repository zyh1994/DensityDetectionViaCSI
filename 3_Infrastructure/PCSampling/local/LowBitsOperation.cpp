#include "LowBitsOperation.h"
#include <cassert>


bool is_big_endian() {
    union {
        uint32_t i;
        char c[4];
    } endian = {0x01020304};

    return endian.c[0] == 1;
}


int convert_negative_data(uint32_t data, int maxbit) {
    // First, we use maxbit to determine whether the highest bit is 1 or not
    // If the highest bit is 1, then it is a negative number
    int result = 0;
    if (data & (1 << (maxbit - 1))) {
        // If the 1 << maxbit is greater than the maximum value of int32_t,
        // then we need to use int64_t
        result = static_cast<int64_t>(data) - (static_cast<int64_t>(1) << maxbit);
    }
    else {
        // positive
        result = data;
    }
    return result;
};


void change_bytes_order(uint8_t* buf, const int32_t buf_size) {
    for (int i = 0; i < buf_size / 2; i++) {
        std::swap(buf[i], buf[buf_size - i - 1]);
    }
}


void write_16bits_by_little_endian(unsigned char* buf, int& idx, uint32_t data) {
    buf[idx++] = data & 0xFF;
    buf[idx++] = (data >> 8) & 0xFF;
};


void write_16bits_by_big_endian(unsigned char* buf, int& idx, uint32_t data) {
    buf[idx++] = (data >> 8) & 0xFF;
    buf[idx++] = data & 0xFF;
};


void write_32bits_by_little_endian(unsigned char* buf, int& idx, uint32_t data) {
    buf[idx++] = data & 0xFF;
    buf[idx++] = (data >> 8) & 0xFF;
    buf[idx++] = (data >> 16) & 0xFF;
    buf[idx++] = (data >> 24) & 0xFF;
};


void write_32bits_by_big_endian(unsigned char* buf, int& idx, uint32_t data) {
    buf[idx++] = (data >> 24) & 0xFF;
    buf[idx++] = (data >> 16) & 0xFF;
    buf[idx++] = (data >> 8) & 0xFF;
    buf[idx++] = data & 0xFF;
};


void write_64bits_by_big_endian(unsigned char* buf, int& idx, uint64_t data) {
    buf[idx++] = (data >> 56) & 0xFF;
    buf[idx++] = (data >> 48) & 0xFF;
    buf[idx++] = (data >> 40) & 0xFF;
    buf[idx++] = (data >> 32) & 0xFF;
    buf[idx++] = (data >> 24) & 0xFF;
    buf[idx++] = (data >> 16) & 0xFF;
    buf[idx++] = (data >> 8) & 0xFF;
    buf[idx++] = data & 0xFF;
};


uint64_t read_64bits_by_little_endian(const unsigned char* buf, int& idx) {
    uint64_t result = static_cast<uint64_t>(buf[idx]) | (static_cast<uint64_t>(buf[idx + 1]) << 8) |
        (static_cast<uint64_t>(buf[idx + 2]) << 16) | (static_cast<uint64_t>(buf[idx + 3]) << 24) |
        (static_cast<uint64_t>(buf[idx + 4]) << 32) | (static_cast<uint64_t>(buf[idx + 5]) << 40) |
        (static_cast<uint64_t>(buf[idx + 6]) << 48) | (static_cast<uint64_t>(buf[idx + 7]) << 56);
    idx += 8;
    return result;
}


uint64_t read_64bits_by_big_endian(const unsigned char* buf, int& idx) {
    uint64_t result = (static_cast<uint64_t>(buf[idx]) << 56) | (static_cast<uint64_t>(buf[idx + 1]) << 48) |
        (static_cast<uint64_t>(buf[idx + 2]) << 40) | (static_cast<uint64_t>(buf[idx + 3]) << 32) |
        (static_cast<uint64_t>(buf[idx + 4]) << 24) | (static_cast<uint64_t>(buf[idx + 5]) << 16) |
        (static_cast<uint64_t>(buf[idx + 6]) << 8) | static_cast<uint64_t>(buf[idx + 7]);
    idx += 8;
    return result;
};


void write_64bits_by_little_endian(unsigned char* buf, int& idx, uint64_t data) {
    buf[idx++] = data & 0xFF;
    buf[idx++] = (data >> 8) & 0xFF;
    buf[idx++] = (data >> 16) & 0xFF;
    buf[idx++] = (data >> 24) & 0xFF;
    buf[idx++] = (data >> 32) & 0xFF;
    buf[idx++] = (data >> 40) & 0xFF;
    buf[idx++] = (data >> 48) & 0xFF;
    buf[idx++] = (data >> 56) & 0xFF;
}


uint32_t read_16bits_by_little_endian(const unsigned char* buf, int& idx) {
    uint32_t result = buf[idx] | (buf[idx + 1] << 8);
    idx += 2;
    return result;
};


uint32_t read_16bits_by_big_endian(const unsigned char* buf, int& idx) {
    uint32_t result = (buf[idx] << 8) | buf[idx + 1];
    idx += 2;
    return result;
};


uint32_t read_32bits_by_little_endian(const unsigned char* buf, int& idx) {
    uint32_t result = buf[idx] | (buf[idx + 1] << 8) | (buf[idx + 2] << 16) | (buf[idx + 3] << 24);
    idx += 4;
    return result;
};


uint32_t read_32bits_by_big_endian(const unsigned char* buf, int& idx) {
    uint32_t result = (buf[idx] << 24) | (buf[idx + 1] << 16) | (buf[idx + 2] << 8) | buf[idx + 3];
    idx += 4;
    return result;
};


uint32_t merge_big_endian(uint32_t int1, uint32_t bits_left1, uint32_t int2, uint32_t bits_left2) {
    if (bits_left1 + bits_left2 > 32) {
        uint64_t temp = static_cast<uint64_t>(int1) << bits_left2;
        temp |= int2;
        return static_cast<uint32_t>(temp & 0xFFFFFFFF);
    }
    return (int1 << bits_left2) | int2;
};


uint32_t merge_little_endian(uint32_t int1, uint32_t bits_left1, uint32_t int2, uint32_t bits_left2) {
    if (bits_left1 + bits_left2 > 32) {
        uint64_t temp = static_cast<uint64_t>(int2) << bits_left1;
        temp |= int1;
        return static_cast<uint32_t>(temp & 0xFFFFFFFF);
    }
    return (int2 << bits_left1) | int1;
};


void split_bits_big_endian(uint32_t data, int16_t bits, uint32_t& extracted, uint32_t& remainder) {

    // Clamp the bits to 0 ~ 32
    bits = (bits > 32) ? 32 : (bits < 0) ? 0 : bits;

    // Suppose the data is 0xAABBCCDD, and the bits is 8
    // Then the extracted is 0xAA, and the remainder is 0xBBCCDD
    extracted = data >> (32 - bits);
    remainder = data & ((1 << (32 - bits)) - 1);
}


void split_bits_little_endian(uint32_t data, int16_t bits, uint32_t& extracted, uint32_t& remainder) {

    // Clamp the bits to 0 ~ 32
    bits = (bits > 32) ? 32 : (bits < 0) ? 0 : bits;

    // Suppose the data is 0xAABBCCDD, and the bits is 8
    // Then the extracted is 0xDD, and the remainder is 0xAABBCC
    extracted = data & ((1 << bits) - 1);
    remainder = data >> bits;
}


uint32_t bits_inverse(uint32_t data, int bits) {
    uint32_t result = ~data;
    return result & ((1 << bits) - 1);
}


void test_convert_negative_data() {
    assert(convert_negative_data(0b0100, 4) == 0b0100);
    assert(convert_negative_data(0b1100, 4) == -4);
    assert(convert_negative_data(0x80000000, 32) == 0x80000000);
    assert(convert_negative_data(0x7FFFFFFF, 32) == 0x7FFFFFFF);
}

void test_change_bytes_order() {
    uint8_t buf[4] = { 0x01, 0x02, 0x03, 0x04 };
    change_bytes_order(buf, 4);
    assert(buf[0] == 0x04 && buf[1] == 0x03 && buf[2] == 0x02 && buf[3] == 0x01);
}

void test_write_16bits_by_little_endian() {
    uint8_t buf[2];
    int idx = 0;
    write_16bits_by_little_endian(buf, idx, 0xABCD);
    assert(buf[0] == 0xCD && buf[1] == 0xAB);
}

void test_write_16bits_by_big_endian() {
    uint8_t buf[2];
    int idx = 0;
    write_16bits_by_big_endian(buf, idx, 0xABCD);
    assert(buf[0] == 0xAB && buf[1] == 0xCD);
}

void test_write_32bits_by_little_endian() {
    uint8_t buf[4];
    int idx = 0;
    write_32bits_by_little_endian(buf, idx, 0xAABBCCDD);
    assert(buf[0] == 0xDD && buf[1] == 0xCC && buf[2] == 0xBB && buf[3] == 0xAA);
}

void test_write_32bits_by_big_endian() {
    uint8_t buf[4];
    int idx = 0;
    write_32bits_by_big_endian(buf, idx, 0xAABBCCDD);
    assert(buf[0] == 0xAA && buf[1] == 0xBB && buf[2] == 0xCC && buf[3] == 0xDD);
}

void test_read_16bits_by_little_endian() {
    uint8_t buf[2] = { 0xCD, 0xAB };
    int idx = 0;
    assert(read_16bits_by_little_endian(buf, idx) == 0xABCD);
}

void test_read_16bits_by_big_endian() {
    uint8_t buf[2] = { 0xAB, 0xCD };
    int idx = 0;
    assert(read_16bits_by_big_endian(buf, idx) == 0xABCD);
}

void test_read_32bits_by_little_endian() {
    uint8_t buf[4] = { 0xDD, 0xCC, 0xBB, 0xAA };
    int idx = 0;
    assert(read_32bits_by_little_endian(buf, idx) == 0xAABBCCDD);
}

void test_read_32bits_by_big_endian() {
    uint8_t buf[4] = { 0xAA, 0xBB, 0xCC, 0xDD };
    int idx = 0;
    assert(read_32bits_by_big_endian(buf, idx) == 0xAABBCCDD);
}

void test_merge_big_endian_standard() {
    uint32_t int1 = 0xFF, int2 = 0xAA;
    uint32_t bits_left1 = 8, bits_left2 = 8;
    uint32_t mergedData = merge_big_endian(int1, bits_left1, int2, bits_left2);
    assert(mergedData == 0xFFAA);
}

void test_merge_little_endian_standard() {
    uint32_t int1 = 0xFF, int2 = 0xAA;
    uint32_t bits_left1 = 8, bits_left2 = 8;
    uint32_t mergedData = merge_little_endian(int1, bits_left1, int2, bits_left2);
    assert(mergedData == 0xAAFF);
}

void test_merge_big_endianness_exceeding_32bits() {
    uint32_t int1 = 0xFFFFFFFF, int2 = 0xAA;
    uint32_t bits_left1 = 32, bits_left2 = 8;
    uint32_t mergedData = merge_big_endian(int1, bits_left1, int2, bits_left2);
    assert(mergedData == 0xFFFFFFAA);
}

void test_merge_little_endianness_exceeding_32bits() {
    uint32_t int1 = 0xFFFFFFFF, int2 = 0xAA;
    uint32_t bits_left1 = 32, bits_left2 = 8;
    uint32_t mergedData = merge_little_endian(int1, bits_left1, int2, bits_left2);
    assert(mergedData == 0xFFFFFFFF);
}

void test_merge_with_zero_values() {
    uint32_t int1 = 0, int2 = 0xAA;
    uint32_t bits_left1 = 8, bits_left2 = 8;
    uint32_t mergedDataBig = merge_big_endian(int1, bits_left1, int2, bits_left2);
    uint32_t mergedDataLittle = merge_little_endian(int1, bits_left1, int2, bits_left2);
    assert(mergedDataBig == 0x00AA);
    assert(mergedDataLittle == 0xAA00);
}

void test_split_standard_big_endian() {
    uint32_t data = 0xAABBCCDD;
    uint32_t extracted, remainder;
    split_bits_big_endian(data, 8, extracted, remainder);
    assert(extracted == 0xAA && remainder == 0xBBCCDD);
}

void test_split_standard_little_endian() {
    uint32_t data = 0xAABBCCDD;
    uint32_t extracted, remainder;
    split_bits_little_endian(data, 8, extracted, remainder);
    assert(extracted == 0xDD && remainder == 0xAABBCC);
}

void test_split_zero_data() {
    uint32_t data = 0;
    uint32_t extracted, remainder;
    split_bits_big_endian(data, 16, extracted, remainder);
    assert(extracted == 0 && remainder == 0);
}

void test_split_extreme_bits() {
    uint32_t data = 0xFFAA;
    uint32_t extracted, remainder;
    split_bits_big_endian(data, 32, extracted, remainder);
    assert(extracted == 0xFFAA && remainder == 0);

    split_bits_little_endian(data, 32, extracted, remainder);
    assert(extracted == 0 && remainder == 0xFFAA);
}


//int main() {
//    test_convert_negative_data();
//    test_change_bytes_order();
//    test_write_16bits_by_little_endian();
//    test_write_16bits_by_big_endian();
//    test_write_32bits_by_little_endian();
//    test_write_32bits_by_big_endian();
//    test_read_16bits_by_little_endian();
//    test_read_16bits_by_big_endian();
//    test_read_32bits_by_little_endian();
//    test_read_32bits_by_big_endian();
//    test_merge_big_endian_standard();
//    test_merge_little_endian_standard();
//    test_merge_big_endianness_exceeding_32bits();
//    test_merge_little_endianness_exceeding_32bits();
//    test_merge_with_zero_values();
//    test_split_standard_big_endian();
//    test_split_standard_little_endian();
//    test_split_extreme_bits();
//    std::cout << "All tests passed!" << std::endl;
//    return 0;
//}

