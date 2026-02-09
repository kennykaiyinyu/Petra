#include <iostream>
#include <vector>

// Updated include path based on the 'include/GreekCore' structure
#include <GreekCore/Pricing/PayOff.h>

int main() {
    std::cout << "Starting GreekCore High-Performance Pricing Engine...\n";

    // 1. Setup Market Parameters
    double spot_price = 100.0;
    double strike_price = 105.0;

    // 2. Initialize Payoffs
    GreekCore::PayOffVanilla callOption(GreekCore::OptionType::Call, strike_price);
    GreekCore::PayOffVanilla putOption(GreekCore::OptionType::Put, strike_price);
    
    // New Exotic Payoffs
    GreekCore::PayOffDigital digitalCall(GreekCore::OptionType::Call, strike_price);
    GreekCore::PayOffDoubleDigital rangeBet(95.0, 110.0); // Pays 1 if 95 < S < 110

    // Path Dependent (Asian)
    GreekCore::PayOffAsian asianCall(GreekCore::OptionType::Call, strike_price);

    // 3. Simulate a "Price Move"
    std::vector<double> spot_moves = {90.0, 95.0, 100.0, 105.0, 110.0, 115.0};

    // 4. Calculate Payoffs (Zero Allocation Loop)
    std::cout << "--- Vanilla & Digital Payoffs ---\n";
    for (double spot : spot_moves) {
        double call_val = callOption(spot);
        double put_val = putOption(spot);
        double dig_val = digitalCall(spot);
        double range_val = rangeBet(spot);

        std::cout << "Spot: " << spot 
                  << " | Call: " << call_val 
                  << " | Put: " << put_val 
                  << " | Dig: " << dig_val
                  << " | Range: " << range_val << "\n";
    }

    // 5. Calculate Path Dependent Payoff
    std::cout << "\n--- Path Dependent (Asian) ---\n";
    // Construct a path that averages to something ITM/OTM
    // Path 1: Avg = 100 (OTM for 105 Call)
    std::vector<double> path1 = {90.0, 100.0, 110.0}; 
    // Path 2: Avg = 110 (ITM for 105 Call -> Payoff 5.0)
    std::vector<double> path2 = {100.0, 110.0, 120.0};

    std::cout << "Path1 Avg: " << 100.0 << " | Asian Call Payoff: " << asianCall(path1) << "\n";
    std::cout << "Path2 Avg: " << 110.0 << " | Asian Call Payoff: " << asianCall(path2) << "\n";

    return 0;
}
