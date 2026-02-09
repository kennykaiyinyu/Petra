#include <iostream>
#include <iomanip>
#include <cmath>
#include <numbers>
#include "GreekCore/Pricing/MonteCarlo.h"
#include "GreekCore/Pricing/PayOff.h"

using namespace GreekCore;

// Analytical Black-Scholes for comparison
double black_scholes_call(double S, double K, double r, double sigma, double T) {
    auto N = [](double x) { return 0.5 * std::erfc(-x / std::numbers::sqrt2); };
    double d1 = (std::log(S / K) + (r + 0.5 * sigma * sigma) * T) / (sigma * std::sqrt(T));
    double d2 = d1 - sigma * std::sqrt(T);
    return S * N(d1) - K * std::exp(-r * T) * N(d2);
}

int main() {
    double S = 100.0;
    double K = 100.0;
    double r = 0.05;
    double sigma = 0.2;
    double T = 1.0;

    std::cout << "--- 1. Vanilla Call Convergence ---\n";
    PayOffVanilla payoff(OptionType::Call, K);
    double exact = black_scholes_call(S, K, r, sigma, T);

    std::cout << "Exact Black-Scholes Price: " << std::fixed << std::setprecision(6) << exact << "\n\n";
    std::cout << std::setw(12) << "Paths" 
              << std::setw(15) << "MC Price" 
              << std::setw(15) << "Error" 
              << std::setw(15) << "1/sqrt(N)" << "\n";
    std::cout << std::string(60, '-') << "\n";

    // Run for powers of 10
    for (int p = 3; p <= 6; ++p) { // 10^3 to 10^6
        size_t paths = std::pow(10, p);
        auto result = MonteCarloPricer::priceEuropean(S, r, sigma, T, paths, payoff);
        
        double error = std::abs(result.price - exact);
        double convergence_rate = 1.0 / std::sqrt(paths);

        std::cout << std::setw(12) << paths 
                  << std::setw(15) << result.price 
                  << std::setw(15) << error
                  << std::setw(15) << convergence_rate << "\n";
    }

    std::cout << "\n--- 2. Digital Call Pricing ---\n";
    PayOffDigital digitalPayoff(OptionType::Call, K);
    // Digital Call BS: exp(-rT) * N(d2)
    auto N = [](double x) { return 0.5 * std::erfc(-x / std::numbers::sqrt2); };
    double d1 = (std::log(S / K) + (r + 0.5 * sigma * sigma) * T) / (sigma * std::sqrt(T));
    double d2 = d1 - sigma * std::sqrt(T);
    double exact_dig = std::exp(-r * T) * N(d2);
    
    std::cout << "Exact Digital Price: " << exact_dig << "\n";
    auto res_dig = MonteCarloPricer::priceEuropean(S, r, sigma, T, 100000, digitalPayoff);
    std::cout << "MC Digital Price (100k paths): " << res_dig.price 
              << " (Err: " << std::abs(res_dig.price - exact_dig) << ")\n";

    std::cout << "\n--- 3. Asian Call (Path Dependent) Convergence ---\n";
    // Using new pricePathDependent
    PayOffAsian asianPayoff(OptionType::Call, K);
    size_t steps = 252; // Daily monitoring
    
    std::cout << "Monitoring Asian Option Convergence (" << steps << " steps)...\n";
    std::cout << std::setw(12) << "Paths" 
              << std::setw(15) << "MC Price" 
              << std::setw(15) << "Std Err" 
              << std::setw(15) << "1/sqrt(N)" << "\n";
    std::cout << std::string(60, '-') << "\n";

    // Run 10^3 to 10^5 paths (computationally more intensive than European)
    for (int p = 3; p <= 5; ++p) { 
        size_t paths = std::pow(10, p);
        auto result = MonteCarloPricer::pricePathDependent(S, r, sigma, T, paths, steps, asianPayoff);
        
        double convergence_rate = 1.0 / std::sqrt(paths);

        std::cout << std::setw(12) << paths 
                  << std::setw(15) << result.price 
                  << std::setw(15) << result.error_estimate
                  << std::setw(15) << convergence_rate << "\n";
    }

    // Asian Options are generally cheaper than European due to averaging reducing volatility
    // We check the last run (highest path count)
    auto final_asian_res = MonteCarloPricer::pricePathDependent(S, r, sigma, T, 100000, steps, asianPayoff);
    std::cout << "\nFinal Asian Price (100k): " << final_asian_res.price << "\n";
    
    if (final_asian_res.price < exact) {
        std::cout << "[Check] Asian Price < European Price (Expected behavior confirmed).\n";
    } else {
        std::cout << "[Check] Warning: Asian Price >= European Price (Unexpected).\n";
    }

    return 0;
}
