#include <iostream>
#include <random>
#include <memory>
#include <vector>
#include "algo_config.h"
#include "crypto.h"
#include "crypto_factory.h"
#include "timing.h"

int main() {
    AlgoConfig cfg = getSelectedAlgorithm();
    auto crypto = createCrypto(cfg);

    std::cout << "Algorithm: " << crypto->name()
              << " (family=" << crypto->family()
              << ", variant=" << crypto->variant() << ")\n";

    // --------- 1) Single keygen for sizes + sanity ---------
    auto [pk, sk] = crypto->generateKeypair();

    std::cout << "PK size: " << pk.size() << " bytes\n";
    std::cout << "SK size: " << sk.size() << " bytes\n";
    std::cout << "Max signature size: " << crypto->maxSignatureSize() << " bytes\n";

    // --------- 2) Keygen benchmark ---------
    const size_t KEYGEN_ITERS = 1000;   // you can set to 10000 if you want
    uint64_t keygen_total_us = 0;

    for (size_t i = 0; i < KEYGEN_ITERS; ++i) {
        auto t1 = nowMicros();
        auto [pk_tmp, sk_tmp] = crypto->generateKeypair();
        auto t2 = nowMicros();
        keygen_total_us += (t2 - t1);
    }

    double keygen_avg_us = double(keygen_total_us) / KEYGEN_ITERS;
    std::cout << "Keygen iterations: " << KEYGEN_ITERS << "\n";
    std::cout << "Avg keygen time:   " << keygen_avg_us << " us\n";

    // --------- 3) Single sign/verify sanity check ---------
    std::vector<uint8_t> msg = {'h','e','l','l','o'};
    auto sig = crypto->sign(msg, sk);
    bool ok = crypto->verify(msg, sig, pk);
    std::cout << "Single sign/verify: " << (ok ? "OK" : "FAIL") << "\n";

    if (!ok) {
        std::cerr << "Aborting, algo not working correctly.\n";
        return 1;
    }

    // --------- 4) Sign/verify benchmark ---------
    const size_t ITERS = 10000;   // same as before
    std::mt19937_64 rng(42);
    std::uniform_int_distribution<unsigned int> dist(0, 255);

    std::vector<uint8_t> bench_msg(64);

    uint64_t sign_total_us   = 0;
    uint64_t verify_total_us = 0;

    for (size_t i = 0; i < ITERS; ++i) {
        for (auto &b : bench_msg) {
            b = static_cast<uint8_t>(dist(rng));
        }

        auto t1 = nowMicros();
        auto s  = crypto->sign(bench_msg, sk);
        auto t2 = nowMicros();
        bool ok2 = crypto->verify(bench_msg, s, pk);
        auto t3 = nowMicros();

        if (!ok2) {
            std::cerr << "Verify failed at iteration " << i << "\n";
            return 1;
        }

        sign_total_us   += (t2 - t1);
        verify_total_us += (t3 - t2);
    }

    double sign_avg_us   = double(sign_total_us)   / ITERS;
    double verify_avg_us = double(verify_total_us) / ITERS;

    std::cout << "Iterations:   " << ITERS << "\n";
    std::cout << "Avg sign time:   " << sign_avg_us   << " us\n";
    std::cout << "Avg verify time: " << verify_avg_us << " us\n";

    return 0;
}
