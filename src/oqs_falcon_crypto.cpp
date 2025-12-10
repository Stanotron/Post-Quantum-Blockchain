#include "oqs_falcon_crypto.h"
#include <stdexcept>

static std::string variantToOqsNameFalcon(const std::string& v) {
    if (v == "512")  return OQS_SIG_alg_falcon_512;
    if (v == "1024") return OQS_SIG_alg_falcon_1024;
    throw std::runtime_error("Unsupported Falcon variant: " + v);
}

OqsFalconCrypto::OqsFalconCrypto(const std::string& variant)
    : variant_(variant),
      oqs_alg_name_(variantToOqsNameFalcon(variant))
{
    sig_ = OQS_SIG_new(oqs_alg_name_.c_str());
    if (!sig_) {
        throw std::runtime_error("Failed to init OQS_SIG for " + oqs_alg_name_);
    }
}

OqsFalconCrypto::~OqsFalconCrypto() {
    if (sig_) {
        OQS_SIG_free(sig_);
        sig_ = nullptr;
    }
}

std::pair<std::vector<uint8_t>, std::vector<uint8_t>>
OqsFalconCrypto::generateKeypair() {
    std::vector<uint8_t> pk(sig_->length_public_key);
    std::vector<uint8_t> sk(sig_->length_secret_key);

    if (OQS_SIG_keypair(sig_, pk.data(), sk.data()) != OQS_SUCCESS) {
        throw std::runtime_error("Falcon keypair generation failed");
    }
    return {pk, sk};
}

std::vector<uint8_t>
OqsFalconCrypto::sign(const std::vector<uint8_t>& msg,
                      const std::vector<uint8_t>& sk) {
    std::vector<uint8_t> sig(sig_->length_signature);
    size_t sig_len = 0;

    if (OQS_SIG_sign(sig_, sig.data(), &sig_len,
                     msg.data(), msg.size(),
                     sk.data()) != OQS_SUCCESS) {
        throw std::runtime_error("Falcon sign failed");
    }
    sig.resize(sig_len);
    return sig;
}

bool
OqsFalconCrypto::verify(const std::vector<uint8_t>& msg,
                        const std::vector<uint8_t>& sig,
                        const std::vector<uint8_t>& pk) {
    auto rc = OQS_SIG_verify(sig_,
                             msg.data(), msg.size(),
                             sig.data(), sig.size(),
                             pk.data());
    return rc == OQS_SUCCESS;
}

std::string OqsFalconCrypto::name() const {
    return "Falcon-" + variant_;
}

size_t OqsFalconCrypto::publicKeySize() const {
    return sig_->length_public_key;
}

size_t OqsFalconCrypto::secretKeySize() const {
    return sig_->length_secret_key;
}

size_t OqsFalconCrypto::maxSignatureSize() const {
    return sig_->length_signature;
}
