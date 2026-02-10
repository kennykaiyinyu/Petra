#include <gtest/gtest.h>
#include "GreekCore/Rates/YieldCurve.h"
#include "GreekCore/Time/Date.h"
#include "GreekCore/Time/DayCounter.h"
#include <vector>
#include <cmath>

using namespace GreekCore;
using namespace GreekCore::Time;

class YieldCurveTest : public ::testing::Test {
protected:
    std::vector<CurveInput> inputs;
    Date today;

    void SetUp() override {
        today = make_date(2023, 1, 1);
        
        // Use Act/365 to keep math simple (0.5 years = 182.5 days)
        // 1. 6M Deposit (approx 0.5y)
        // 2. 1Y Swap (approx 1.0y)
        // 3. 2Y Swap
        
        // Exact 0.5, 1.0, 2.0 years under Act/365
        Date d_6m = today + std::chrono::days(182); // ~0.498, rounding for logic checks
        // For testing "Exact" numbers, let's just assume we want roughly the right horizons.
        // A better approach for this Unit Test is to use the dates that correspond to exactly 0.5, 1.0 using the DayCounter logic
        // But simply providing arbitrary dates works if we check result consistency.
        
        Date d_1y = today + std::chrono::days(365);
        Date d_2y = today + std::chrono::days(730);
        Date d_6m_exact = today + std::chrono::days(static_cast<int>(365 * 0.5)); // 182 days

        inputs = {
            {d_6m_exact, 0.02, 0}, 
            {d_1y, 0.025, 1}, 
            {d_2y, 0.030, 1} 
        };
    }
};

TEST_F(YieldCurveTest, BootstrapsCorrectly) {
    // Default Act/365
    YieldCurve curve(today, inputs);

    // 1. Check 6M Point
    // T = 182/365 approx 0.4986
    // Simple Deposit Formula: 1 + 0.02 * T = 1/DF
    double t_6m = 182.0 / 365.0;
    double expected_df_6m = 1.0 / (1.0 + 0.02 * t_6m);
    
    double df_6m = curve.getDiscountFactor(inputs[0].maturity_date);
    EXPECT_NEAR(df_6m, expected_df_6m, 1e-7);

    // 2. Check 1Y Point
    // T = 1.0
    // Swap pays 2.5% annual. 1.025 * DF = 1.0 => DF = 1/1.025
    double df_1y = curve.getDiscountFactor(inputs[1].maturity_date);
    EXPECT_NEAR(df_1y, 1.0 / 1.025, 1e-7);
}

TEST_F(YieldCurveTest, ThrowsOnUnsortedInput) {
    std::vector<CurveInput> bad_inputs = {
        {make_date(2024, 1, 1), 0.025, 1},
        {make_date(2023, 6, 1), 0.02, 0}
    };
    EXPECT_THROW(YieldCurve(today, bad_inputs), std::invalid_argument);
}

TEST_F(YieldCurveTest, ZeroRateCalculation) {
    YieldCurve curve(today, inputs);
    
    // Check Zero Rate at 1Y
    // r = -ln(DF) / t
    double df_1y = 1.0/1.025;
    double expected_zero = -std::log(df_1y) / 1.0;

    double zero_1y = curve.getZeroRate(inputs[1].maturity_date);
    EXPECT_NEAR(zero_1y, expected_zero, 1e-7);
}
