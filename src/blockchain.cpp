#include "blockchain.h"
#include "block_utils.h"
#include "transaction.h"

Blockchain::Blockchain(std::shared_ptr<Crypto> crypto)
    : crypto_(std::move(crypto)) {
    chain_.push_back(makeGenesisBlock());
}

Block Blockchain::makeGenesisBlock() const {
    Block g;
    g.index = 0;
    // prev_hash stays all-zero
    g.timestamp = 0; // hardcoded
    g.transactions.clear();
    g.block_hash = computeBlockHash(g);
    return g;
}

Block Blockchain::createBlockWithTransactions(const std::vector<Transaction>& txs) {
    Block b;
    b.index = latestBlock().index + 1;
    b.prev_hash = latestBlock().block_hash;
    // For simplicity: just use a dummy timestamp (e.g. 1, 2, 3...)
    b.timestamp = static_cast<uint64_t>(b.index);

    b.transactions = txs;
    b.block_hash = computeBlockHash(b);

    // We do NOT automatically push it into chain here,
    // so we can validate first if we want.
    return b;
}

bool Blockchain::validateBlock(const Block& block) const {
    // 1. Check linkage
    if (block.index != latestBlock().index + 1) {
        return false;
    }
    if (block.prev_hash != latestBlock().block_hash) {
        return false;
    }

    // 2. Check block hash
    auto recomputed = computeBlockHash(block);
    if (recomputed != block.block_hash) {
        return false;
    }

    // 3. Check all transaction signatures
    for (const auto& tx : block.transactions) {
        std::vector<uint8_t> msg = serializeTxForSigning(tx);
        bool ok = crypto_->verify(msg, tx.signature, tx.from_pubkey);
        if (!ok) {
            return false;
        }
    }

    return true;
}
