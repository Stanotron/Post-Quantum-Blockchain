#pragma once
#include <array>
#include <cstdint>
#include <vector>

// Append 32-bit / 64-bit integers in little-endian form
void appendUint32(std::vector<uint8_t>& out, uint32_t v);
void appendUint64(std::vector<uint8_t>& out, uint64_t v);

// 32-byte hash used for block hashing.
// Now implemented as SHA3-256 over the input bytes.
std::array<uint8_t, 32> simpleHash32(const std::vector<uint8_t>& data);
