#ifndef GREEKCORE_INTERPOLATOR_H
#define GREEKCORE_INTERPOLATOR_H

#include <vector>
#include <algorithm>
#include <stdexcept>
#include <span>   // C++20: The "Zero-Copy" View
#include <cmath>

namespace GreekCore {

    /**
     * @brief Concept to ensure the strategy pattern is followed at ***compile-time***.
     */
    template<typename T>
    concept InterpolationStrategy = requires(T t, double x, std::span<const double> x_vals, std::span<const double> y_vals) {
        { t.interpolate(x, x_vals, y_vals) } -> std::convertible_to<double>;
    };

    /**
     * @brief Linear Interpolation with Flat Extrapolation.
     * Replaces legacy Java 'LinearInterpolator.class'
     */
    struct LinearInterpolator {

        // [[nodiscard]] warns the user if they call this function but ignore the result
        [[nodiscard]]
        static double interpolate(double x, std::span<const double> x_vals, std::span<const double> y_vals);
    };
}
#endif // GREEKCORE_INTERPOLATOR_H
