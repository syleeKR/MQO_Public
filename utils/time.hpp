#pragma once

#include <chrono>
#include <functional>
#include <iostream>
#include <string>

namespace timing {

// Type alias for convenience
using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock>;

// Function to get the current time
inline TimePoint now() {
    return Clock::now();
}

inline double get_duration(const TimePoint& start, const TimePoint& end) {
    return std::chrono::duration<double>(end - start).count();
}

inline double get_duration(const TimePoint& start) {
    auto end = now();
    return std::chrono::duration<double>(end - start).count();
}

} 
