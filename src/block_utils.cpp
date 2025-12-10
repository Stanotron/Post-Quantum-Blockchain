#include "block_utils.h"

void appendUint32(std::vector<uint8_t>& out, uint32_t v) {
    out.push_back(static_cast<uint8_t>(v & 0xFF));
    out.push_back(static_cast<uint8_t>((v >> 8) & 0xFF));
    out.push_back(static_cast<uint8_t>((v >> 16) & 0xFF));
    out.push_back(static_cast<uint8_t>((v >> 24) & 0xFF));
}

void appendUint64(std::vector<uint8_t>& out, uint64_t v) {
    for (int i = 0; i < 8; ++i) {
        out.push_back(static_cast<uint8_t>((v >> (8 * i)) & 0xFF));
    }
}

std::array<uint8_t, 32> simpleHash32(const std::vector<uint8_t>& data) {
    // 64-bit FNV-1a, expanded to 32 bytes
    const uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
    const uint64_t FNV_PRIME        = 1099511628211ULL;

    uint64_t h = FNV_OFFSET_BASIS;
    for (uint8_t b : data) {
        h ^= static_cast<uint64_t>(b);
        h *= FNV_PRIME;
    }

    std::array<uint8_t, 32> out{};
    // Derive 4x 64-bit words from h
    uint64_t h1 = h;
    uint64_t h2 = h ^ 0xA5A5A5A5A5A5A5A5ULL;
    uint64_t h3 = h + 0x1234567890ABCDEFULL;
    uint64_t h4 = h ^ 0x0F0F0F0F0F0F0F0FULL;

    uint64_t words[4] = { h1, h2, h3, h4 };
    for (int w = 0; w < 4; ++w) {
        for (int i = 0; i < 8; ++i) {
            out[w * 8 + i] = static_cast<uint8_t>((words[w] >> (8 * i)) & 0xFF);
        }
    }
    return out;
}
