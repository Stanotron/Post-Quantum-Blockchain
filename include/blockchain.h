#pragma once
#include <memory>
#include <vector>
#include "block.h"
#include "crypto.h"

class Blockchain {
public:
    explicit Blockchain(std::shared_ptr<Crypto> crypto);

    const Block& genesisBlock() const { return chain_.front(); }
    const Block& latestBlock() const { return chain_.back(); }

    // Create a new block on top of the latest one (for us: block 1)
    Block createBlockWithTransactions(const std::vector<Transaction>& txs);

    // Validate a block: linkage + hash + all transaction signatures.
    bool validateBlock(const Block& block) const;

private:
    Block makeGenesisBlock() const;

    std::vector<Block> chain_;
    std::shared_ptr<Crypto> crypto_;
};
