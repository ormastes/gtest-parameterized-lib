cmake_minimum_required(VERSION 3.16)
project(GTestGeneratorExample)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)
include(GoogleTest)
enable_testing()

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

# Removed - has test suite redefinition issue, functionality covered by test_private_access
# add_executable(test_friend_access test_friend_access.cpp)
# target_link_libraries(test_friend_access PRIVATE gtest gtest_main pthread)

# Removed - test_class_friend_access.cpp doesn't exist
# add_executable(test_class_friend_access test_class_friend_access.cpp)
# target_link_libraries(test_class_friend_access PRIVATE gtest gtest_main pthread)

# Multi-cpp executable to test when library is used in multiple files
add_executable(multi_test mygen_test.cpp additional_test.cpp)
target_link_libraries(multi_test PRIVATE gtest gtest_main pthread)

# Multi-cpp executable to test TEST_F_FRIEND and TEST_G_FRIEND across files
add_executable(multi_friend_test test_friend_multi_file1.cpp test_friend_multi_file2.cpp)
target_link_libraries(multi_friend_test PRIVATE gtest gtest_main pthread)

# Test for TEST_G_FRIEND with ALIGNED and FULL generation modes
add_executable(test_friend_generation_modes test_friend_generation_modes.cpp)
target_link_libraries(test_friend_generation_modes PRIVATE gtest gtest_main pthread)

# Comparison test between TEST_G and TEST_G_FRIEND generation
add_executable(test_generation_comparison test_generation_comparison.cpp)
target_link_libraries(test_generation_comparison PRIVATE gtest gtest_main pthread)

# Simple TEST_G_FRIEND generation test
add_executable(test_friend_simple_gen test_friend_simple_gen.cpp)
target_link_libraries(test_friend_simple_gen PRIVATE gtest gtest_main pthread)

# Working TEST_G_FRIEND with ALIGNED and FULL modes
add_executable(test_friend_modes_working test_friend_modes_working.cpp)
target_link_libraries(test_friend_modes_working PRIVATE gtest gtest_main pthread)

set(GTEST_GENERATOR_TEST_TARGETS
    mygen_test
    intensive_test
    test_aligned_mode
    test_private_access
    test_array_compare
    test_mode_counts
    test_define_macros
    test_error_handling
    test_aligned_debug
    test_simple_aligned
    test_aligned_counting
    multi_test
    multi_friend_test
    test_friend_generation_modes
    test_generation_comparison
    test_friend_simple_gen
    test_friend_modes_working)

foreach(target IN LISTS GTEST_GENERATOR_TEST_TARGETS)
  gtest_discover_tests(${target}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DISCOVERY_TIMEOUT 30
  )
endforeach()
