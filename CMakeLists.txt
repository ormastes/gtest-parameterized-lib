cmake_minimum_required(VERSION 3.16)
project(GTestGeneratorExample)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

add_executable(test_mygen test_mygen.cpp)
target_link_libraries(test_mygen PRIVATE gtest gtest_main pthread)