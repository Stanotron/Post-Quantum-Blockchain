#include "crypto_factory.h"
#include "oqs_mldsa_crypto.h"
#include "oqs_falcon_crypto.h"
#include "hawk_crypto.h"
#include <stdexcept>

std::shared_ptr<Crypto> createCrypto(const AlgoConfig& cfg) {
    switch (cfg.family) {
    case AlgoFamily::ML_DSA:
        return std::make_shared<OqsMldsaCrypto>(cfg.variant);
    case AlgoFamily::FALCON:
        return std::make_shared<OqsFalconCrypto>(cfg.variant);
    case AlgoFamily::HAWK:
        return std::make_shared<HawkCrypto>(cfg.variant);
    default:
        throw std::runtime_error("Unsupported family in createCrypto");
    }
}
