#pragma once
#include <cstdint>
#include <memory>
#include <vector>
#include "crypto.h"
#include "transaction.h"

class Wallet {
public:
    explicit Wallet(std::shared_ptr<Crypto> crypto);

    void generateNewKeypair();

    const std::vector<uint8_t>& publicKey() const { return pk_; }
    const std::vector<uint8_t>& secretKey() const { return sk_; }

    // Create and sign a transaction from this wallet to 'to_pubkey'
    Transaction createTransaction(const std::vector<uint8_t>& to_pubkey,
                                  uint64_t amount,
                                  uint64_t nonce) const;

private:
    std::shared_ptr<Crypto> crypto_;
    std::vector<uint8_t> pk_;
    std::vector<uint8_t> sk_;
};
