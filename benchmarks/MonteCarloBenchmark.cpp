#include <benchmark/benchmark.h>
#include "GreekCore/Pricing/MonteCarlo.h"
#include "GreekCore/Pricing/PayOff.h"
#include "GreekCore/Rates/InterpolatorStrategy.h"
#include <vector>
#include <span>

using namespace GreekCore;

// Profile the Core Monte Carlo Engine
static void BM_MonteCarlo_Call(benchmark::State& state) {
    double S0 = 100.0;
    double r = 0.05;
    double sigma = 0.2;
    double T = 1.0;
    PayOffVanilla payoff(OptionType::Call, 100.0);

    // Number of paths is passed via range argument
    size_t paths = state.range(0);

    for (auto _ : state) {
        // Prevent compiler optimization of the result
        auto result = MonteCarloPricer::priceEuropean(S0, r, sigma, T, paths, payoff);
        benchmark::DoNotOptimize(result);
    }
    
    // Custom Metric: Paths per Second
    state.counters["Paths"] = benchmark::Counter(state.iterations() * paths, benchmark::Counter::kIsRate);
}

// Register benchmark with varying ranges of paths
// Range: 1k to 1M paths
BENCHMARK(BM_MonteCarlo_Call)->Range(1024, 1024 * 1024);

static void BM_Interpolation(benchmark::State& state) {
    // Setup curve
    std::vector<double> times;
    std::vector<double> rates;
    for(int i=0; i < 100; ++i) {
        times.push_back(i * 0.5); // 0, 0.5, 1.0 ...
        rates.push_back(0.02 + i * 0.001);
    }
    
    // Ensure data is in cache before timing
    std::span<const double> t_span(times);
    std::span<const double> r_span(rates);
    
    double x = 15.25; // Random lookup point

    for (auto _ : state) {
        double val = LinearInterpolator::interpolate(x, t_span, r_span);
        benchmark::DoNotOptimize(val);
        x += 0.0001;
        if(x > 49.0) x = 0.0;
    }
}
BENCHMARK(BM_Interpolation);

BENCHMARK_MAIN();
