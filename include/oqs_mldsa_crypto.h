#pragma once
#include "crypto.h"
#include <oqs/oqs.h>
#include <string>

class OqsMldsaCrypto : public Crypto {
public:
    explicit OqsMldsaCrypto(const std::string& variant);
    ~OqsMldsaCrypto() override;

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
    std::string family() const override { return "ML-DSA"; }
    std::string variant() const override { return variant_; }

    size_t publicKeySize() const override;
    size_t secretKeySize() const override;
    size_t maxSignatureSize() const override;

private:
    std::string variant_;       // "44", "65", "87"
    std::string oqs_alg_name_;  // "ML-DSA-44", etc.
    OQS_SIG* sig_ = nullptr;
};
