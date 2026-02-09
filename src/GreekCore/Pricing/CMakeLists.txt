cmake_minimum_required(VERSION 3.10)
project(GreekCorePricingEngine)

# Set C++ Standard (C++17 recommended for modern features)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define source files
# Includes main.cpp and the implementation file for PayOff
# If PayOff is header-only, remove pricing/PayOff.cpp
set(SOURCES
        main.cpp
        pricing/PayOff.cpp
)

# Create the executable
add_executable(GreekCore ${SOURCES})

# Add the current source directory to the include path
# This allows #include "pricing/PayOff.h" to resolve correctly
target_include_directories(GreekCore PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
