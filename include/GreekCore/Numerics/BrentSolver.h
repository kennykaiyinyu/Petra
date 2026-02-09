#ifndef GREEKCORE_BRENTSOLVER_H
#define GREEKCORE_BRENTSOLVER_H

#include <cmath>
#include <concepts>
#include <limits>
#include <algorithm>
#include <functional>

namespace GreekCore {

    struct SolverResult {
        double root;
        bool converged;
        int iterations;
    };

    /**
     * @brief Brent's Method Implementation.
     * Guaranteed convergence if the root is bracketed.
     * Combines Bisection, Secant, and Inverse Quadratic Interpolation.
     */
    class BrentSolver {
    public:
        /**
         * @brief Solves f(x) = 0 for x in [min, max].
         * 
         * @param f The function to solve.
         * @param min Lower bound of the bracket.
         * @param max Upper bound of the bracket.
         * @param tolerance Convergence tolerance (absolute).
         * @param max_iter Maximum number of iterations.
         * @return SolverResult containing the root and status.
         */
        [[nodiscard]]
        static SolverResult solve(std::function<double(double)> f, double min, double max, double tolerance = 1e-8, int max_iter = 100);
    };
}
#endif // GREEKCORE_BRENTSOLVER_H
