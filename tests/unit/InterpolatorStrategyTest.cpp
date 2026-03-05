#include <gtest/gtest.h>
#include <GreekCore/Rates/InterpolatorStrategy.h>
#include <vector>

class InterpolatorStrategyTest : public ::testing::Test {
protected:
    std::vector<double> x_vals = {1.0, 2.0, 3.0, 5.0};
    std::vector<double> y_vals = {0.04, 0.045, 0.048, 0.05};
};

TEST_F(InterpolatorStrategyTest, InterpolatesMidpointsCorrectly) {
    // 1.5 years is exactly halfway between 1.0 (4%) and 2.0 (4.5%)
    // Expected: 4.25%
    double result = GreekCore::LinearInterpolator::interpolate(1.5, x_vals, y_vals);
    
    EXPECT_NEAR(result, 0.0425, 1e-9);
}

TEST_F(InterpolatorStrategyTest, ExtrapolatesFlatCorrectly) {
    // Below the first point (should return 1.0y rate)
    EXPECT_DOUBLE_EQ(GreekCore::LinearInterpolator::interpolate(0.5, x_vals, y_vals), 0.04);
    // Above the last point (should return 5.0y rate)
    EXPECT_DOUBLE_EQ(GreekCore::LinearInterpolator::interpolate(10.0, x_vals, y_vals), 0.05);
}

TEST_F(InterpolatorStrategyTest, ThrowsOnSizeMismatch) {
    std::vector<double> bad_y = {0.04, 0.05}; // Too short
    EXPECT_THROW(GreekCore::LinearInterpolator::interpolate(1.5, x_vals, bad_y), std::invalid_argument);
}