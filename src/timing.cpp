// src/timing.cpp
#include "timing.h"
#include <chrono>

uint64_t nowMicros() {
    using namespace std::chrono;
    return duration_cast<microseconds>(
        steady_clock::now().time_since_epoch()
    ).count();
}
