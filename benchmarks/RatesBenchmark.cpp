#include <benchmark/benchmark.h>
#include "GreekCore/Rates/Tenor.h"
#include "GreekCore/Rates/InterpolatorStrategy.h"
#include "GreekCore/Rates/YieldCurve.h"
#include "GreekCore/Time/Date.h"
#include "GreekCore/Time/Calendar.h"
#include <vector>
#include <string>

using namespace GreekCore::Rates;

static void BM_TenorParse_Valid(benchmark::State& state) {
    for (auto _ : state) {
        benchmark::DoNotOptimize(Tenor::parse_noexcept("3M"));
    }
}
BENCHMARK(BM_TenorParse_Valid);

static void BM_TenorParse_Invalid_NoExcept(benchmark::State& state) {
    for (auto _ : state) {
        benchmark::DoNotOptimize(Tenor::parse_noexcept("INVALID"));
    }
}
BENCHMARK(BM_TenorParse_Invalid_NoExcept);

static void BM_TenorParse_Invalid_Exception(benchmark::State& state) {
    for (auto _ : state) {
        try {
            benchmark::DoNotOptimize(Tenor::parse("INVALID"));
        } catch (const std::exception&) {
        }
    }
}
BENCHMARK(BM_TenorParse_Invalid_Exception);

static void BM_LinearInterpolator(benchmark::State& state) {
    std::vector<double> x_vals = {1.0, 2.0, 3.0, 4.0, 5.0};
    std::vector<double> y_vals = {0.05, 0.06, 0.07, 0.08, 0.09};
    double x = 2.5;
    for (auto _ : state) {
        benchmark::DoNotOptimize(GreekCore::LinearInterpolator::interpolate(x, x_vals, y_vals));
    }
}
BENCHMARK(BM_LinearInterpolator);

static void BM_YieldCurveBootstrapping(benchmark::State& state) {
    using namespace GreekCore;
    using namespace std::chrono;
    
    // Some valid date
    Date ref_date = Date{year_month_day{year{2023}, month{1}, day{1}}};
    
    std::vector<CurveInput> instruments = {
        {InstrumentType::Deposit, 0.05, Tenor::parse("3M").add_to(ref_date), ref_date, 0},
        {InstrumentType::Deposit, 0.055, Tenor::parse("6M").add_to(ref_date), ref_date, 0},
        {InstrumentType::FRA, 0.06, Tenor::parse("9M").add_to(ref_date), Tenor::parse("6M").add_to(ref_date), 0},
        {InstrumentType::Swap, 0.065, Tenor::parse("1Y").add_to(ref_date), ref_date, 2},
        {InstrumentType::Swap, 0.07, Tenor::parse("2Y").add_to(ref_date), ref_date, 2},
        {InstrumentType::Swap, 0.075, Tenor::parse("3Y").add_to(ref_date), ref_date, 2}
    };
    
    for (auto _ : state) {
        YieldCurve curve(ref_date, instruments);
        benchmark::DoNotOptimize(curve.getDiscountFactor(1.5));
    }
}
BENCHMARK(BM_YieldCurveBootstrapping);

BENCHMARK_MAIN();
