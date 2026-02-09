#include "GreekCore/Rates/Interpolator.h"
#include <algorithm>
#include <stdexcept>

namespace GreekCore {

    double LinearInterpolator::interpolate(double x, std::span<const double> x_vals, std::span<const double> y_vals) {
        // 1. Safety Checks (Debug mode only for performance)
        if (x_vals.size() != y_vals.size() || x_vals.empty()) {
            throw std::invalid_argument("Mismatch in data size or empty dataset");
        }

        // 2. Extrapolation (Flat) - Standard for Yield Curves
        if (x <= x_vals.front()) return y_vals.front();
        if (x >= x_vals.back()) return y_vals.back();

        // 3. Binary Search (std::upper_bound is O(log n))
        // Finds the first element strictly greater than x
        auto it = std::upper_bound(x_vals.begin(), x_vals.end(), x);

        // 4. Calculate indices
        size_t i = std::distance(x_vals.begin(), it);

        // 5. The Math (y = y1 + slope * (x - x1))
        double x1 = x_vals[i - 1];
        double x2 = x_vals[i];
        double y1 = y_vals[i - 1];
        double y2 = y_vals[i];

        return y1 + (x - x1) * (y2 - y1) / (x2 - x1);
    }
}
