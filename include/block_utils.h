#pragma once
#include <array>
#include <cstdint>
#include <vector>

// Append 32-bit / 64-bit integers in little-endian form
void appendUint32(std::vector<uint8_t>& out, uint32_t v);
void appendUint64(std::vector<uint8_t>& out, uint64_t v);

// Simple non-cryptographic 32-byte hash (FNV-like)
// This is ONLY for our toy blockchain, not real security.
std::array<uint8_t, 32> simpleHash32(const std::vector<uint8_t>& data);
