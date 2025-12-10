#include "wallet.h"

Wallet::Wallet(std::shared_ptr<Crypto> crypto)
    : crypto_(std::move(crypto)) {}

void Wallet::generateNewKeypair() {
    auto pair = crypto_->generateKeypair();
    pk_ = std::move(pair.first);
    sk_ = std::move(pair.second);
}

Transaction Wallet::createTransaction(const std::vector<uint8_t>& to_pubkey,
                                      uint64_t amount,
                                      uint64_t nonce) const {
    Transaction tx;
    tx.from_pubkey = pk_;
    tx.to_pubkey   = to_pubkey;
    tx.amount      = amount;
    tx.nonce       = nonce;

    // Sign the tx body
    std::vector<uint8_t> msg = serializeTxForSigning(tx);
    tx.signature = crypto_->sign(msg, sk_);

    return tx;
}
