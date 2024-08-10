cmake_minimum_required(VERSION 3.14)
project(simd-test)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest)

FetchContent_Declare(
  benchmark
  GIT_REPOSITORY https://github.com/google/benchmark.git
  GIT_TAG v1.8.3
)
FetchContent_MakeAvailable(benchmark)

include_directories(${CMAKE_SOURCE_DIR})

add_executable(test test.cpp)
target_link_libraries(test gtest gtest_main)

add_executable(bench bench.cpp)
target_link_libraries(bench benchmark benchmark_main)
