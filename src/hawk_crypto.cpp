#include "hawk_crypto.h"
#include <stdexcept>
#include <algorithm>
#include <ctime>    // time()

HawkCrypto::HawkCrypto(const std::string& variant)
    : variant_(variant)
{
    if (variant == "512") {
        logn_ = 9;
    } else if (variant == "1024") {
        logn_ = 10;
    } else {
        throw std::runtime_error("Unsupported Hawk variant: " + variant);
    }

    priv_len_ = HAWK_PRIVKEY_SIZE(logn_);
    pub_len_  = HAWK_PUBKEY_SIZE(logn_);
    sig_len_  = HAWK_SIG_SIZE(logn_);

    tmp_k_ = HAWK_TMPSIZE_KEYGEN(logn_);
    tmp_s_ = HAWK_TMPSIZE_SIGN(logn_);
    tmp_v_ = HAWK_TMPSIZE_VERIFY(logn_);

    tmp_len_ = tmp_k_;
    if (tmp_s_ > tmp_len_) tmp_len_ = tmp_s_;
    if (tmp_v_ > tmp_len_) tmp_len_ = tmp_v_;

    // RNG init (same style as your bench)
    uint64_t tq = (uint64_t)time(nullptr);
    shake_init(&rng_, 256);
    shake_inject(&rng_, &tq, sizeof tq);
    shake_flip(&rng_);
}

std::pair<std::vector<uint8_t>, std::vector<uint8_t>>
HawkCrypto::generateKeypair() {
    std::vector<uint8_t> sk(priv_len_);
    std::vector<uint8_t> pk(pub_len_);
    std::vector<uint8_t> tmp(tmp_len_);

    int ok = hawk_keygen(logn_,
                         sk.data(),  // priv
                         pk.data(),  // pub
                         (hawk_rng)&shake_extract, &rng_,
                         tmp.data(), tmp_k_);
    if (!ok) {
        throw std::runtime_error("Hawk keygen failed");
    }

    return {pk, sk};
}

std::vector<uint8_t>
HawkCrypto::sign(const std::vector<uint8_t>& msg,
                 const std::vector<uint8_t>& sk) {
    if (sk.size() != priv_len_) {
        throw std::runtime_error("Hawk sign: unexpected secret key size");
    }

    std::vector<uint8_t> sig(sig_len_);
    std::vector<uint8_t> tmp(tmp_len_);

    shake_context scd;
    hawk_sign_start(&scd);
    if (!msg.empty()) {
        shake_inject(&scd, msg.data(), msg.size());
    }

    int ok = hawk_sign_finish(logn_,
                              (hawk_rng)&shake_extract, &rng_,
                              sig.data(), &scd,
                              sk.data(),
                              tmp.data(), tmp_s_);
    if (!ok) {
        throw std::runtime_error("Hawk sign failed");
    }

    // For Hawk, sig_len_ is the encoded length; we just return full buffer.
    return sig;
}

bool
HawkCrypto::verify(const std::vector<uint8_t>& msg,
                   const std::vector<uint8_t>& sig,
                   const std::vector<uint8_t>& pk) {
    if (pk.size() != pub_len_) {
        return false;
    }
    if (sig.size() != sig_len_) {
        // Depending on Hawk encoding, you can relax this if needed.
        return false;
    }

    std::vector<uint8_t> tmp(tmp_len_);

    shake_context scd;
    hawk_verify_start(&scd);
    if (!msg.empty()) {
        shake_inject(&scd, msg.data(), msg.size());
    }

    int ok = hawk_verify_finish(logn_,
                                sig.data(), sig_len_,
                                &scd,
                                pk.data(), pub_len_,
                                tmp.data(), tmp_v_);
    return ok != 0;
}

std::string HawkCrypto::name() const {
    return "Hawk-" + variant_;
}

size_t HawkCrypto::publicKeySize() const {
    return pub_len_;
}

size_t HawkCrypto::secretKeySize() const {
    return priv_len_;
}

size_t HawkCrypto::maxSignatureSize() const {
    return sig_len_;
}
