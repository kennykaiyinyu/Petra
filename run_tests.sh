#!/bin/bash
set -e

PROJECT_ROOT=$(pwd)
echo "Building Petra..."
mkdir -p ../build
cd ../build
cmake "$PROJECT_ROOT" -DCMAKE_BUILD_TYPE=Release
cmake --build . -- -j$(nproc)

echo "Running Unit Tests..."
ctest --output-on-failure

echo "Running Benchmarks..."
./benchmarks/GreekCoreBenchmarks