#include <gtest/gtest.h>
#include <cmath>
#include "GreekCore/Pricing/BinomialTree.h"
#include "GreekCore/Pricing/PayOff.h"

using namespace GreekCore;

TEST(BinomialTreeTest, ConvergesToBlackScholes_European) {
    // Data: S=100, K=100, r=5%, vol=20%, T=1y
    // BS Price: ~10.4506
    double S = 100.0;
    double K = 100.0;
    double r = 0.05;
    double sigma = 0.2;
    double T = 1.0;
    int steps = 500; // Requires decent steps for 1e-2 accuracy

    PayOffVanilla payoff(OptionType::Call, K);
    auto res = BinomialTreePricer::price(S, r, sigma, T, steps, payoff, ExerciseType::European);

    EXPECT_NEAR(res.price, 10.45, 0.05);

    // Check Greeks
    // BS Delta: N(d1) ~ 0.6368
    EXPECT_NEAR(res.delta, 0.637, 0.01);
    
    // BS Gamma: N'(d1)/(S*vol*sqrt(T)) ~ 0.352 / (100*0.2*1) = 0.0176
    EXPECT_NEAR(res.gamma, 0.0176, 0.002);
}

TEST(BinomialTreeTest, AmericanPut_Optimization) {
    // Deep ITM Put with positive rates -> Early Exercise
    double S = 80.0;
    double K = 100.0;
    double r = 0.10; // High rate emphasizes early exercise benefit
    double sigma = 0.2;
    double T = 1.0;
    int steps = 100;

    PayOffVanilla payoff(OptionType::Put, K);
    auto euro = BinomialTreePricer::price(S, r, sigma, T, steps, payoff, ExerciseType::European);
    auto amer = BinomialTreePricer::price(S, r, sigma, T, steps, payoff, ExerciseType::American);

    // American Put should be strictly greater than European
    EXPECT_GT(amer.price, euro.price);

    // Intrinsic Value is 20. American Option price >= Intrinsic
    EXPECT_GE(amer.price, 20.0);
}

TEST(BinomialTreeTest, CalculatesGreeksSensibly) {
    // ATM Option has max time value, so meaningful Greeks
    double S = 100.0;
    double K = 100.0;
    double r = 0.05;
    double sigma = 0.2;
    double T = 1.0;
    int steps = 50; 

    PayOffVanilla payoff(OptionType::Call, K);
    auto res = BinomialTreePricer::price(S, r, sigma, T, steps, payoff, ExerciseType::European);

    // Delta ~ 0.6
    EXPECT_GT(res.delta, 0.5);
    EXPECT_LT(res.delta, 0.7);

    // Gamma > 0 for long option
    EXPECT_GT(res.gamma, 0.0);

    // Theta < 0 (Time decay)
    EXPECT_LT(res.theta, -0.01);
}


TEST(BinomialTreeTest, PutCallParity_European) {
    // C - P = S - K*exp(-rT)
    double S = 100.0;
    double K = 110.0;
    double r = 0.05;
    double sigma = 0.3;
    double T = 0.5;
    int steps = 100;
    
    PayOffVanilla call_payoff(OptionType::Call, K);
    PayOffVanilla put_payoff(OptionType::Put, K);

    auto call = BinomialTreePricer::price(S, r, sigma, T, steps, call_payoff, ExerciseType::European);
    auto put = BinomialTreePricer::price(S, r, sigma, T, steps, put_payoff, ExerciseType::European);


    double rhs = S - K * std::exp(-r * T);
    double lhs = call.price - put.price;

    EXPECT_NEAR(lhs, rhs, 1e-3);
}
