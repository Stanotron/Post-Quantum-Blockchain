#include "transaction.h"
#include "block_utils.h"

std::vector<uint8_t> serializeTxForSigning(const Transaction& tx) {
    std::vector<uint8_t> out;

    // from_pubkey (length + bytes)
    appendUint64(out, static_cast<uint64_t>(tx.from_pubkey.size()));
    out.insert(out.end(), tx.from_pubkey.begin(), tx.from_pubkey.end());

    // to_pubkey (length + bytes)
    appendUint64(out, static_cast<uint64_t>(tx.to_pubkey.size()));
    out.insert(out.end(), tx.to_pubkey.begin(), tx.to_pubkey.end());

    // amount + nonce
    appendUint64(out, tx.amount);
    appendUint64(out, tx.nonce);

    return out;
}
