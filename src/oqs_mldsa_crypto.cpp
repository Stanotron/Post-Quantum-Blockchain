#include "oqs_mldsa_crypto.h"
#include <stdexcept>

static std::string variantToOqsName(const std::string& v) {
    if (v == "44") return OQS_SIG_alg_ml_dsa_44;
    if (v == "65") return OQS_SIG_alg_ml_dsa_65;
    if (v == "87") return OQS_SIG_alg_ml_dsa_87;
    throw std::runtime_error("Unsupported ML-DSA variant: " + v);
}

OqsMldsaCrypto::OqsMldsaCrypto(const std::string& variant)
    : variant_(variant),
      oqs_alg_name_(variantToOqsName(variant))
{
    sig_ = OQS_SIG_new(oqs_alg_name_.c_str());
    if (!sig_) {
        throw std::runtime_error("Failed to init OQS_SIG for " + oqs_alg_name_);
    }
}

OqsMldsaCrypto::~OqsMldsaCrypto() {
    if (sig_) {
        OQS_SIG_free(sig_);
        sig_ = nullptr;
    }
}

std::pair<std::vector<uint8_t>, std::vector<uint8_t>>
OqsMldsaCrypto::generateKeypair() {
    std::vector<uint8_t> pk(sig_->length_public_key);
    std::vector<uint8_t> sk(sig_->length_secret_key);

    if (OQS_SIG_keypair(sig_, pk.data(), sk.data()) != OQS_SUCCESS) {
        throw std::runtime_error("ML-DSA keypair generation failed");
    }
    return {pk, sk};
}

std::vector<uint8_t>
OqsMldsaCrypto::sign(const std::vector<uint8_t>& msg,
                     const std::vector<uint8_t>& sk) {
    std::vector<uint8_t> sig(sig_->length_signature);
    size_t sig_len = 0;

    if (OQS_SIG_sign(sig_, sig.data(), &sig_len,
                     msg.data(), msg.size(),
                     sk.data()) != OQS_SUCCESS) {
        throw std::runtime_error("ML-DSA sign failed");
    }
    sig.resize(sig_len);
    return sig;
}

bool
OqsMldsaCrypto::verify(const std::vector<uint8_t>& msg,
                       const std::vector<uint8_t>& sig,
                       const std::vector<uint8_t>& pk) {
    auto rc = OQS_SIG_verify(sig_,
                             msg.data(), msg.size(),
                             sig.data(), sig.size(),
                             pk.data());
    return rc == OQS_SUCCESS;
}

std::string OqsMldsaCrypto::name() const {
    return "ML-DSA-" + variant_;
}

size_t OqsMldsaCrypto::publicKeySize() const {
    return sig_->length_public_key;
}

size_t OqsMldsaCrypto::secretKeySize() const {
    return sig_->length_secret_key;
}

size_t OqsMldsaCrypto::maxSignatureSize() const {
    return sig_->length_signature;
}
