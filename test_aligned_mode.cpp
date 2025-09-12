#include <gtest/gtest.h>
#include "gtest_generator.h"
#include <vector>
#include <string>
#include <set>

// Global static counters for each test to verify execution count
static int g_basic_aligned_count = 0;
static int g_different_sizes_count = 0;
static int g_single_column_count = 0;
static int g_cartesian_count = 0;
static int g_default_full_count = 0;
static int g_string_values_count = 0;
static int g_declaration_order_count = 0;
static int g_complex_types_count = 0;
static int g_run_count_full = 0;
static int g_run_count_aligned = 0;
static int g_empty_gen_count = 0;

// Test fixture for ALIGNED mode tests
class AlignedModeTest : public gtest_generator::TestWithGenerator {
};

// Test fixture for FULL mode backward compatibility tests
class FullModeTest : public gtest_generator::TestWithGenerator {
};

// Test 1: Basic ALIGNED mode with same-size columns
TEST_G(AlignedModeTest, BasicAlignedSameSize) {
    auto a = GENERATOR(1, 2, 3);
    auto b = GENERATOR(10, 20, 30);
    auto c = GENERATOR(100, 200, 300);
    
    USE_GENERATOR(ALIGNED);
    
    g_basic_aligned_count++;
    
    // Should generate 3 runs: (1,10,100), (2,20,200), (3,30,300)
    static std::vector<std::tuple<int, int, int>> results;
    results.push_back({a, b, c});
    
    // Verify results after all runs
    if (GetParam() == 2) { // Last run (0-indexed)
        EXPECT_EQ(g_basic_aligned_count, 3);
        EXPECT_EQ(results.size(), 3);
        EXPECT_EQ(results[0], std::make_tuple(1, 10, 100));
        EXPECT_EQ(results[1], std::make_tuple(2, 20, 200));
        EXPECT_EQ(results[2], std::make_tuple(3, 30, 300));
    }
}

// Test 2: ALIGNED mode with different-size columns
TEST_G(AlignedModeTest, AlignedDifferentSizes) {
    auto x = GENERATOR(1, 2);          // size 2
    auto y = GENERATOR(10, 20, 30, 40); // size 4
    auto z = GENERATOR(100, 200, 300);  // size 3
    
    USE_GENERATOR(ALIGNED);
    
    g_different_sizes_count++;
    
    // Should generate 4 runs (max size)
    // x cycles: 1, 2, 1, 2
    // y cycles: 10, 20, 30, 40
    // z cycles: 100, 200, 300, 100
    static std::vector<std::tuple<int, int, int>> results;
    results.push_back({x, y, z});
    
    if (GetParam() == 3) { // Last run
        EXPECT_EQ(g_different_sizes_count, 4);
        EXPECT_EQ(results.size(), 4);
        EXPECT_EQ(results[0], std::make_tuple(1, 10, 100));
        EXPECT_EQ(results[1], std::make_tuple(2, 20, 200));
        EXPECT_EQ(results[2], std::make_tuple(1, 30, 300));
        EXPECT_EQ(results[3], std::make_tuple(2, 40, 100));
    }
}

// Test 3: ALIGNED mode with single column
TEST_G(AlignedModeTest, SingleColumn) {
    auto val = GENERATOR(5, 10, 15, 20);
    
    USE_GENERATOR(ALIGNED);
    
    g_single_column_count++;
    
    static std::vector<int> results;
    results.push_back(val);
    
    if (GetParam() == 3) { // Last run
        EXPECT_EQ(g_single_column_count, 4);
        EXPECT_EQ(results.size(), 4);
        EXPECT_EQ(results[0], 5);
        EXPECT_EQ(results[1], 10);
        EXPECT_EQ(results[2], 15);
        EXPECT_EQ(results[3], 20);
    }
}

// Test 4: Verify FULL mode still works (backward compatibility)
TEST_G(FullModeTest, CartesianProduct) {
    auto a = GENERATOR(1, 2);
    auto b = GENERATOR(10, 20);
    
    USE_GENERATOR(FULL);  // Explicit FULL mode
    
    g_cartesian_count++;
    
    // Should generate 4 runs: (1,10), (1,20), (2,10), (2,20)
    static std::set<std::pair<int, int>> results;
    results.insert({a, b});
    
    if (GetParam() == 3) { // Last run
        EXPECT_EQ(g_cartesian_count, 4);
        EXPECT_EQ(results.size(), 4);
        EXPECT_TRUE(results.count({1, 10}));
        EXPECT_TRUE(results.count({1, 20}));
        EXPECT_TRUE(results.count({2, 10}));
        EXPECT_TRUE(results.count({2, 20}));
    }
}

// Test 5: Default mode is FULL (backward compatibility)
TEST_G(FullModeTest, DefaultIsFull) {
    auto x = GENERATOR(1, 2, 3);
    auto y = GENERATOR(10, 20);
    
    USE_GENERATOR();  // No parameter = FULL mode
    
    g_default_full_count++;
    
    // Should generate 6 runs (3 * 2)
    if (GetParam() == 5) { // Last run
        EXPECT_EQ(g_default_full_count, 6);
    }
}

