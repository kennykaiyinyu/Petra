#include <benchmark/benchmark.h>
#include <thread>
#include "GreekCore/Numerics/Statistics.h"

// Define a simple task to simulate a single Monte Carlo path
static void BM_StatisticsMean_Mutex(benchmark::State& state) {
    GreekCore::StatisticsMean base;
    GreekCore::StatisticsThreadSafe<GreekCore::StatisticsMean> gatherer(std::move(base));
    
    for (auto _ : state) {
        gatherer.dumpOneResult(1.0);
    }
}

static void BM_StatisticsMean_LockFree(benchmark::State& state) {
    GreekCore::StatisticsMeanLockFree gatherer;
    
    for (auto _ : state) {
        gatherer.dumpOneResult(1.0);
    }
}

static void BM_StatisticsMean_Mutex_Contention(benchmark::State& state) {
    static GreekCore::StatisticsMean base_static;
    static GreekCore::StatisticsThreadSafe<GreekCore::StatisticsMean> shared_gatherer(std::move(base_static));
    
    if (state.thread_index() == 0) {
        // Reset or setup if needed
    }
    
    for (auto _ : state) {
        shared_gatherer.dumpOneResult(1.0);
    }
}

static void BM_StatisticsMean_LockFree_Contention(benchmark::State& state) {
    static GreekCore::StatisticsMeanLockFree shared_gatherer;
    
    for (auto _ : state) {
        shared_gatherer.dumpOneResult(1.0);
    }
}

BENCHMARK(BM_StatisticsMean_Mutex);
BENCHMARK(BM_StatisticsMean_LockFree);
BENCHMARK(BM_StatisticsMean_Mutex_Contention)->Threads(4);
BENCHMARK(BM_StatisticsMean_LockFree_Contention)->Threads(4);

BENCHMARK_MAIN();
