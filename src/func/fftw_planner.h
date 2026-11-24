#pragma once

#include "fftw3.h"
#include <mutex>

// FFTW shares planner state between evaluators. Execution uses independent
// plans and buffers and can proceed concurrently.
inline std::mutex fftwPlannerMutex;

template<typename Function, typename... Args>
auto withFftwPlannerLock(Function function, Args... args) {
    std::lock_guard<std::mutex> lock(fftwPlannerMutex);
    return function(args...);
}
