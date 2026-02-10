#ifndef GREEKCORE_MONTECARLO_H
#define GREEKCORE_MONTECARLO_H

#include <vector>
#include <random>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <functional>
#include <numbers>
#include "GreekCore/Numerics/RNG.h"
#include "GreekCore/Pricing/Parameters.h"

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
    private:
        struct SimResult {
            double price;
            double std_err;
        };

        template<typename Func>
        static MonteCarloResult calculateWithGreeks(double S0, const Parameters& r, const Parameters& sigma, double T, Func pricer_func) {
            // Base Price
            SimResult base_res = pricer_func(S0, r, sigma, T);
            double price = base_res.price;
            double error_est = base_res.std_err;
            
            // Finite Differences for Greeks
            double dS = S0 * 0.01;
            double P_up = pricer_func(S0 + dS, r, sigma, T).price;
            double P_down = pricer_func(S0 - dS, r, sigma, T).price;
            double delta = (P_up - P_down) / (2.0 * dS);
            double gamma = (P_up - 2.0 * price + P_down) / (dS * dS);
            
            // Vega (Bump Sigma)
            // Note: Currently Parameters does not support "bumping" a curve easily without re-construction.
            // For simple sensitivity, we assume Parallel Shift.
            double dSigma = 0.01; 
            Parameters sigma_up = Parameters(sigma.rootMeanSquare(0, T) + dSigma); // Simplify for now: treat as const bump
            Parameters sigma_down = Parameters(sigma.rootMeanSquare(0, T) - dSigma);

            double P_vol_up = pricer_func(S0, r, sigma_up, T).price;
            double P_vol_down = pricer_func(S0, r, sigma_down, T).price;
            double vega = (P_vol_up - P_vol_down) / (2.0 * dSigma); 
            
            double dR = 0.0001; 
            Parameters r_up = Parameters(r.mean(0, T) + dR);
            Parameters r_down = Parameters(r.mean(0, T) - dR);
            double P_rho_up = pricer_func(S0, r_up, sigma, T).price;
            double P_rho_down = pricer_func(S0, r_down, sigma, T).price;
            double rho = (P_rho_up - P_rho_down) / (2.0 * dR);
            
            double dT = 1.0 / 365.0; 
            double theta_val = 0.0;
            if (T > dT) {
                double P_theta = pricer_func(S0, r, sigma, T - dT).price;
                theta_val = (P_theta - price) / dT; 
            }

            return {price, delta, gamma, theta_val, vega, rho, error_est, 0.0};
        }

    public:
        // Templated European Pricer
        template<typename PayoffType>
        static MonteCarloResult priceEuropean(double S0, const Parameters& r, const Parameters& sigma, double T, 
                                            size_t paths, const PayoffType& payoff) {
            
            auto engine_logic = [&](double S_loc, const Parameters& r_loc, const Parameters& sigma_loc, double T_loc) -> SimResult {
                // Use integral of rates and vol for the full period
                double r_integral = r_loc.integral(0.0, T_loc);
                double vol_sq_integral = sigma_loc.integralSquare(0.0, T_loc);
                double rm_vol = std::sqrt(vol_sq_integral / T_loc);

                double drift = r_integral - 0.5 * vol_sq_integral;
                double diff = std::sqrt(vol_sq_integral);
                double df = std::exp(-r_integral);
                
                Xoshiro256 local_rng(42); 

                double sum = 0.0;
                double sum_sq = 0.0;
                size_t n_pairs = 0;

                for (size_t i = 0; i < paths; i += 2) {
                    double u1 = Xoshiro256::to_double(local_rng());
                    if (u1 < 1e-9) u1 = 1e-9;
                    double u2 = Xoshiro256::to_double(local_rng());
                    
                    double R = std::sqrt(-2.0 * std::log(u1));
                    double theta_bm = 2.0 * std::numbers::pi * u2;
                    double z1 = R * std::cos(theta_bm);
                    
                    // Antithetic
                    double ST_1 = S_loc * std::exp(drift + diff * z1);
                    double ST_2 = S_loc * std::exp(drift + diff * (-z1));
                    
                    double val1 = payoff(ST_1);
                    double val2 = payoff(ST_2);
                    double pair_avg = 0.5 * (val1 + val2);

                    sum += pair_avg;
                    sum_sq += pair_avg * pair_avg;
                    n_pairs++;
                }
                
                double mean = (n_pairs > 0) ? (sum / n_pairs) : 0.0;
                double variance = (n_pairs > 1) ? ((sum_sq - n_pairs * mean * mean) / (n_pairs - 1)) : 0.0;
                if (variance < 0) variance = 0.0;
                double std_err = std::sqrt(variance) / std::sqrt(n_pairs);

                return {mean * df, std_err * df};
            };

            return calculateWithGreeks(S0, r, sigma, T, engine_logic);
        }

        // Templated Path Dependent Pricer
        template<typename PayoffType>
        static MonteCarloResult pricePathDependent(double S0, const Parameters& r, const Parameters& sigma, double T, 
                                            size_t paths, size_t steps,
                                            const PayoffType& payoff) {
            
            auto engine_logic = [&](double S_loc, const Parameters& r_loc, const Parameters& sigma_loc, double T_loc) -> SimResult {
                double dt = T_loc / steps;
                
                // Precompute step parameters (approximation: assume constant over step)
                // Ideally we would integrate r and sigma over [t, t+dt] for each step
                double df = std::exp(-r_loc.integral(0.0, T_loc));
                
                Xoshiro256 local_rng(42); 

                double sum = 0.0;
                double sum_sq = 0.0;
                
                std::vector<double> path(steps);

                for (size_t i = 0; i < paths; ++i) {
                    double current_S = S_loc;
                    double current_time = 0.0;

                    for(size_t j=0; j<steps; ++j) {
                        double next_time = current_time + dt;
                        
                        // Exact integration for this step
                        double r_step = r_loc.integral(current_time, next_time);
                        double vol_sq_step = sigma_loc.integralSquare(current_time, next_time);
                        
                        double drift = r_step - 0.5 * vol_sq_step;
                        double diff = std::sqrt(vol_sq_step);

                        double u1 = Xoshiro256::to_double(local_rng());
                        if (u1 < 1e-9) u1 = 1e-9;
                        double u2 = Xoshiro256::to_double(local_rng());
                        
                        double R = std::sqrt(-2.0 * std::log(u1));
                        double theta_bm = 2.0 * std::numbers::pi * u2;
                        double z = R * std::cos(theta_bm);

                        current_S *= std::exp(drift + diff * z);
                        path[j] = current_S;
                        
                        current_time = next_time;
                    }
                    
                    double val = payoff(path);
                    sum += val;
                    sum_sq += val * val;
                }
                
                double mean = (paths > 0) ? (sum / paths) : 0.0;
                double variance = (paths > 1) ? ((sum_sq - paths * mean * mean) / (paths - 1)) : 0.0;
                if (variance < 0) variance = 0.0;
                double std_err = std::sqrt(variance) / std::sqrt(paths);

                return {mean * df, std_err * df};
            };

            return calculateWithGreeks(S0, r, sigma, T, engine_logic);
        }
    };
}
#endif // GREEKCORE_MONTECARLO_H
