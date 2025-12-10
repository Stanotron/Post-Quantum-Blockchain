#pragma once
#include <cstdint>
#include <vector>

// Simple account-style transaction
struct Transaction {
    std::vector<uint8_t> from_pubkey;
    std::vector<uint8_t> to_pubkey;
    uint64_t amount = 0;
    uint64_t nonce  = 0;
    std::vector<uint8_t> signature; // filled after signing
};

// Serialize the transaction fields that are covered by the signature
// (i.e., WITHOUT the signature itself).
std::vector<uint8_t> serializeTxForSigning(const Transaction& tx);
