#include <gtest/gtest.h>
#include "GreekCore/Numerics/BrentSolver.h"
#include <cmath>

using namespace GreekCore;

// Test standard polynomial: x^2 - 4 = 0 -> Roots at 2, -2
TEST(BrentSolverTest, FindsRootsOfPolynomial) {
    auto func = [](double x) { return x * x - 4.0; };
    
    // Test bracket [0, 5], expected root 2
    auto result = BrentSolver::solve(func, 0.0, 5.0);
    EXPECT_TRUE(result.converged);
    EXPECT_NEAR(result.root, 2.0, 1e-8);

    // Test bracket [-5, 0], expected root -2
    auto result2 = BrentSolver::solve(func, -5.0, 0.0);
    EXPECT_TRUE(result2.converged);
    EXPECT_NEAR(result2.root, -2.0, 1e-8);
}

// Test trigonometric function: sin(x) = 0 -> Root at PI
TEST(BrentSolverTest, FindsRootOfTrigFunction) {
    auto func = [](double x) { return std::sin(x); };
    
    // Bracket [3, 4], expect PI ~ 3.14159...
    auto result = BrentSolver::solve(func, 3.0, 4.0);
    EXPECT_TRUE(result.converged);
    EXPECT_NEAR(result.root, M_PI, 1e-8);
}

// Test difficult case: x * exp(-x) = 0.2
// Use x - 0.2*exp(x) = 0 or similar for testing
// Let's use f(x) = x^3 - 2x - 5 which has a root around 2.09455
TEST(BrentSolverTest, FindsRootOfCubic) {
    auto func = [](double x) { return x*x*x - 2*x - 5; };
    
    // Bracket [1, 3]
    auto result = BrentSolver::solve(func, 1.0, 3.0);
    EXPECT_TRUE(result.converged);
    EXPECT_NEAR(result.root, 2.09455148, 1e-6);
}

TEST(BrentSolverTest, FailsWithoutBracket) {
    auto func = [](double x) { return x * x + 1.0; }; // No real roots
    
    // Bracket [-1, 1], both positive
    auto result = BrentSolver::solve(func, -1.0, 1.0);
    EXPECT_FALSE(result.converged);
    EXPECT_EQ(result.root, 0.0); // Fail sentinel
}

TEST(BrentSolverTest, FindsExactRootAtBound) {
    auto func = [](double x) { return x - 5.0; };
    
    // Bound is the root
    auto result = BrentSolver::solve(func, 5.0, 10.0);
    EXPECT_TRUE(result.converged);
    EXPECT_DOUBLE_EQ(result.root, 5.0);
}
