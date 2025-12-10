#include "block_utils.h"
#include <oqs/sha3.h> 

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
    std::array<uint8_t, 32> out{};

    // SHA3-256 from liboqs
    // void OQS_SHA3_sha3_256(uint8_t *output, const uint8_t *input, size_t inlen);
    if (!data.empty()) {
        OQS_SHA3_sha3_256(out.data(),
                          data.data(),
                          data.size());
    } else {
        // hash of empty input
        OQS_SHA3_sha3_256(out.data(),
                          nullptr,
                          0);
    }

    return out;
}