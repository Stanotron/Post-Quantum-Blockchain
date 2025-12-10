#pragma once
#include <array>
#include <cstdint>
#include <vector>
#include "transaction.h"

struct Block {
    uint32_t index = 0;
    std::array<uint8_t, 32> prev_hash{};  // hash of previous block
    uint64_t timestamp = 0;
    std::vector<Transaction> transactions;
    std::array<uint8_t, 32> block_hash{}; // hash of this block's contents
};

// Serialize the parts of the block that go into block_hash computation.
// This includes transactions and their signatures, but NOT block_hash itself.
std::vector<uint8_t> serializeBlockForHash(const Block& block);

// Compute the block hash using our simpleHash32().
std::array<uint8_t, 32> computeBlockHash(const Block& block);

// Serialize the full block (including block_hash) to measure its size.
std::vector<uint8_t> serializeFullBlock(const Block& block);
