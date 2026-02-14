#include "GreekCore/Rates/Interpolator.h"
#include <algorithm>
#include <stdexcept>

namespace GreekCore {

    #ifndef PETRA_INTERPOLATOR_H
#define PETRA_INTERPOLATOR_H

#include <vector>
#include <algorithm>
#include <stdexcept>
#include <span> // C++20: Zero-copy memory view
#include <cmath>


/**
 * @brief Log-Linear Interpolation optimized for cache locality.
 */
double LinearInterpolator::interpolate(double x, std::span<const double> x_vals, std::span<const double> y_vals) {
    if (x_vals.size() != y_vals.size() || x_vals.empty()) [[unlikely]] {
        throw std::invalid_argument("Size mismatch or empty vectors");
    }

    if (x <= x_vals.front()) [[unlikely]] {
        return y_vals.front();
    }
    
    if (x >= x_vals.back()) [[unlikely]] {
        return y_vals.back();
    }

    auto it = std::upper_bound(x_vals.begin(), x_vals.end(), x);
    
    size_t i = std::distance(x_vals.begin(), it) - 1;

    double x1 = x_vals[i];
    double x2 = x_vals[i + 1];
    double y1 = y_vals[i];
    double y2 = y_vals[i + 1];

    double slope = (y2 - y1) / (x2 - x1);
    return y1 + (x - x1) * slope;
}



#endif // PETRA_INTERPOLATOR_H
}
