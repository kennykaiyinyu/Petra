# Petra

Petra is a high-performance quantitative finance library implemented in C++20. It provides core pricing engines and rate modelling tools designed for efficiency and modern C++ practices.

## Features

*   **Yield Curve Bootstrapping**: Supports Deposits, FRAs, and Swaps with configurable interpolation and day count strategies.
*   **Monte Carlo Engine**: High-performance pricing for European and Path-Dependent options, including Greek calculation and async execution.
*   **Binomial Tree**: Pricing for American and European options.
*   **Modern C++**: Utilizes C++20 Concepts, `std::span`, and template strategies for zero-overhead abstraction.
*   **Utilities**: Date arithmetic, Tenor parsing (e.g., "T/N", "3M"), and Brent's solver.

## Build Instructions

### Prerequisites

*   CMake 3.20+
*   C++20 compliant compiler (GCC, Clang, MSVC)

### Building

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -- -j$(nproc)
```

## Running Tests

Unit tests are built using GoogleTest.

```bash
cd build
ctest --output-on-failure
```

## Benchmarks

Performance benchmarks are built using Google Benchmark.

```bash
./build/benchmarks/GreekCoreBenchmarks
```