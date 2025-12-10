#pragma once
#include <cstdint>
#include <string>
#include <vector>

class Crypto {
public:
    virtual ~Crypto() = default;

    // (public, secret)
    virtual std::pair<std::vector<uint8_t>, std::vector<uint8_t>>
    generateKeypair() = 0;

    virtual std::vector<uint8_t>
    sign(const std::vector<uint8_t>& msg,
         const std::vector<uint8_t>& sk) = 0;

    virtual bool
    verify(const std::vector<uint8_t>& msg,
           const std::vector<uint8_t>& sig,
           const std::vector<uint8_t>& pk) = 0;

    // Metadata for reporting
    virtual std::string name() const = 0;    // e.g. "ML-DSA-44"
    virtual std::string family() const = 0;  // "ML-DSA", "Falcon", "Hawk"
    virtual std::string variant() const = 0; // "44", "512", etc.

    virtual size_t publicKeySize() const = 0;
    virtual size_t secretKeySize() const = 0;
    virtual size_t maxSignatureSize() const = 0;
};
