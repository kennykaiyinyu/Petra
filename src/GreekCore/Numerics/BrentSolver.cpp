#include "GreekCore/Numerics/BrentSolver.h"
#include <cmath>
#include <limits>
#include <algorithm>

namespace GreekCore {

    SolverResult BrentSolver::solve(std::function<double(double)> f, double min, double max, double tolerance, int max_iter) {
        double a = min;
        double b = max;
        double fa = f(a);
        double fb = f(b);

        // 1. Initial Bracket Check
        if (fa * fb > 0.0) {
            if (std::abs(fa) < std::numeric_limits<double>::epsilon()) return {a, true, 0};
            if (std::abs(fb) < std::numeric_limits<double>::epsilon()) return {b, true, 0};
             // Bracket invalid
             return {0.0, false, 0}; 
        }

        double c = a;
        double fc = fa;
        double d = b - a; // Step size
        double e = d;     // Previous step size

        for (int iter = 0; iter < max_iter; ++iter) {
            // Ensure c is the "other" side of the bracket (opposite sign to b)
            if ((fb > 0.0 && fc > 0.0) || (fb < 0.0 && fc < 0.0)) {
                c = a;
                fc = fa;
                d = b - a;
                e = d;
            }

            // Keep b as the best guess (closest to 0)
            if (std::abs(fc) < std::abs(fb)) {
                a = b; b = c; c = a;
                fa = fb; fb = fc; fc = fa;
            }

            // Convergence check
            double tol = 2.0 * std::numeric_limits<double>::epsilon() * std::abs(b) + tolerance;
            double m = 0.5 * (c - b); // Half interval

            if (std::abs(m) <= tol || fb == 0.0) {
                return {b, true, iter + 1};
            }

            // Attempt Interpolation
            if (std::abs(e) >= tol && std::abs(fa) > std::abs(fb)) {
                double s = fb / fa;
                double p;
                double q;

                if (a == c) {
                    // Linear interpolation (Secant)
                    p = 2.0 * m * s;
                    q = 1.0 - s;
                } else {
                    // Inverse Quadratic Interpolation
                    q = fa / fc;
                    double r = fb / fc;
                    p = s * (2.0 * m * q * (q - r) - (b - a) * (r - 1.0));
                    q = (q - 1.0) * (r - 1.0) * (s - 1.0);
                }

                if (p > 0.0) {
                    q = -q;
                } else {
                    p = -p;
                }

                s = e;
                e = d;

                // Condition to accept interpolation
                // 1. Inside interval (less that 3/4 of m approx)
                // 2. Converging faster than bisection
                if (2.0 * p < 3.0 * m * q - std::abs(tol * q) && p < std::abs(0.5 * s * q)) {
                    d = p / q;
                } else {
                    // Bisect
                    d = m;
                    e = m;
                }
            } else {
                // Bisect
                d = m;
                e = m;
            }

            a = b;
            fa = fb;

            if (std::abs(d) > tol) {
                b += d;
            } else {
                b += (m > 0.0 ? tol : -tol);
            }
            fb = f(b);
        }

        return {b, false, max_iter};
    }
}
