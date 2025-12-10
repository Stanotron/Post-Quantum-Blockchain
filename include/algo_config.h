#pragma once
#include <string>

enum class AlgoFamily {
    ML_DSA,
    FALCON,
    HAWK
};

struct AlgoConfig {
    AlgoFamily family;
    std::string variant;  // "44", "65", "87", "512", "1024", etc.
};

// For now, just hardcode here; later you can use CLI args.
AlgoConfig getSelectedAlgorithm();
