cmake_minimum_required(VERSION 3.16)
project(GTestGeneratorExample)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

add_executable(mygen_test mygen_test.cpp)
target_link_libraries(mygen_test PRIVATE gtest gtest_main pthread)

add_executable(intensive_test intensive_test.cpp)
target_link_libraries(intensive_test PRIVATE gtest gtest_main pthread)