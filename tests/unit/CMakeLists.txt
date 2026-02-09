add_executable(GreekCoreUnitTests InterpolatorTest.cpp BrentSolverTest.cpp YieldCurveTest.cpp MonteCarloTest.cpp TimeTest.cpp TenorTest.cpp BinomialTreeTest.cpp)

target_link_libraries(GreekCoreUnitTests PRIVATE 
    GreekCore 
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(GreekCoreUnitTests)
