#include <iostream>
#include <vector>
#include "Engine.h"
#include "Strategy.h"
// In the future: #include <GreekCore/Pricing/MonteCarlo.h>

using namespace Petra::MarketMaker;

void log_quote(const Quote& q) {
    if (q.bid_px == 0 && q.ask_px == 0) {
        std::cout << "[MM-ENGINE] !! CIRCUIT BREAKER TRIGGERED !! Quotes Pulled." << std::endl;
        return;
    }
    std::cout << "[MM-ENGINE] Bid: " << q.bid_px << " x " << q.bid_qty 
              << " | Ask: " << q.ask_px << " x " << q.ask_qty << std::endl;
}

int main() {
    std::cout << "Starting Petra Market Maker with VPIN Toxicity Detection..." << std::endl;

    ConstantSpreadStrategy strategy(50.0, 100.0);
    Engine<ConstantSpreadStrategy> engine(strategy, log_quote);

    // Simulate Fake Feed with a "Toxic Flow" event
    double price = 100.0;
    uint64_t time_ns = 1670000000000;

    std::cout << "\n--- Phase 1: Normal Market ---" << std::endl;
    // Balanced flow
    for (int i = 0; i < 5; ++i) {
        price += (i % 2 == 0 ? 0.05 : -0.05); // Oscillate
        Tick t{time_ns, price, 1.0, true};
        std::cout << "Tick: " << price << std::endl;
        engine.on_market_data(t);
        time_ns += 1000000;
    }

    std::cout << "\n--- Phase 2: Toxic Sell-Off (Informed Trading) ---" << std::endl;
    // Massive one-sided selling
    for (int i = 0; i < 10; ++i) {
        price -= 0.10; // Rapid drop
        Tick t{time_ns, price, 10.0, true}; // Large size sells
        std::cout << "Tick: " << price << " (Large Sell 10.0)" << std::endl;
        engine.on_market_data(t);
        
        // Print toxicity for demo
        std::cout << "       Current Toxicity: " << engine.get_state().market_toxicity << std::endl;

        time_ns += 500000; // Faster
    }

    return 0;
}
