#ifndef PETRA_MM_STRATEGY_H
#define PETRA_MM_STRATEGY_H

#include "Types.h"
#include <concepts>

namespace Petra::MarketMaker {

    /**
     * @brief Strategy Concept.
     * Logic must implement on_tick to output a new Quote.
     */
    template<typename T>
    concept MakingStrategy = requires(T s, const Tick& tick, const PortfolioState& state) {
        { s.on_tick(tick, state) } -> std::convertible_to<Quote>;
        { s.name() } -> std::convertible_to<const char*>;
    };

    /**
     * @brief Example: Constant Spread Strategy.
     */
    struct ConstantSpreadStrategy {
        double spread_bps;
        double size;

        ConstantSpreadStrategy(double spread_bps_in, double size_in) 
            : spread_bps(spread_bps_in), size(size_in) {}

        [[nodiscard]]
        Quote on_tick(const Tick& tick, const PortfolioState& state) const {
            double mid = tick.price;
            
            // Adaptive Spreading:
            // If toxicity is high, widen the spread to protect against adverse selection.
            double toxicity_multiplier = 1.0 + (state.market_toxicity * 2.0); // Up to 3x spread
            double current_spread_bps = spread_bps * toxicity_multiplier;

            double half_spread = mid * (current_spread_bps * 0.0001) * 0.5;

            return Quote{
                .bid_px = mid - half_spread,
                .ask_px = mid + half_spread,
                .bid_qty = size,
                .ask_qty = size
            };
        }

        [[nodiscard]]
        const char* name() const { return "ConstantSpread"; }
    };
}

#endif // PETRA_MM_STRATEGY_H
