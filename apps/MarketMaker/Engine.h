#ifndef PETRA_MM_ENGINE_H
#define PETRA_MM_ENGINE_H

#include <vector>
#include <functional>
#include "Types.h"
#include "Strategy.h"
#include "Risk/Toxicity.h"

namespace Petra::MarketMaker {

    /**
     * @brief The HFT Engine.
     */
    template<MakingStrategy StrategyType>
    class Engine {
    public:
        using QuoteCallback = std::function<void(const Quote&)>;

        Engine(StrategyType strategy, QuoteCallback on_new_quote)
            : strategy_(std::move(strategy))
            , on_new_quote_(std::move(on_new_quote))
            , state_{0.0, 0.0, 0.0} 
            , toxicity_monitor_(50) // Look back 50 trades
        {}

        void on_market_data(const Tick& tick) {
            // 1. Update Toxicity Monitor
            toxicity_monitor_.on_tick(tick);
            state_.market_toxicity = toxicity_monitor_.get_toxicity();

            // 2. Run Strategy
            Quote new_quote = strategy_.on_tick(tick, state_);

            // 3. Risk: Circuit Breaker for Extreme Toxicity
            // If VPIN > 0.8, pull all quotes (send empty)
            if (state_.market_toxicity > 0.8) {
                // In prod, this would log a warning
                on_new_quote_(Quote::empty()); 
                return;
            }

            // 4. Standard Pre-Trade Checks
            if (new_quote.bid_px <= 0 || new_quote.ask_px <= 0) return;
            if (new_quote.bid_px >= new_quote.ask_px) return; 

            on_new_quote_(new_quote);
        }

        void on_fill(Side side, double price, double quantity) {
            if (side == Side::Buy) {
                state_.inventory += quantity;
                state_.cash -= price * quantity;
            } else {
                state_.inventory -= quantity;
                state_.cash += price * quantity;
            }
        }

        [[nodiscard]] const PortfolioState& get_state() const { return state_; }

    private:
        StrategyType strategy_;
        QuoteCallback on_new_quote_;
        PortfolioState state_;
        Risk::VpinEstimator toxicity_monitor_;
    };

}
#endif // PETRA_MM_ENGINE_H
