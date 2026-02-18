#include <iostream>
#include <thread>
#include <chrono>
#include <memory>
#include <vector>
#include <cmath>
#include <iomanip>
#include "GreekCore/Pricing/MonteCarlo.h"
#include "GreekCore/Pricing/PayOff.h"
#include "GreekCore/Numerics/Statistics.h"

using namespace GreekCore;

int main() {
    std::cout << "===========================================" << std::endl;
    std::cout << "   Async Monte Carlo Pricing with Monitoring" << std::endl;
    std::cout << "===========================================" << std::endl;

    double S0 = 100.0;
    double K = 100.0;
    double T = 1.0;
    size_t paths = 5000000; // Increased paths to allow time for monitoring

    Parameters r(0.05);
    Parameters sigma(0.2);
    
    PayOffVanilla callPayoff(OptionType::Call, K);

    // 1. Create the Lock-Free Gatherer directly
    // This class uses std::atomic internally, so no Decorator/Mutex wrapper is needed.
    // It is significantly faster for high-frequency updates.
    StatisticsMeanLockFree safeGatherer; 

    std::cout << "[Main] Launching Call Pricing Task (Async)..." << std::endl;
    
    // We pass the SAFE gatherer to the engine
    // Note: We pass by REFERENCE (safeGatherer). We own the object on the stack.
    auto callFuture = MonteCarloPricer::priceEuropeanAsync(
        S0, r, sigma, T, paths, callPayoff, safeGatherer
    );

    std::cout << "[Main] Monitoring progress..." << std::endl;
    std::cout << std::fixed << std::setprecision(4);

    // Monitor loop
    while (callFuture.wait_for(std::chrono::milliseconds(200)) != std::future_status::ready) {
        // Read interim results safely
        auto results = safeGatherer.getResultsSoFar();
        if (!results.empty() && !results[0].empty()) {
            double currentPrice = results[0][0];
            std::cout << "[Monitor] Current Estimate: " << currentPrice << std::endl;
        } else {
            std::cout << "[Monitor] initializing..." << std::endl;
        }
    }

    std::cout << "[Main] Task complete." << std::endl;
    
    // Get final result ensures any exceptions are thrown
    // Although future.get() is void here, it propagates exceptions.
    callFuture.get();

    auto results = safeGatherer.getResultsSoFar();
    double finalPrice = results[0][0];

    std::cout << "\n--- Final Result ---" << std::endl;
    std::cout << "Call Price: " << finalPrice << std::endl;

    return 0;
}
