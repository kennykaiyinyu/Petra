#ifndef PETRA_MM_TYPES_H
#define PETRA_MM_TYPES_H

#include <cstdint>
#include <limits>

namespace Petra::MarketMaker {

    struct Tick {
        uint64_t timestamp; // Nanos since epoch
        double price;
        double size;
        bool is_trade;      // true = trade, false = BBO update
    };

    struct Quote {
        double bid_px;
        double ask_px;
        double bid_qty;
        double ask_qty;
        
        static constexpr Quote empty() {
            return {0.0, 0.0, 0.0, 0.0};
        }
    };

    struct PortfolioState {
        double inventory;
        double cash;
        double current_risk_exposure; // Vega/Delta etc.
        double market_toxicity;       // 0.0 (safe) to 1.0 (toxic)
    };

    enum class Side { Buy, Sell, None };
}

#endif // PETRA_MM_TYPES_H
