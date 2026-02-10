#ifndef PETRA_MM_STRATEGY_ASE_H
#define PETRA_MM_STRATEGY_ASE_H

#include "Types.h"
#include <cmath>
#include <algorithm>

namespace Petra::MarketMaker {

    /**
     * @brief Avellaneda-Stoikov Inventory Strategy (2008)
     * 
     * Adjusts quotes based on current inventory to control risk.
     * 
     * Reservation Price (r): The price at which the agent is indifferent between buying/selling.
     * r(s, q, t) = s - q * gamma * sigma^2 * (T - t)
     * 
     * Optimal Bid = r - spread/2
     * Optimal Ask = r + spread/2
     * 
     * If inventory (q) is positive (Long), r < s.
     * Quotes shift DOWN. We bid lower (buy less) and ask lower (sell more).
     */
    class AvellanedaStoikovStrategy {
    public:
        struct Config {
            double gamma;       // Risk Aversion (e.g., 0.1)
            double sigma;       // Volatility (e.g., 2.0 annual)
            double T;           // Time horizon (e.g., 1.0 day = 1/252)
            double k;           // Order Book Liquidity Parameter (e.g., 1.5)
            double A;           // Order Arrival Rate parameter (e.g., 140)
        };

        AvellanedaStoikovStrategy(Config cfg) : cfg_(cfg) {}

        [[nodiscard]]
        Quote on_tick(const Tick& tick, const PortfolioState& state) const {
            double s = tick.price;
            double q = state.inventory;
            
            // 1. Calculate Reservation Price
            // Note: In HFT, (T-t) is often treated as a constant "window ahead" or just 1.0 for simplification
            double reservation_price = s - (q * cfg_.gamma * (cfg_.sigma * cfg_.sigma) * cfg_.T);

            // 2. Calculate Optimal Spread
            // delta_optimal = (1/gamma) * ln(1 + gamma/k) <-- Simplified classic result
            // Or typically we just use a calibrated base spread + inventory scaling
            // Let's use the explicit approximation:
            double half_spread = (1.0 / cfg_.gamma) * std::log(1.0 + (cfg_.gamma / cfg_.k));
            
            // Ensure minimum spread (tick size protection)
            if (half_spread < 0.01) half_spread = 0.01;

            // 3. Widen spread if toxic (Hybrid approach)
            if (state.market_toxicity > 0.0) {
                half_spread *= (1.0 + state.market_toxicity * 2.0);
            }

            return Quote{
                .bid_px = reservation_price - half_spread,
                .ask_px = reservation_price + half_spread,
                .bid_qty = 100.0, // Standard lot
                .ask_qty = 100.0
            };
        }

        [[nodiscard]]
        const char* name() const { return "Avellaneda-Stoikov"; }

    private:
        Config cfg_;
    };

}

#endif // PETRA_MM_STRATEGY_ASE_H
