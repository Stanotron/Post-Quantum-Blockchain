#pragma once
#include "crypto.h"
#include <string>
#include <cstddef>
#include <cstdint>

// Pull in your Hawk API; adjust the path as needed.
extern "C" {
#include "hawk.h"   // same include as your benchmark
}

class HawkCrypto : public Crypto {
public:
    explicit HawkCrypto(const std::string& variant);
    ~HawkCrypto() override = default;

    std::pair<std::vector<uint8_t>, std::vector<uint8_t>>
    generateKeypair() override;

    std::vector<uint8_t>
    sign(const std::vector<uint8_t>& msg,
         const std::vector<uint8_t>& sk) override;

    bool
    verify(const std::vector<uint8_t>& msg,
           const std::vector<uint8_t>& sig,
           const std::vector<uint8_t>& pk) override;

    std::string name() const override;
    std::string family() const override { return "Hawk"; }
    std::string variant() const override { return variant_; }

    size_t publicKeySize() const override;
    size_t secretKeySize() const override;
    size_t maxSignatureSize() const override;

private:
    std::string variant_;  // "512" (logn=9) or "1024" (logn=10)
    unsigned logn_;

    size_t priv_len_;
    size_t pub_len_;
    size_t sig_len_;
    size_t tmp_k_;
    size_t tmp_s_;
    size_t tmp_v_;
    size_t tmp_len_;

    // RNG context (same as your bench)
    shake_context rng_;
};
