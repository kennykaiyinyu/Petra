#ifndef PETRA_MM_RISK_TOXICITY_H
#define PETRA_MM_RISK_TOXICITY_H

#include <cmath>
#include <algorithm>
#include <deque>
#include "../Types.h"

namespace Petra::MarketMaker::Risk {

    /**
     * @brief Real-time VPIN (Volume-Synchronized Probability of Informed Trading) Estimator.
     * 
     * VPIN is the standard metric for "Order Flow Toxicity".
     * High VPIN indicates a high imbalance between buy and sell pressure,
     * often preceding volatility events or "Flash Crashes".
     * 
     * Implementation:
     * 1. Classify trades as Buy/Sell volumes.
     * 2. Track absolute order flow imbalance (OI).
     * 3. VPIN = MovingAverage( |V_buy - V_sell| ) / MovingAverage( Total Volume )
     */
    class VpinEstimator {
    public:
        // Config: Window size for moving average (e.g., last 50 trades)
        explicit VpinEstimator(size_t window_size = 50) 
            : window_size_(window_size)
            , last_price_(0.0) 
            , buy_vol_sum_(0.0)
            , sell_vol_sum_(0.0)
            , total_vol_sum_(0.0)
        {}

        /**
         * @brief Updates metrics with a new market tick.
         * Uses the "Tick Rule" to classify trade direction if not provided.
         */
        void on_tick(const Tick& tick) {
            if (!tick.is_trade) return;

            // 1. Classify Trade Direction (Tick Rule)
            // If price > last_price: Buy
            // If price < last_price: Sell
            // If price == last_price: Same as previous
            Side direction = Side::None;
            if (tick.price > last_price_) direction = Side::Buy;
            else if (tick.price < last_price_) direction = Side::Sell;
            else direction = last_direction_;

            last_price_ = tick.price;
            last_direction_ = direction;

            // 2. Update Volumes
            double v_buy = (direction == Side::Buy) ? tick.size : 0.0;
            double v_sell = (direction == Side::Sell) ? tick.size : 0.0;

            update_rolling_window(v_buy, v_sell);
        }

        /**
         * @brief Returns current toxicity metric [0.0 - 1.0].
         * > 0.6 is usually considered "Toxic".
         */
        [[nodiscard]]
        double get_toxicity() const {
            if (total_vol_sum_ < 1e-9) return 0.0; // Avoid Div/0
            
            // VPIN approx = |BuyVol - SellVol| / TotalVol
            double imbalance = std::abs(buy_vol_sum_ - sell_vol_sum_);
            return imbalance / total_vol_sum_;
        }

    private:
        struct TradeRecord {
            double buy_vol;
            double sell_vol;
        };

        void update_rolling_window(double v_buy, double v_sell) {
            history_.push_back({v_buy, v_sell});
            
            buy_vol_sum_ += v_buy;
            sell_vol_sum_ += v_sell;
            total_vol_sum_ += (v_buy + v_sell);

            if (history_.size() > window_size_) {
                TradeRecord old = history_.front();
                history_.pop_front();
                
                buy_vol_sum_ -= old.buy_vol;
                sell_vol_sum_ -= old.sell_vol;
                total_vol_sum_ -= (old.buy_vol + old.sell_vol);
            }
        }

        size_t window_size_;
        double last_price_;
        Side last_direction_ = Side::None;

        // Rolling sums
        double buy_vol_sum_;
        double sell_vol_sum_;
        double total_vol_sum_;

        std::deque<TradeRecord> history_;
    };
}

#endif // PETRA_MM_RISK_TOXICITY_H
