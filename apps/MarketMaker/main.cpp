#include <iostream>
#include <vector>
#include <random>
#include <iomanip>
#include "Engine.h"
#include "Strategy.h"
#include "StrategyAvellaneda.h"

using namespace Petra::MarketMaker;

// Global engine pointer for the simulation feed to callback
// In real code, this would be a proper event loop
template<typename E>
void simulate_matching_engine(E& engine, const Quote& q, double mid_price, double dt) {
    // Simple outcome generator:
    // If Quote crosses Mid (unlikely in real life without trade) or is very close, fill it.
    // Probabilistic fill model: P(fill) ~ exp(-k * distance)
    
    std::mt19937 rng(42 + (int)mid_price);
    std::uniform_real_distribution<> dist(0.0, 1.0);

    double prob_scale = 0.8; // High liquidity

    // Check Bid Fill
    double bid_dist = mid_price - q.bid_px;
    if (bid_dist < 0) bid_dist = 0; // Crossed
    double p_fill_bid = prob_scale * std::exp(-10.0 * bid_dist);
    
    if (q.bid_px > 0 && dist(rng) < p_fill_bid) {
        std::cout << "   [EXCHANGE] Filled BUY  " << q.bid_qty << " @ " << q.bid_px << "\n";
        engine.on_fill(Side::Buy, q.bid_px, q.bid_qty);
    }

    // Check Ask Fill
    double ask_dist = q.ask_px - mid_price;
    if (ask_dist < 0) ask_dist = 0;
    double p_fill_ask = prob_scale * std::exp(-10.0 * ask_dist);
    
    if (q.ask_px > 0 && dist(rng) < p_fill_ask) {
        std::cout << "   [EXCHANGE] Filled SELL " << q.ask_qty << " @ " << q.ask_px << "\n";
        engine.on_fill(Side::Sell, q.ask_px, q.ask_qty);
    }
}

int main() {
    std::cout << "Starting Petra Market Maker (Avellaneda-Stoikov + VPIN)...\n\n";

    // Config: gamma=0.1 (Risk Averse), sigma=2.0, T=1.0, k=1.5
    AvellanedaStoikovStrategy::Config cfg{0.1, 2.0, 1.0, 1.5, 140.0};
    AvellanedaStoikovStrategy strategy(cfg);
    
    // Engine Callback (just print)
    auto on_quote = [](const Quote& q) {
        // std::cout << "Q: " << q.bid_px << " | " << q.ask_px << "\n";
    };

    Engine<AvellanedaStoikovStrategy> engine(strategy, on_quote);

    double price = 100.0;
    uint64_t time_ns = 1670000000000;
    std::mt19937 rng(12345);
    std::normal_distribution<> walk(0.0, 0.1); // Random Walk

    std::cout << std::fixed << std::setprecision(4);
    std::cout << "Time | Market Pr | Inventory | Bid       Ask      | Action\n";
    std::cout << "---------------------------------------------------------\n";

    // Simulation Loop
    for (int i = 0; i < 50; ++i) {
        // 1. Market Moves
        price += walk(rng);
        
        // 2. Tick Arrives
        Tick t{time_ns, price, 1.0, true};
        
        // 3. Engine Decides Quote
        // We manually assume the engine generated the quote internally for the simulation step
        // In a real app, engine.on_market_data would emit the quote via callback.
        // We peek at what the strategy WOULD do to simulate the fill:
        Quote q = strategy.on_tick(t, engine.get_state());
        
        std::cout << i << "   | " << price << "  | " << std::setw(5) << engine.get_state().inventory << "     | " 
                  << q.bid_px << "  " << q.ask_px << " | ";

        // 4. Update Engine (Toxicity, internal state)
        engine.on_market_data(t);

        // 5. Simulate Exchange Matches
        simulate_matching_engine(engine, q, price, 1.0);

        time_ns += 1000000;
    }

    return 0;
}