// Test 6: ALIGNED mode with strings
TEST_G(AlignedModeTest, StringValues) {
    auto str = GENERATOR(std::string("A"), std::string("B"), std::string("C"));
    auto num = GENERATOR(1, 2, 3);
    
    USE_GENERATOR(ALIGNED);
    
    g_string_values_count++;
    
    static std::vector<std::pair<std::string, int>> results;
    results.push_back({str, num});
    
    if (GetParam() == 2) { // Last run
        EXPECT_EQ(g_string_values_count, 3);
        EXPECT_EQ(results.size(), 3);
        EXPECT_EQ(results[0], std::make_pair(std::string("A"), 1));
        EXPECT_EQ(results[1], std::make_pair(std::string("B"), 2));
        EXPECT_EQ(results[2], std::make_pair(std::string("C"), 3));
    }
}

// Test 7: ALIGNED mode preserves declaration order
TEST_G(AlignedModeTest, DeclarationOrder) {
    // Declare in specific order
    auto third = GENERATOR(300, 301);
    auto first = GENERATOR(100, 101);
    auto second = GENERATOR(200, 201);
    
    USE_GENERATOR(ALIGNED);
    
    g_declaration_order_count++;
    
    static std::vector<std::tuple<int, int, int>> results;
    results.push_back({third, first, second});
    
    if (GetParam() == 1) { // Last run
        EXPECT_EQ(g_declaration_order_count, 2);
        EXPECT_EQ(results.size(), 2);
        // Values should align by position, not by variable name
        EXPECT_EQ(results[0], std::make_tuple(300, 100, 200));
        EXPECT_EQ(results[1], std::make_tuple(301, 101, 201));
    }
}

// Test 8: Complex type with ALIGNED mode
struct Point {
    int x, y;
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};

TEST_G(AlignedModeTest, ComplexTypes) {
    auto p = GENERATOR(Point{1, 1}, Point{2, 2});
    auto scale = GENERATOR(10, 20);
    
    USE_GENERATOR(ALIGNED);
    
    g_complex_types_count++;
    
    static std::vector<std::pair<Point, int>> results;
    results.push_back({p, scale});
    
    if (GetParam() == 1) { // Last run
        EXPECT_EQ(g_complex_types_count, 2);
        EXPECT_EQ(results.size(), 2);
        EXPECT_EQ(results[0].first.x, 1);
        EXPECT_EQ(results[0].first.y, 1);
        EXPECT_EQ(results[0].second, 10);
        EXPECT_EQ(results[1].first.x, 2);
        EXPECT_EQ(results[1].first.y, 2);
        EXPECT_EQ(results[1].second, 20);
    }
}

// Test 9: Verify run count difference between modes
TEST_G(FullModeTest, RunCountFull) {
    auto a = GENERATOR(1, 2, 3);
    auto b = GENERATOR(10, 20);
    auto c = GENERATOR(100, 200);
    
    USE_GENERATOR(FULL);
    
    g_run_count_full++;
    
    if (GetParam() == 11) { // Last run (3*2*2 - 1 = 11)
        EXPECT_EQ(g_run_count_full, 12); // Cartesian product
    }
}

TEST_G(AlignedModeTest, RunCountAligned) {
    auto a = GENERATOR(1, 2, 3);
    auto b = GENERATOR(10, 20);
    auto c = GENERATOR(100, 200);
    
    USE_GENERATOR(ALIGNED);
    
    g_run_count_aligned++;
    
    if (GetParam() == 2) { // Last run (max(3,2,2) - 1 = 2)
        EXPECT_EQ(g_run_count_aligned, 3); // Max column size
    }
}

// Test 10: Empty generator list edge case
TEST_G(AlignedModeTest, EmptyGenerator) {
    // At least one generator is required
    auto val = GENERATOR(42);
    
    USE_GENERATOR(ALIGNED);
    
    g_empty_gen_count++;
    
    EXPECT_EQ(val, 42);
    EXPECT_EQ(g_empty_gen_count, 1);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    int result = RUN_ALL_TESTS();
    
    // Verify all test execution counts
    printf("\n=== Test Execution Count Summary ===\n");
    printf("BasicAlignedSameSize: %d (expected: 3)\n", g_basic_aligned_count);
    printf("AlignedDifferentSizes: %d (expected: 4)\n", g_different_sizes_count);
    printf("SingleColumn: %d (expected: 4)\n", g_single_column_count);
    printf("CartesianProduct: %d (expected: 4)\n", g_cartesian_count);
    printf("DefaultIsFull: %d (expected: 6)\n", g_default_full_count);
    printf("StringValues: %d (expected: 3)\n", g_string_values_count);
    printf("DeclarationOrder: %d (expected: 2)\n", g_declaration_order_count);
    printf("ComplexTypes: %d (expected: 2)\n", g_complex_types_count);
    printf("RunCountFull: %d (expected: 12)\n", g_run_count_full);
    printf("RunCountAligned: %d (expected: 3)\n", g_run_count_aligned);
    printf("EmptyGenerator: %d (expected: 1)\n", g_empty_gen_count);
    
    // Verify counts match expectations
    bool counts_ok = 
        g_basic_aligned_count == 3 &&
        g_different_sizes_count == 4 &&
        g_single_column_count == 4 &&
        g_cartesian_count == 4 &&
        g_default_full_count == 6 &&
        g_string_values_count == 3 &&
        g_declaration_order_count == 2 &&
        g_complex_types_count == 2 &&
        g_run_count_full == 12 &&
        g_run_count_aligned == 3 &&
        g_empty_gen_count == 1;
    
    if (!counts_ok) {
        printf("\nERROR: Test execution counts do not match expected values!\n");
        return 1;
    }
    
    printf("\nAll test execution counts match expected values!\n");
    return result;
}