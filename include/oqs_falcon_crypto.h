#pragma once
#include "crypto.h"
#include <oqs/oqs.h>
#include <string>

class OqsFalconCrypto : public Crypto {
public:
    explicit OqsFalconCrypto(const std::string& variant);
    ~OqsFalconCrypto() override;

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
    std::string family() const override { return "Falcon"; }
    std::string variant() const override { return variant_; }

    size_t publicKeySize() const override;
    size_t secretKeySize() const override;
    size_t maxSignatureSize() const override;

private:
    std::string variant_;      // "512", "1024"
    std::string oqs_alg_name_; // OQS_SIG_alg_falcon_512 etc.
    OQS_SIG* sig_ = nullptr;
};
