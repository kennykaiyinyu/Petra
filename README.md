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
