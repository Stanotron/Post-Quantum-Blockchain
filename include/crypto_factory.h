#pragma once
#include <memory>
#include "algo_config.h"
#include "crypto.h"

std::shared_ptr<Crypto> createCrypto(const AlgoConfig& cfg);
