cmake_minimum_required(VERSION 3.20)

project(GreekCore VERSION 0.1.0 LANGUAGES CXX)

# --- Modern C++20 Standard ---
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# define sources
set(SOURCES 
    src/GreekCore/Pricing/BinomialTree.cpp
    src/GreekCore/Rates/Tenor.cpp
    src/GreekCore/Numerics/BrentSolver.cpp
    src/GreekCore/Rates/Interpolator.cpp
    src/GreekCore/Numerics/RNG.cpp
    src/GreekCore/Pricing/PayOff.cpp
    src/GreekCore/Pricing/MonteCarlo.cpp
    src/GreekCore/Pricing/Parameters.cpp
    src/GreekCore/Numerics/Statistics.cpp
    src/GreekCore/Rates/YieldCurve.cpp
    src/GreekCore/Time/Date.cpp
    src/GreekCore/Time/Calendar.cpp
    src/GreekCore/Time/DayCounter.cpp
)

# --- Component Library ---
add_library(GreekCore STATIC ${SOURCES})
target_include_directories(GreekCore PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# --- Dependencies (GoogleTest) ---
include(CTest)
option(BUILD_TESTING "Build the testing tree." ON)
if(BUILD_TESTING)
    include(FetchContent)
    FetchContent_Declare(
      googletest
      URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    )
    # Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    # Add test subdirectory
    add_subdirectory(tests)
endif()

# --- Examples ---
option(BUILD_EXAMPLES "Build examples" ON)
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# --- Benchmarks ---
option(BUILD_BENCHMARKS "Build benchmarks" ON)
if(BUILD_BENCHMARKS)
    include(FetchContent)
    FetchContent_Declare(
      googlebenchmark
      URL https://github.com/google/benchmark/archive/refs/tags/v1.8.3.zip
    )
    set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
    set(BENCHMARK_DOWNLOAD_DEPENDENCIES ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googlebenchmark)

    add_subdirectory(benchmarks)

# Applications
add_subdirectory(apps/MarketMaker)
endif()
# Example showing the Bridge logic from screenshots
add_executable(BridgeUsage examples/BridgeUsage.cpp)
target_link_libraries(BridgeUsage PRIVATE GreekCore)