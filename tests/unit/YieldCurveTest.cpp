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
        
        Date d_1y = today + std::chrono::days(365);
        Date d_2y = today + std::chrono::days(730);
        Date d_6m_exact = today + std::chrono::days(static_cast<int>(365 * 0.5)); // 182 days

        inputs = {
            {InstrumentType::Deposit, 0.02, d_6m_exact, today, 0}, 
            {InstrumentType::Swap, 0.025, d_1y, today, 1}, 
            {InstrumentType::Swap, 0.030, d_2y, today, 1} 
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
        {InstrumentType::Swap, 0.025, make_date(2024, 1, 1), today, 1},
        {InstrumentType::Deposit, 0.02, make_date(2023, 6, 1), today, 0}
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

TEST_F(YieldCurveTest, HandlesFRA) {
    // Add an FRA from 6M to 9M
    // 6M DF is known from first input (~0.990099)
    // FRA Rate 3%
    Date d_6m = inputs[0].maturity_date;
    Date d_9m = today + std::chrono::days(273); // ~0.75y
    
    std::vector<CurveInput> fra_inputs = inputs;
    // Insert FRA after 6M deposit
    fra_inputs.insert(fra_inputs.begin() + 1, {InstrumentType::FRA, 0.03, d_9m, d_6m, 0});
    
    YieldCurve curve(today, fra_inputs);
    
    // Check that the curve bootstrapped successfully
    EXPECT_NO_THROW(auto x = curve.getDiscountFactor(d_9m));
}
