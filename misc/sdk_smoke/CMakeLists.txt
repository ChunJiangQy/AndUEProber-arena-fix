# Compiles every translation unit of a generated SDK:
#   cmake -S misc/sdk_smoke -B build/sdk_smoke -DSDK_DIR=<dump>/SDK && cmake --build build/sdk_smoke
cmake_minimum_required(VERSION 3.20)
project(sdk_smoke LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SDK_DIR "" CACHE PATH "Generated SDK directory containing SDK.hpp")
if(NOT EXISTS "${SDK_DIR}/SDK.hpp")
    message(FATAL_ERROR "Set SDK_DIR to a generated SDK directory containing SDK.hpp")
endif()

file(GLOB_RECURSE SDK_SOURCES CONFIGURE_DEPENDS "${SDK_DIR}/*.cpp")
add_library(sdk_smoke OBJECT ${SDK_SOURCES} "${CMAKE_CURRENT_SOURCE_DIR}/Demo.cpp")
target_include_directories(sdk_smoke PRIVATE "${SDK_DIR}")

# Generated UE headers rely on offsetof over non-standard-layout classes and on
# engine literal conventions.
target_compile_options(sdk_smoke PRIVATE
    -Wno-invalid-offsetof
    -Wno-class-conversion
    -Wno-return-stack-address
    -Wno-unused-value
    -Wno-multichar
    -Wno-invalid-source-encoding
)
