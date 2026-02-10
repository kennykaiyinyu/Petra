#include <iostream>
#include <vector>
#include <iomanip>
#include "GreekCore/Pricing/PayOff.h"
#include "GreekCore/Pricing/Parameters.h"
#include "GreekCore/Pricing/VanillaOption.h"
#include "GreekCore/Pricing/MonteCarlo.h"
#include "GreekCore/Numerics/Statistics.h"

using namespace GreekCore;

int main() {
    std::cout << "--- Design Patterns in Derivatives Pricing ---\n";
    std::cout << "1. Bridge Pattern (Parameters)\n";
    std::cout << "2. Strategy Pattern (PayOff)\n";
    std::cout << "3. Wrapper Pattern (VanillaOption)\n";
    std::cout << "4. Gatherer Pattern (StatisticsMC)\n\n";

    double Spot = 100.0;
    double Strike = 100.0;
    double r_val = 0.05;
    double Vol_val = 0.2;
    double Expiry = 1.0;
    unsigned long NumberOfPaths = 100000;

    // 1. Parameters (Bridge)
    Parameters rParam(r_val);
    Parameters VolParam(Vol_val);

    // 2. Option
    PayOffVanilla callPayOff(OptionType::Call, Strike);
    VanillaOption<PayOffVanilla> theOption(callPayOff, Expiry);

    // 3. Statistics (Gatherer)
    // We want to see convergence at powers of 10
    std::vector<unsigned long> stoppingPoints = { 
        100, 500, 1000, 5000, 10000, 20000, 50000, 75000, 100000 
    };
    
    // The Decorator Pattern: 
    // StatisticsConvergence WRAPS StatisticsMean
    StatisticsMean meanGatherer; 
    StatisticsConvergence gatherer(meanGatherer, stoppingPoints);

    // 4. Engine Execution
    std::cout << "Pricing Call Option using Statistics Gatherer (" << NumberOfPaths << " paths)...\n";
    
    MonteCarloPricer::priceEuropean(
        Spot, 
        rParam, 
        VolParam, 
        theOption.getExpiry(), 
        NumberOfPaths, 
        theOption.getPayOff(),
        gatherer
    );

    auto results = gatherer.getResultsSoFar(); 
    
    std::cout << "\n--- Convergence Table ---\n";
    std::cout << std::setw(10) << "Paths" << std::setw(15) << "Mean" << std::setw(15) << "Std Err" << "\n";
    std::cout << std::string(40, '-') << "\n";
    
    for (const auto& row : results) {
        std::cout << std::setw(10) << row[0] 
                  << std::setw(15) << row[1] 
                  << std::setw(15) << row[2] << "\n";
    }

    double callPrice = results.back()[1]; // Last result
    std::cout << "\nFinal Call Price: " << callPrice << "\n";

    // Re-verify Put-Call Parity with convenient overload (which uses local gatherer internally)
    PayOffVanilla putPayOff(OptionType::Put, Strike);
    auto resultPut = MonteCarloPricer::priceEuropean(Spot, rParam, VolParam, Expiry, NumberOfPaths, putPayOff);
    
    std::cout << "Put Price:  " << resultPut.price << "\n";

    double bond = std::exp(-rParam.integral(0, Expiry));
    double left = callPrice + Strike * bond;
    double right = resultPut.price + Spot;

    std::cout << "\nTest Put-Call Parity:\n";
    std::cout << "Call + K*exp(-rT) == " << left << "\n";
    std::cout << "Put + Spot        == " << right << "\n";
    std::cout << "Diff              == " << std::abs(left - right) << "\n";

    return 0;
}
