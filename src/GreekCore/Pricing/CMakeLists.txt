cmake_minimum_required(VERSION 3.10)
project(GreekCorePricingEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
        main.cpp
        pricing/PayOff.cpp
)

add_executable(GreekCore ${SOURCES})

target_include_directories(GreekCore PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
