# For FetchContent_MakeAvailable
cmake_minimum_required(VERSION 3.14)

project(aarch64-instruction-performance LANGUAGES CXX)

include(FetchContent)

FetchContent_Declare(
  cpu_features
  GIT_REPOSITORY https://github.com/google/cpu_features
  GIT_TAG v0.11.0
)
FetchContent_MakeAvailable(cpu_features)

FetchContent_Declare(
  xbyak_aarch64
  GIT_REPOSITORY https://github.com/fujitsu/xbyak_aarch64
  # v1.3.0 as in commit message
  GIT_TAG 3f8c682b9c6ff562dc008c7d1b8307a683f05d20
)
FetchContent_MakeAvailable(xbyak_aarch64)

FetchContent_Declare(
  counters
  GIT_REPOSITORY https://github.com/lemire/counters
  GIT_TAG v4.1.0
)
FetchContent_MakeAvailable(counters)

FetchContent_Declare(
  benchmark
  GIT_REPOSITORY https://github.com/google/benchmark
  GIT_TAG v1.9.5
)
set(BENCHMARK_ENABLE_TESTING OFF CACHE INTERNAL "")
FetchContent_MakeAvailable(benchmark)

add_compile_options(-W -Wall -Wextra)

add_executable(benchmark-neon benchmark-neon.cpp)
target_link_libraries(benchmark-neon PRIVATE xbyak_aarch64::xbyak_aarch64 counters::counters benchmark::benchmark)

add_executable(benchmark-sve benchmark-sve.cpp)
target_link_libraries(benchmark-sve PRIVATE CpuFeatures::cpu_features xbyak_aarch64::xbyak_aarch64 counters::counters benchmark::benchmark)
