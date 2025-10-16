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

add_executable(test_aligned_mode test_aligned_mode.cpp)
target_link_libraries(test_aligned_mode PRIVATE gtest gtest_main pthread)

add_executable(test_private_access test_private_access.cpp)
target_link_libraries(test_private_access PRIVATE gtest gtest_main pthread)

add_executable(test_array_compare test_array_compare.cpp)
target_link_libraries(test_array_compare PRIVATE gtest gtest_main pthread)

add_executable(test_mode_counts test_mode_counts.cpp)
target_link_libraries(test_mode_counts PRIVATE gtest gtest_main pthread)

add_executable(test_define_macros test_define_macros.cpp)
target_link_libraries(test_define_macros PRIVATE gtest gtest_main pthread)

add_executable(test_error_handling test_error_handling.cpp)
target_link_libraries(test_error_handling PRIVATE gtest gtest_main pthread)

add_executable(test_aligned_debug test_aligned_debug.cpp)
target_link_libraries(test_aligned_debug PRIVATE gtest gtest_main pthread)

add_executable(test_simple_aligned test_simple_aligned.cpp)
target_link_libraries(test_simple_aligned PRIVATE gtest gtest_main pthread)

add_executable(test_aligned_counting test_aligned_counting.cpp)
target_link_libraries(test_aligned_counting PRIVATE gtest gtest_main pthread)

# Multi-cpp executable to test when library is used in multiple files
add_executable(multi_test mygen_test.cpp additional_test.cpp)
target_link_libraries(multi_test PRIVATE gtest gtest_main pthread)