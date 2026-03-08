add_executable(GreekCoreBenchmarks MonteCarloBenchmark.cpp)

add_executable(GreekCoreBenchmarks_Stats BenchmarkStatistics.cpp)
target_link_libraries(GreekCoreBenchmarks_Stats PRIVATE 
    GreekCore 
    benchmark::benchmark
    benchmark::benchmark_main
)

target_link_libraries(GreekCoreBenchmarks PRIVATE 
    GreekCore 
    benchmark::benchmark
    benchmark::benchmark_main
)

add_executable(GreekCoreBenchmarks_Rates RatesBenchmark.cpp)
target_link_libraries(GreekCoreBenchmarks_Rates PRIVATE GreekCore benchmark::benchmark benchmark::benchmark_main)
