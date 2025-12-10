#include <iostream>
#include <memory>
#include <vector>
#include <iomanip>
#include <sstream>

#include "algo_config.h"
#include "crypto_factory.h"
#include "crypto.h"
#include "timing.h"
#include "wallet.h"
#include "blockchain.h"
#include "block.h"
#include "transaction.h"

// Helper: convert bytes to hex string
std::string toHex(const uint8_t* data, size_t len, size_t maxLen = 64) {
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    size_t shown = std::min(len, maxLen);
    for (size_t i = 0; i < shown; ++i) {
        oss << std::setw(2) << static_cast<int>(data[i]);
    }
    if (len > maxLen) {
        oss << "...";
    }
    return oss.str();
}

std::string hashToHex(const std::array<uint8_t, 32>& h) {
    return toHex(h.data(), h.size());
}

int main() {
    // 1. Select algorithm
    AlgoConfig cfg = getSelectedAlgorithm();
    auto crypto = createCrypto(cfg);

    std::cout << "=== Blockchain benchmark ===\n";
    std::cout << "Algorithm: " << crypto->name()
              << " (family=" << crypto->family()
              << ", variant=" << crypto->variant() << ")\n\n";

    // 2. Keygen benchmark: 100 wallets
    const size_t WALLET_COUNT = 100;
    uint64_t keygen_total_us = 0;

    std::cout << "[Keygen] Generating " << WALLET_COUNT
              << " wallets to measure keygen performance...\n";

    for (size_t i = 0; i < WALLET_COUNT; ++i) {
        auto t1 = nowMicros();
        auto [pk, sk] = crypto->generateKeypair();
        auto t2 = nowMicros();
        keygen_total_us += (t2 - t1);
        (void)pk; (void)sk; // not used further here
    }

    double keygen_avg_us = static_cast<double>(keygen_total_us) / WALLET_COUNT;
    std::cout << "Wallet keygen count: " << WALLET_COUNT << "\n";
    std::cout << "Avg wallet keygen time: " << keygen_avg_us << " us\n\n";

    // 3. Create blockchain and Alice/Bob wallets
    Blockchain chain(crypto);

    Wallet alice(crypto);
    Wallet bob(crypto);
    alice.generateNewKeypair();
    bob.generateNewKeypair();

    std::cout << "[Wallets] Alice PK size: " << alice.publicKey().size() << " bytes\n";
    std::cout << "[Wallets] Bob   PK size: " << bob.publicKey().size()   << " bytes\n\n";

    // 4. Create and sign 100 transactions: Alice -> Bob, amounts 1..100
    const size_t TX_PER_BLOCK = 1000;
    std::vector<Transaction> txs;
    txs.reserve(TX_PER_BLOCK);

    uint64_t sign_total_us = 0;

    std::cout << "[Tx] Creating and signing " << TX_PER_BLOCK
              << " transactions (Alice -> Bob)...\n";

    for (uint64_t i = 1; i <= TX_PER_BLOCK; ++i) {
        auto t1 = nowMicros();
        Transaction tx = alice.createTransaction(bob.publicKey(), i, i); // amount = nonce = i
        auto t2 = nowMicros();
        sign_total_us += (t2 - t1);
        txs.push_back(std::move(tx));
    }

    double sign_avg_us = static_cast<double>(sign_total_us) / TX_PER_BLOCK;
    std::cout << "Transactions per block: " << TX_PER_BLOCK << "\n";
    std::cout << "Avg tx sign time (in block context): " << sign_avg_us << " us\n\n";

    // 5. Build block 1
    Block block1 = chain.createBlockWithTransactions(txs);

    std::cout << "[Block] Block index: " << block1.index << "\n";
    std::cout << "[Block] Tx count:    " << block1.transactions.size() << "\n";
    std::cout << "[Block] Prev hash:   " << hashToHex(block1.prev_hash) << "\n";
    std::cout << "[Block] Block hash:  " << hashToHex(block1.block_hash) << "\n\n";

    // 5a. Inspect first transaction in detail
    if (!block1.transactions.empty()) {
        const Transaction& t0 = block1.transactions.front();
        std::vector<uint8_t> body0 = serializeTxForSigning(t0);

        std::cout << "[Tx0] from_pubkey size: " << t0.from_pubkey.size() << " bytes\n";
        std::cout << "[Tx0] to_pubkey size:   " << t0.to_pubkey.size()   << " bytes\n";
        std::cout << "[Tx0] amount:           " << t0.amount << "\n";
        std::cout << "[Tx0] nonce:            " << t0.nonce  << "\n";
        std::cout << "[Tx0] body length:      " << body0.size() << " bytes\n";
        std::cout << "[Tx0] signature length: " << t0.signature.size() << " bytes\n";
        std::cout << "[Tx0] body (first 64 bytes hex): "
                  << toHex(body0.data(), body0.size()) << "\n";
        std::cout << "[Tx0] sig  (first 64 bytes hex): "
                  << toHex(t0.signature.data(), t0.signature.size()) << "\n\n";
    }

    // 5b. Compute serialized block size
    std::vector<uint8_t> serialized = serializeFullBlock(block1);
    size_t block_size_bytes = serialized.size();
    double avg_tx_size_bytes = static_cast<double>(block_size_bytes) / TX_PER_BLOCK;

    std::cout << "[Block] Serialized block size: " << block_size_bytes << " bytes\n";
    std::cout << "[Block] Avg tx size (block_size / tx_count): "
              << avg_tx_size_bytes << " bytes\n";
    std::cout << "[Block] First 64 bytes of serialized block (hex): "
              << toHex(serialized.data(), serialized.size()) << "\n\n";

    // 6. Sanity check: single validation
    std::cout << "[Validate] Running single validateBlock(block1)...\n";
    bool ok_single = chain.validateBlock(block1);
    std::cout << "[Validate] Result: " << (ok_single ? "OK" : "FAIL") << "\n\n";
    if (!ok_single) {
        std::cerr << "Block validation failed unexpectedly. Aborting.\n";
        return 1;
    }

    // 7. Tampering test: flip one byte in the first tx signature and check that validation fails
    if (!block1.transactions.empty() && !block1.transactions[0].signature.empty()) {
        Block tampered = block1;
        tampered.transactions[0].signature[0] ^= 0x01; // flip 1 bit in signature

        std::cout << "[Tamper] Flipped 1 bit of first tx's signature.\n";
        bool ok_tampered = chain.validateBlock(tampered);
        std::cout << "[Tamper] validateBlock(tampered) result: "
                  << (ok_tampered ? "OK (unexpected!)" : "FAIL (as expected)") << "\n\n";
    }

    // 8. Block verification benchmark (many iterations)
    const size_t BLOCK_VERIFY_ITERS = 100;
    uint64_t block_verify_total_us = 0;

    std::cout << "[Benchmark] Running " << BLOCK_VERIFY_ITERS
              << " block validation iterations...\n";

    for (size_t i = 0; i < BLOCK_VERIFY_ITERS; ++i) {
        auto t1 = nowMicros();
        bool ok = chain.validateBlock(block1);
        auto t2 = nowMicros();

        if (!ok) {
            std::cerr << "Block validation FAILED at iteration " << i << "\n";
            return 1;
        }

        block_verify_total_us += (t2 - t1);
    }

    double block_verify_avg_us =
        static_cast<double>(block_verify_total_us) / BLOCK_VERIFY_ITERS;
    double verify_per_tx_us =
        block_verify_avg_us / TX_PER_BLOCK;

    std::cout << "[Benchmark] Avg block verify time:   "
              << block_verify_avg_us << " us\n";
    std::cout << "[Benchmark] Avg verify time per tx:  "
              << verify_per_tx_us << " us\n";

    return 0;
}
