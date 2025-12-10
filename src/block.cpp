#include "block.h"
#include "block_utils.h"

std::vector<uint8_t> serializeBlockForHash(const Block& block) {
    std::vector<uint8_t> out;

    appendUint32(out, block.index);
    // prev_hash (32 bytes)
    out.insert(out.end(), block.prev_hash.begin(), block.prev_hash.end());
    appendUint64(out, block.timestamp);

    // Transactions
    appendUint64(out, static_cast<uint64_t>(block.transactions.size()));
    for (const auto& tx : block.transactions) {
        // We include BOTH body and signature in the hash,
        // so changing any detail will change block_hash.
        std::vector<uint8_t> body = serializeTxForSigning(tx);

        // body length + body
        appendUint64(out, static_cast<uint64_t>(body.size()));
        out.insert(out.end(), body.begin(), body.end());

        // signature length + signature
        appendUint64(out, static_cast<uint64_t>(tx.signature.size()));
        out.insert(out.end(), tx.signature.begin(), tx.signature.end());
    }

    return out;
}

std::array<uint8_t, 32> computeBlockHash(const Block& block) {
    auto payload = serializeBlockForHash(block);
    return simpleHash32(payload);
}

std::vector<uint8_t> serializeFullBlock(const Block& block) {
    std::vector<uint8_t> out;

    appendUint32(out, block.index);
    out.insert(out.end(), block.prev_hash.begin(), block.prev_hash.end());
    appendUint64(out, block.timestamp);

    // block_hash itself
    out.insert(out.end(), block.block_hash.begin(), block.block_hash.end());

    // Transactions (same encoding as in serializeBlockForHash)
    appendUint64(out, static_cast<uint64_t>(block.transactions.size()));
    for (const auto& tx : block.transactions) {
        std::vector<uint8_t> body = serializeTxForSigning(tx);

        appendUint64(out, static_cast<uint64_t>(body.size()));
        out.insert(out.end(), body.begin(), body.end());

        appendUint64(out, static_cast<uint64_t>(tx.signature.size()));
        out.insert(out.end(), tx.signature.begin(), tx.signature.end());
    }

    return out;
}
