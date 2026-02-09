#ifndef GREEKCORE_MONTECARLO_H
#define GREEKCORE_MONTECARLO_H

#include <vector>
#include <random>
#include <cmath>
#include <algorithm>
#include <concepts>
#include <numeric>
#include <iostream> 
#include <numbers>
#include <functional>

namespace GreekCore {

    struct MonteCarloResult {
        double price;
        double delta;
        double gamma;
        double theta;
        double vega;
        double rho;
        double error_estimate;
        double runtime_ms;
    };

    /**
     * @brief High-Performance Monte Carlo Engine.
     */
    class MonteCarloPricer {
    public:
        [[nodiscard]]
        static MonteCarloResult priceEuropean(double S0, double r, double sigma, double T, 
                                            size_t paths, const std::function<double(double)>& payoff);

        [[nodiscard]]
        static MonteCarloResult pricePathDependent(double S0, double r, double sigma, double T, 
                                            size_t paths, size_t steps,
                                            const std::function<double(const std::vector<double>&)>& payoff);
    };
}
#endif // GREEKCORE_MONTECARLO_H
