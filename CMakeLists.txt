# For FetchContent_MakeAvailable
cmake_minimum_required(VERSION 3.14)

project(aarch64-instruction-performance LANGUAGES CXX)

include(CTest)
include(FetchContent)

FetchContent_Declare(
  xbyak_aarch64
  GIT_REPOSITORY https://github.com/fujitsu/xbyak_aarch64
  GIT_TAG v1.1.3
)
FetchContent_MakeAvailable(xbyak_aarch64)

FetchContent_Declare(
  counters
  GIT_REPOSITORY https://github.com/lemire/counters
  GIT_TAG v4.1.0
)
FetchContent_MakeAvailable(counters)

find_package(benchmark REQUIRED)

add_compile_options(-W -Wall -Wextra)

add_executable(benchmark-neon benchmark-neon.cpp)
target_link_libraries(benchmark-neon PRIVATE xbyak_aarch64::xbyak_aarch64 counters::counters benchmark::benchmark)
