#pragma once
#include <cstddef>
#include <vector>

// AUX filt uses direct-form II transposed coefficients, with implicit a[0]=1.
// State is the same array returned by a two-output ordinary filt call.
inline void filter_block(const double* src, double* dst, size_t frames,
                         const std::vector<double>& b, const std::vector<double>& a,
                         std::vector<double>& state) {
    for (size_t i = 0; i < frames; ++i) {
        const double x = src[i];
        const double y = b.front()*x + (state.empty() ? 0.0 : state.front());
        for (size_t k = 0; k < state.size(); ++k) {
            const size_t j = k+1;
            state[k] = (j < b.size() ? b[j]*x : 0.0)
                     - (j < a.size() ? a[j]*y : 0.0)
                     + (j < state.size() ? state[j] : 0.0);
        }
        dst[i] = y;
    }
}
