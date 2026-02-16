#ifndef GREEKCORE_INTERPOLATORSTRATEGY_H
#define GREEKCORE_INTERPOLATORSTRATEGY_H

#include <vector>
#include <algorithm>
#include <stdexcept>
#include <span>
#include <cmath>
#include <concepts>

namespace GreekCore {

    /**
     * @brief Concept to ensure the strategy pattern is followed at ***compile-time***.
     */
    template<typename T>
    concept InterpolatorStrategy = requires(T t, double x, std::span<const double> x_vals, std::span<const double> y_vals) {
        { t.interpolate(x, x_vals, y_vals) } -> std::convertible_to<double>;
    };

    /**
     * @brief Linear Interpolation with Flat Extrapolation.
     * 
     * Uses binary search to find the interval and interpolates linearly.
     * Extrapolates flat (constant) beyond the first and last points.
     */
    struct LinearInterpolator {

        /**
         * @brief Interpolates $y$ at a given point $x$.
         * 
         * @param x The query point.
         * @param x_vals Monotonically increasing independent variable values.
         * @param y_vals Dependent variable values corresponding to x_vals.
         * @return The interpolated value.
         * @throws std::invalid_argument If arrays are empty or sizes differ.
         */
        [[nodiscard]] 
        static double interpolate(double x, std::span<const double> x_vals, std::span<const double> y_vals) {
            if (x_vals.size() != y_vals.size() || x_vals.empty()) [[unlikely]] {
                throw std::invalid_argument("Size mismatch or empty vectors");
            }

            if (x <= x_vals.front()) [[unlikely]] {
                return y_vals.front();
            }
            
            if (x >= x_vals.back()) [[unlikely]] {
                return y_vals.back();
            }

            // Find first element strictly greater than x
            auto it = std::upper_bound(x_vals.begin(), x_vals.end(), x);
            
            size_t i = std::distance(x_vals.begin(), it) - 1;

            auto x1 = x_vals[i];
            auto x2 = x_vals[i + 1];
            auto y1 = y_vals[i];
            auto y2 = y_vals[i + 1];

            auto slope = (y2 - y1) / (x2 - x1);
            return y1 + (x - x1) * slope;
        }
    };
}
#endif // GREEKCORE_INTERPOLATORSTRATEGY_H