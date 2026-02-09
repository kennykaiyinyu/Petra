#include "GreekCore/Pricing/MonteCarlo.h"
#include "GreekCore/Numerics/RNG.h"
#include <random>
#include <cmath>

namespace GreekCore {

    struct SimResult {
        double price;
        double std_err;
    };

    MonteCarloResult MonteCarloPricer::priceEuropean(double S0, double r, double sigma, double T, 
                                        size_t paths, const std::function<double(double)>& payoff) {
        
        // 1. Base Simulation
        auto run_sim = [&](double S_loc, double r_loc, double sigma_loc, double T_loc) -> SimResult {
             double drift = (r_loc - 0.5 * sigma_loc * sigma_loc) * T_loc;
             double diff = sigma_loc * std::sqrt(T_loc);
             double df = std::exp(-r_loc * T_loc);
             
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
                 
                 // Antithetic Variates
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
        
        // Base Price
        SimResult base_res = run_sim(S0, r, sigma, T);
        double price = base_res.price;
        double error_est = base_res.std_err;
        
        // Finite Differences for Greeks
        double dS = S0 * 0.01;
        double P_up = run_sim(S0 + dS, r, sigma, T).price;
        double P_down = run_sim(S0 - dS, r, sigma, T).price;
        double delta = (P_up - P_down) / (2.0 * dS);
        double gamma = (P_up - 2.0 * price + P_down) / (dS * dS);
        
        double dSigma = 0.01; 
        double P_vol_up = run_sim(S0, r, sigma + dSigma, T).price;
        double P_vol_down = run_sim(S0, r, sigma - dSigma, T).price;
        double vega = (P_vol_up - P_vol_down) / (2.0 * dSigma); 
        
        double dR = 0.0001; 
        double P_rho_up = run_sim(S0, r + dR, sigma, T).price;
        double P_rho_down = run_sim(S0, r - dR, sigma, T).price;
        double rho = (P_rho_up - P_rho_down) / (2.0 * dR);
        
        double dT = 1.0 / 365.0; 
        double theta_val = 0.0;
        if (T > dT) {
            double P_theta = run_sim(S0, r, sigma, T - dT).price;
            theta_val = (P_theta - price) / dT; 
        }

        return {price, delta, gamma, theta_val, vega, rho, error_est, 0.0};
    }

    MonteCarloResult MonteCarloPricer::pricePathDependent(double S0, double r, double sigma, double T, 
                                        size_t paths, size_t steps,
                                        const std::function<double(const std::vector<double>&)>& payoff) {
        
        auto run_sim = [&](double S_loc, double r_loc, double sigma_loc, double T_loc) -> SimResult {
             double dt = T_loc / steps;
             double drift = (r_loc - 0.5 * sigma_loc * sigma_loc) * dt;
             double diff = sigma_loc * std::sqrt(dt);
             double df = std::exp(-r_loc * T_loc);
             
             Xoshiro256 local_rng(42); 

             double sum = 0.0;
             double sum_sq = 0.0;
             
             std::vector<double> path(steps);

             for (size_t i = 0; i < paths; ++i) {
                 double current_S = S_loc;
                 for(size_t j=0; j<steps; ++j) {
                     double u1 = Xoshiro256::to_double(local_rng());
                     if (u1 < 1e-9) u1 = 1e-9;
                     double u2 = Xoshiro256::to_double(local_rng());
                     
                     double R = std::sqrt(-2.0 * std::log(u1));
                     double theta_bm = 2.0 * std::numbers::pi * u2;
                     double z = R * std::cos(theta_bm);

                     current_S *= std::exp(drift + diff * z);
                     path[j] = current_S;
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
        
        // Base Price
        SimResult base_res = run_sim(S0, r, sigma, T);
        double price = base_res.price;
        double error_est = base_res.std_err;
        
        // Finite Differences for Greeks
        double dS = S0 * 0.01;
        double P_up = run_sim(S0 + dS, r, sigma, T).price;
        double P_down = run_sim(S0 - dS, r, sigma, T).price;
        double delta = (P_up - P_down) / (2.0 * dS);
        double gamma = (P_up - 2.0 * price + P_down) / (dS * dS);
        
        double dSigma = 0.01; 
        double P_vol_up = run_sim(S0, r, sigma + dSigma, T).price;
        double P_vol_down = run_sim(S0, r, sigma - dSigma, T).price;
        double vega = (P_vol_up - P_vol_down) / (2.0 * dSigma); 
        
        double dR = 0.0001; 
        double P_rho_up = run_sim(S0, r + dR, sigma, T).price;
        double P_rho_down = run_sim(S0, r - dR, sigma, T).price;
        double rho = (P_rho_up - P_rho_down) / (2.0 * dR);
        
        double dT = 1.0 / 365.0; 
        double theta_val = 0.0;
        if (T > dT) {
            double P_theta = run_sim(S0, r, sigma, T - dT).price;
            theta_val = (P_theta - price) / dT; 
        }

        return {price, delta, gamma, theta_val, vega, rho, error_est, 0.0};
    }
}
