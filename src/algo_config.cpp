#include "algo_config.h"

// CHANGE THIS TO SWITCH ALGORITHMS
    // return {AlgoFamily::ML_DSA, "44"};
    // return {AlgoFamily::ML_DSA, "65"};
    // return {AlgoFamily::ML_DSA, "87"};
    // return {AlgoFamily::FALCON, "512"};
    // return {AlgoFamily::FALCON, "1024"};
    // return {AlgoFamily::HAWK, "512"};
    // return {AlgoFamily::HAWK, "1024"};

AlgoConfig getSelectedAlgorithm() {
    return {AlgoFamily::FALCON, "512"};
}
