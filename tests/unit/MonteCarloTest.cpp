#include <gtest/gtest.h>
#include "GreekCore/Pricing/MonteCarlo.h"
#include "GreekCore/Pricing/PayOff.h"
#include <cmath>

using namespace GreekCore;

// Helper Black-Scholes analytical formula for Call
double black_scholes_call(double S, double K, double r, double sigma, double T) {
    auto N = [](double x) { return 0.5 * std::erfc(-x / std::numbers::sqrt2); };
    double d1 = (std::log(S / K) + (r + 0.5 * sigma * sigma) * T) / (sigma * std::sqrt(T));
    double d2 = d1 - sigma * std::sqrt(T);
    return S * N(d1) - K * std::exp(-r * T) * N(d2);
}

TEST(MonteCarloTest, ConvergenceToBlackScholes) {
    // Parameters
    double S = 100.0;
    double K = 100.0;
    double r = 0.05;
    double sigma = 0.2;
    double T = 1.0;
    size_t paths = 100000;

    PayOffVanilla payoff(OptionType::Call, K);
    
    // Updated syntax: No template parameter //
    auto result = MonteCarloPricer::priceEuropean(S, r, sigma, T, paths, payoff);
    double exact = black_scholes_call(S, K, r, sigma, T);

    // Standard Error check (99% confidence interval is approx +/- 2.57 * SE)
    // We expect the exact price to be within the confidence interval of the MC result
    // Or loosely near 1% relative error for 100k paths.
    
    // std::cout << "MC: " << result.price << " Exact: " << exact << " Err: " << result.error_estimate << std::endl;

    EXPECT_NEAR(result.price, exact, 3.0 * result.error_estimate); // Within 3 sigma
    EXPECT_NEAR(result.price, 10.45058, 0.1); // Known value approx
}

TEST(MonteCarloTest, AntitheticVarianceReduction) {
    // Hard to test randomness deterministically without seed control,
    // but we can ensure it runs and produces logical values.
    double S = 100.0;
    PayOffVanilla payoff(OptionType::Call, 110.0); // OTM Call
    auto result = MonteCarloPricer::priceEuropean(S, 0.05, 0.2, 1.0, 1000, payoff);
    
    EXPECT_GT(result.price, 0.0);
    EXPECT_LT(result.price, S); 
}

TEST(MonteCarloTest, PutOptionPricing) {
    double S = 100.0;
    double K = 100.0;  // ATM
    double r = 0.05;
    double sigma = 0.2;
    double T = 1.0;
    size_t paths = 50000;

    PayOffVanilla payoff(OptionType::Put, K);
    
    auto result = MonteCarloPricer::priceEuropean(S, r, sigma, T, paths, payoff);
    
    double exact_call = black_scholes_call(S, K, r, sigma, T);
    double exact_put = exact_call - S + K * std::exp(-r * T);

    EXPECT_NEAR(result.price, exact_put, 3.0 * result.error_estimate);
}

// Helper for Analytical Greeks
struct BSGreeks {
    double delta;
    double gamma;
    double vega;
    double rho;
    double theta;
};

BSGreeks black_scholes_greeks_call_struct(double S, double K, double r, double sigma, double T) {
    auto N = [](double x) { return 0.5 * std::erfc(-x / std::numbers::sqrt2); };
    // pdf is 1/sqrt(2pi) * exp(-x^2/2)
    auto pdf = [](double x) { return (1.0 / std::sqrt(2.0 * std::numbers::pi)) * std::exp(-0.5 * x * x); };

    double d1 = (std::log(S / K) + (r + 0.5 * sigma * sigma) * T) / (sigma * std::sqrt(T));
    double d2 = d1 - sigma * std::sqrt(T);

    double delta = N(d1);
    double gamma = pdf(d1) / (S * sigma * std::sqrt(T));
    double vega = S * pdf(d1) * std::sqrt(T); 
    
    double rho = K * T * std::exp(-r * T) * N(d2); // dV/dr
    double theta = - (S * pdf(d1) * sigma) / (2.0 * std::sqrt(T)) - r * K * std::exp(-r * T) * N(d2);

    return {delta, gamma, vega, rho, theta};
}

TEST(MonteCarloTest, GreeksAccuracy) {
    double S = 100.0;
    double K = 100.0; 
    double r = 0.05;
    double sigma = 0.2;
    double T = 1.0;
    size_t paths = 200000; 

    PayOffVanilla payoff(OptionType::Call, K);
    auto result = MonteCarloPricer::priceEuropean(S, r, sigma, T, paths, payoff);
    auto expected = black_scholes_greeks_call_struct(S, K, r, sigma, T);

    // Std::cout debug info
    std::cout << "Greeks Verification:" << std::endl;
    std::cout << "Delta: MC=" << result.delta << " BS=" << expected.delta << std::endl;
    std::cout << "Gamma: MC=" << result.gamma << " BS=" << expected.gamma << std::endl;
    std::cout << "Vega:  MC=" << result.vega  << " BS=" << expected.vega << std::endl;
    std::cout << "Rho:   MC=" << result.rho   << " BS=" << expected.rho << std::endl;
    std::cout << "Theta: MC=" << result.theta << " BS=" << expected.theta << std::endl;

    EXPECT_NEAR(result.delta, expected.delta, 0.02); 
    EXPECT_NEAR(result.gamma, expected.gamma, 0.005);
    EXPECT_NEAR(result.vega, expected.vega, 1.0);
    EXPECT_NEAR(result.rho, expected.rho, 2.0);
    EXPECT_NEAR(result.theta, expected.theta, 1.0);
}

