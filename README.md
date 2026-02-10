# GreekCore

High-Performance C++20 Option Pricing & Yield Curve Library.

## Structure

- **include/GreekCore/**: Public header-only library (Calculation Engine).
  - **Pricing/**: Option pricing engines and payoffs.
  - **Rates/**: Yield curve construction and interpolation.
  - **Time/**: Date handling, Calendars, and Day Count Conventions.
- **apps/**: Applications built on GreekCore.
  - **MarketMaker/**: Low-latency trading engine.
- **tests/**: Unit and integration tests (GoogleTest).
- **examples/**: Usage examples.
- **benchmarks/**: Performance benchmarks.

## Build

```bash
mkdir build && cd build
cmake ..
cmake --build .
ctest
```

## Requirements

- C++20 Compiler (GCC 10+, Clang 10+, MSVC 19.29+)
- CMake 3.20+

## Key Features

### Asynchronous Monte Carlo Engine
GreekCore now supports asynchronous pricing for long-running simulations, allowing the main application thread to remain responsive.

- **Non-Blocking Execution**: Use `MonteCarloPricer::priceEuropeanAsync` to return a `std::future`.
- **Thread-Safe Monitoring**: Use `StatisticsThreadSafe` (Decorator Pattern) to query simulation progress in real-time from the main thread while the worker thread computes results.
- **Zero-Overhead Pattern**: Supports `std::unique_ptr` and pass-by-reference to avoid shared pointer locking overhead if desired.

See `examples/AsyncMonteCarlo.cpp` for a complete demonstration.
