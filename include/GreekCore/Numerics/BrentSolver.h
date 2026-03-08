#ifndef GREEKCORE_BRENTSOLVER_H
#define GREEKCORE_BRENTSOLVER_H

#include <cmath>
#include <limits>
#include <algorithm>
#include "GreekCore/Utils/CompilerMacros.h"

namespace GreekCore {

    class BrentSolver {
    public:
        /**
         * @brief Solves f(x) = 0. Returns NaN if failed.
         * Implementation is in header to allow aggressive inlining of the functor F.
         */
        template <typename F>
        static FORCE_INLINE double solve(F&& f, double min, double max, double tolerance = 1e-8, int max_iter = 100) {
            double a = min;
            double b = max;
            double fa = f(a);
            double fb = f(b);

            // Optional: Check if root is bracketed. 
            if (fa * fb > 0.0) [[unlikely]] {
                 if (std::abs(fa) < tolerance) return a;
                 if (std::abs(fb) < tolerance) return b;
                 return std::numeric_limits<double>::quiet_NaN();
            }

            double c = a;
            double fc = fa;
            double d = b - a;
            double e = d;

            constexpr double epsilon = std::numeric_limits<double>::epsilon();

            for (int iter = 0; iter < max_iter; ++iter) {
                if (std::abs(fc) < std::abs(fb)) {
                    a = b; b = c; c = a;
                    fa = fb; fb = fc; fc = fa;
                }

                const double tol = 2.0 * epsilon * std::abs(b) + tolerance;
                const double m = 0.5 * (c - b);

                if (std::abs(m) <= tol || fb == 0.0) [[unlikely]] {
                    return b;
                }

                if (std::abs(e) >= tol && std::abs(fa) > std::abs(fb)) [[likely]] {
                    double s = fb / fa;
                    double p;
                    double q;

                    if (a == c) {
                        p = 2.0 * m * s;
                        q = 1.0 - s;
                    } else {
                        const double q0 = fa / fc;
                        const double r = fb / fc;
                        p = s * (2.0 * m * q0 * (q0 - r) - (b - a) * (r - 1.0));
                        q = (q0 - 1.0) * (r - 1.0) * (s - 1.0);
                    }

                    if (p > 0.0) q = -q;
                    else p = -p;

                    s = e;
                    e = d;

                    if (2.0 * p < 3.0 * m * q - std::abs(tol * q) && p < std::abs(0.5 * s * q)) [[likely]] {
                        d = p / q;
                    } else {
                        d = m;
                        e = m;
                    }
                } else {
                    d = m;
                    e = m;
                }

                a = b;
                fa = fb;

                if (std::abs(d) > tol) [[likely]] {
                    b += d;
                } else {
                    b += std::copysign(tol, m); 
                }
                fb = f(b);
            }
            return b;
        }
    };
}
#endif // GREEKCORE_BRENTSOLVER_H
