#include <iostream>
#include <vector>
#include "GreekCore/Pricing/PayOff.h"
#include "GreekCore/Pricing/Parameters.h"
#include "GreekCore/Pricing/VanillaOption.h"
#include "GreekCore/Pricing/MonteCarlo.h"

using namespace GreekCore;

// Mimicking the structure from the user's screenshot
// Ideally, the MonteCarlo wrapper would take VanillaOption, but we can unwrap it here for now
template<typename OptionType>
MonteCarloResult SimpleMonteCarlo(
    const OptionType& theOption, 
    double Spot, 
    const Parameters& Vol, 
    const Parameters& r, 
    unsigned long NumberOfPaths
) {
    return MonteCarloPricer::priceEuropean(
        Spot, 
        r, 
        Vol, 
        theOption.getExpiry(), 
        NumberOfPaths, 
        theOption.getPayOff()
    );
}

int main() {
    double Spot = 100.0;
    double Strike = 100.0;
    double r_val = 0.05;
    double Vol_val = 0.2;
    double Expiry = 1.0;
    unsigned long NumberOfPaths = 100000;

    // 1. Build Parameter Classes
    // Explicitly using the constant implementation (like screenshot) or just double (implicit)
    Parameters rParam(r_val);
    Parameters VolParam(Vol_val);

    // 2. Build Option
    PayOffVanilla callPayOff(OptionType::Call, Strike);
    VanillaOption<PayOffVanilla> theOption(callPayOff, Expiry);

    std::cout << "Pricing Call Option...\n";
    auto resultCall = SimpleMonteCarlo(theOption, Spot, VolParam, rParam, NumberOfPaths);
    
    std::cout << "Call Price: " << resultCall.price << "\n";

    // 3. Put Option
    PayOffVanilla putPayOff(OptionType::Put, Strike);
    VanillaOption<PayOffVanilla> putOption(putPayOff, Expiry);
    
    std::cout << "Pricing Put Option...\n";
    auto resultPut = SimpleMonteCarlo(putOption, Spot, VolParam, rParam, NumberOfPaths);
    std::cout << "Put Price: " << resultPut.price << "\n";

    // 4. Put-Call Parity Test
    // Call + K*exp(-rT) = Put + S
    // Note: Screenshot uses B(r,t) which is likely Bond price exp(-rT)
    double bond = std::exp(-rParam.integral(0, Expiry));
    double left = resultCall.price + Strike * bond;
    double right = resultPut.price + Spot;

    std::cout << "\nTest Put-Call Parity:\n";
    std::cout << "Call + K*exp(-rT) == " << left << "\n";
    std::cout << "Put + Spot        == " << right << "\n";
    std::cout << "Diff              == " << std::abs(left - right) << "\n";

    return 0;
}
