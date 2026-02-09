add_executable(GreekCoreBenchmarks MonteCarloBenchmark.cpp)

target_link_libraries(GreekCoreBenchmarks PRIVATE 
    GreekCore 
    benchmark::benchmark
    benchmark::benchmark_main
)
