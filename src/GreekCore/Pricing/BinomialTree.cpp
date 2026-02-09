#include "GreekCore/Pricing/BinomialTree.h"
#include <cmath>
#include <vector>
#include <algorithm>
#include <stdexcept>

namespace GreekCore {

    TreeResult BinomialTreePricer::price(double S0, double r, double sigma, double T, 
                                         int steps, std::function<double(double)> payoff, ExerciseType exercise) {
        
        if (steps < 2) throw std::invalid_argument("Steps must be at least 2 for Greek calculation");

        double dt = T / steps;
        double df = std::exp(-r * dt);
        
        // CRR Parameters
        double u = std::exp(sigma * std::sqrt(dt));
        double d = 1.0 / u;
        double p = (std::exp(r * dt) - d) / (u - d);
        double q = 1.0 - p;

        // Initialize leaves at maturity (Step N)
        std::vector<double> values(steps + 1);
        
        // j counts number of 'up' moves
        for (int j = 0; j <= steps; ++j) {
            double spot = S0 * std::pow(u, j) * std::pow(d, steps - j);
            values[j] = payoff(spot);
        }

        // Variables to store Greek Calculation Data
        double val_u = 0.0, val_d = 0.0;     // Values at Step 1
        double val_uu = 0.0, val_ud = 0.0, val_dd = 0.0; // Values at Step 2
        
        // Backward Induction
        for (int i = steps - 1; i >= 0; --i) {
            for (int j = 0; j <= i; ++j) {
                // Expectation
                double continuation = df * (p * values[j+1] + q * values[j]);
                
                // Spot at node (i, j)
                double spot = S0 * std::pow(u, j) * std::pow(d, i - j);
                
                if (exercise == ExerciseType::American) {
                    double intrinsic = payoff(spot);
                    values[j] = std::max(intrinsic, continuation);
                } else {
                    values[j] = continuation;
                }
            }

            // Snapshots for Greeks
            if (i == 2) {
                val_dd = values[0];
                val_ud = values[1];
                val_uu = values[2];
            }
            if (i == 1) {
                val_d = values[0];
                val_u = values[1];
            }
        }

        double price = values[0];

        // Calculate Greeks
        // Delta = (V_u - V_d) / (S_u - S_d)
        double S_u = S0 * u;
        double S_d = S0 * d;
        double delta = (val_u - val_d) / (S_u - S_d);

        // Gamma
        // Gamma approx = [ (V_uu - V_ud)/(S_uu - S_ud) - (V_ud - V_dd)/(S_ud - S_dd) ] / (0.5 * (S_uu - S_dd))
        // S_ud = S0 (since u = 1/d)
        double S_uu = S0 * u * u;
        double S_dd = S0 * d * d;
        double S_ud = S0; // == S_uu * d approx

        double delta_upper = (val_uu - val_ud) / (S_uu - S_ud);
        double delta_lower = (val_ud - val_dd) / (S_ud - S_dd);
        
        // Denominator is the change in spot between the midpoints of the upper/lower intervals
        double h = 0.5 * (S_uu - S_dd); 
        double gamma = (delta_upper - delta_lower) / h;

        // Theta
        double theta = (val_ud - price) / (2.0 * dt);

        return {price, delta, gamma, theta};
    }
}
