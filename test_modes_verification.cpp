// test_modes_verification.cpp
// Comprehensive test to verify ALIGNED and FULL modes work correctly
// with detailed output showing all combinations

#include "gtest_generator.h"
#include <gtest/gtest.h>
#include <iostream>
#include <vector>
#include <set>

// ============================================================================
// ALIGNED Mode Tests - Should iterate in parallel (zip)
// ============================================================================

struct AlignedModeTest : ::gtest_generator::TestWithGenerator {
    static std::vector<std::string> combinations;
    static void SetUpTestSuite() {
        combinations.clear();
    }
};

std::vector<std::string> AlignedModeTest::combinations;

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(AlignedModeTest__TwoGeneratorsSameSize);

TEST_G(AlignedModeTest, TwoGeneratorsSameSize) {
    auto a = GENERATOR(1, 2, 3);
    auto b = GENERATOR(10, 20, 30);
    USE_GENERATOR(ALIGNED);

    std::string combo = std::to_string(a) + "," + std::to_string(b);
    combinations.push_back(combo);

    std::cout << "ALIGNED: a=" << a << ", b=" << b << std::endl;

    // In ALIGNED mode, they should zip together
    // Expected combinations: (1,10), (2,20), (3,30)
    EXPECT_TRUE((a == 1 && b == 10) ||
                (a == 2 && b == 20) ||
                (a == 3 && b == 30));
}

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(AlignedModeTest__TwoGeneratorsDifferentSize);

TEST_G(AlignedModeTest, TwoGeneratorsDifferentSize) {
    auto x = GENERATOR(1, 2, 3, 4, 5);  // 5 values
    auto y = GENERATOR(100, 200);        // 2 values
    USE_GENERATOR(ALIGNED);

    std::string combo = std::to_string(x) + "," + std::to_string(y);
    combinations.push_back(combo);

    std::cout << "ALIGNED: x=" << x << ", y=" << y << std::endl;

    // In ALIGNED mode with different sizes, should take min(5,2) = 2 iterations
    // Expected combinations: (1,100), (2,200)
    EXPECT_TRUE((x == 1 && y == 100) ||
                (x == 2 && y == 200));
}

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(AlignedModeTest__ThreeGenerators);

TEST_G(AlignedModeTest, ThreeGenerators) {
    auto a = GENERATOR(1, 2, 3, 4);
    auto b = GENERATOR(10, 20, 30);
    auto c = GENERATOR(100, 200, 300, 400);
    USE_GENERATOR(ALIGNED);

    std::string combo = std::to_string(a) + "," + std::to_string(b) + "," + std::to_string(c);
    combinations.push_back(combo);

    std::cout << "ALIGNED: a=" << a << ", b=" << b << ", c=" << c << std::endl;

    // Min size is 3, so only 3 combinations
    // Expected: (1,10,100), (2,20,200), (3,30,300)
    EXPECT_TRUE((a == 1 && b == 10 && c == 100) ||
                (a == 2 && b == 20 && c == 200) ||
                (a == 3 && b == 30 && c == 300));
}

// ============================================================================
// FULL Mode Tests - Should generate Cartesian product
// ============================================================================

struct FullModeTest : ::gtest_generator::TestWithGenerator {
    static std::set<std::string> combinations;
    static void SetUpTestSuite() {
        combinations.clear();
    }
};

std::set<std::string> FullModeTest::combinations;

TEST_G(FullModeTest, TwoGeneratorsSmall) {
    auto a = GENERATOR(1, 2);
    auto b = GENERATOR(10, 20);
    USE_GENERATOR(FULL);  // Explicit FULL mode (also the default)

    std::string combo = std::to_string(a) + "," + std::to_string(b);
    combinations.insert(combo);

    std::cout << "FULL: a=" << a << ", b=" << b << std::endl;

    // In FULL mode, all combinations should be generated
    // Expected: (1,10), (1,20), (2,10), (2,20) = 2*2 = 4 combinations
    EXPECT_TRUE(a == 1 || a == 2);
    EXPECT_TRUE(b == 10 || b == 20);
}

TEST_G(FullModeTest, TwoGeneratorsDifferentSize) {
    auto x = GENERATOR(1, 2, 3);
    auto y = GENERATOR(100, 200);
    USE_GENERATOR();  // Default is FULL

    std::string combo = std::to_string(x) + "," + std::to_string(y);
    combinations.insert(combo);

    std::cout << "FULL: x=" << x << ", y=" << y << std::endl;

    // FULL mode: 3*2 = 6 combinations
    // (1,100), (1,200), (2,100), (2,200), (3,100), (3,200)
    EXPECT_TRUE(x >= 1 && x <= 3);
    EXPECT_TRUE(y == 100 || y == 200);
}

TEST_G(FullModeTest, ThreeGenerators) {
    auto a = GENERATOR(1, 2);
    auto b = GENERATOR(10, 20);
    auto c = GENERATOR(100, 200);
    USE_GENERATOR(FULL);

    std::string combo = std::to_string(a) + "," + std::to_string(b) + "," + std::to_string(c);
    combinations.insert(combo);

    std::cout << "FULL: a=" << a << ", b=" << b << ", c=" << c << std::endl;

    // FULL mode: 2*2*2 = 8 combinations
    EXPECT_TRUE(a == 1 || a == 2);
    EXPECT_TRUE(b == 10 || b == 20);
    EXPECT_TRUE(c == 100 || c == 200);
}

// ============================================================================
// Verification Tests - Check that correct number of combinations were created
// ============================================================================

TEST(AlignedVerification, TwoGeneratorsSameSize_Count) {
    // Should have 3 combinations from ALIGNED mode
    EXPECT_EQ(AlignedModeTest::combinations.size(), 3);

    std::cout << "\n=== ALIGNED Mode: TwoGeneratorsSameSize ===" << std::endl;
    std::cout << "Total combinations: " << AlignedModeTest::combinations.size() << std::endl;
    std::cout << "Expected: 3 (min of 3,3)" << std::endl;
    std::cout << "Combinations:" << std::endl;
    for (const auto& combo : AlignedModeTest::combinations) {
        std::cout << "  " << combo << std::endl;
    }
}

TEST(FullVerification, TwoGeneratorsSmall_Count) {
    // Should have 4 combinations from FULL mode (2*2)
    EXPECT_EQ(FullModeTest::combinations.size(), 4);

    std::cout << "\n=== FULL Mode: TwoGeneratorsSmall ===" << std::endl;
    std::cout << "Total combinations: " << FullModeTest::combinations.size() << std::endl;
    std::cout << "Expected: 4 (2*2 Cartesian product)" << std::endl;
    std::cout << "Combinations:" << std::endl;
    for (const auto& combo : FullModeTest::combinations) {
        std::cout << "  " << combo << std::endl;
    }
}

// ============================================================================
// Single Generator Test - Should work the same in both modes
// ============================================================================

struct SingleGeneratorTest : ::gtest_generator::TestWithGenerator {
    static int count_aligned;
    static int count_full;
};

int SingleGeneratorTest::count_aligned = 0;
int SingleGeneratorTest::count_full = 0;

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(SingleGeneratorTest__AlignedMode);

TEST_G(SingleGeneratorTest, AlignedMode) {
    auto val = GENERATOR(1, 2, 3, 4, 5);
    USE_GENERATOR(ALIGNED);

    count_aligned++;
    std::cout << "Single ALIGNED: val=" << val << " (iteration " << count_aligned << ")" << std::endl;

    EXPECT_TRUE(val >= 1 && val <= 5);
}

TEST_G(SingleGeneratorTest, FullMode) {
    auto val = GENERATOR(1, 2, 3, 4, 5);
    USE_GENERATOR(FULL);

    count_full++;
    std::cout << "Single FULL: val=" << val << " (iteration " << count_full << ")" << std::endl;

    EXPECT_TRUE(val >= 1 && val <= 5);
}

// ============================================================================
// String Generator Tests
// ============================================================================

struct StringGeneratorTest : ::gtest_generator::TestWithGenerator {
    static std::vector<std::string> aligned_combos;
    static std::set<std::string> full_combos;
};

std::vector<std::string> StringGeneratorTest::aligned_combos;
std::set<std::string> StringGeneratorTest::full_combos;

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(StringGeneratorTest__AlignedStrings);

TEST_G(StringGeneratorTest, AlignedStrings) {
    auto name = GENERATOR("Alice", "Bob", "Charlie");
    auto age = GENERATOR(20, 30, 40);
    USE_GENERATOR(ALIGNED);

    std::string combo = std::string(name) + ":" + std::to_string(age);
    aligned_combos.push_back(combo);

    std::cout << "ALIGNED String: " << name << " is " << age << " years old" << std::endl;

    // Should zip together: (Alice,20), (Bob,30), (Charlie,40)
    EXPECT_TRUE((std::string(name) == "Alice" && age == 20) ||
                (std::string(name) == "Bob" && age == 30) ||
                (std::string(name) == "Charlie" && age == 40));
}

TEST_G(StringGeneratorTest, FullStrings) {
    auto prefix = GENERATOR("Mr", "Ms");
    auto name = GENERATOR("Smith", "Jones");
    USE_GENERATOR(FULL);

    std::string combo = std::string(prefix) + " " + std::string(name);
    full_combos.insert(combo);

    std::cout << "FULL String: " << prefix << " " << name << std::endl;

    // Should create all 4 combinations: 2*2 = 4
}

// ============================================================================
// Main function with summary
// ============================================================================

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);

    std::cout << "\n========================================" << std::endl;
    std::cout << "Testing ALIGNED vs FULL Mode" << std::endl;
    std::cout << "Comprehensive Verification" << std::endl;
    std::cout << "========================================\n" << std::endl;

    std::cout << "ALIGNED Mode: Generators iterate in parallel (like Python zip)" << std::endl;
    std::cout << "  - Takes minimum count across all generators" << std::endl;
    std::cout << "  - Example: GENERATOR(1,2,3) + GENERATOR(10,20) = 2 iterations" << std::endl;
    std::cout << "  - Combinations: (1,10), (2,20)" << std::endl;

    std::cout << "\nFULL Mode: Cartesian product of all generators (default)" << std::endl;
    std::cout << "  - All combinations of all values" << std::endl;
    std::cout << "  - Example: GENERATOR(1,2,3) + GENERATOR(10,20) = 6 iterations" << std::endl;
    std::cout << "  - Combinations: (1,10), (1,20), (2,10), (2,20), (3,10), (3,20)" << std::endl;

    std::cout << "\n========================================\n" << std::endl;

    int result = RUN_ALL_TESTS();

    std::cout << "\n========================================" << std::endl;
    std::cout << "Test Summary" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Single generator ALIGNED iterations: " << SingleGeneratorTest::count_aligned << " (expected: 5)" << std::endl;
    std::cout << "Single generator FULL iterations: " << SingleGeneratorTest::count_full << " (expected: 5)" << std::endl;
    std::cout << "String ALIGNED combinations: " << StringGeneratorTest::aligned_combos.size() << " (expected: 3)" << std::endl;
    std::cout << "String FULL combinations: " << StringGeneratorTest::full_combos.size() << " (expected: 4)" << std::endl;
    std::cout << "========================================\n" << std::endl;

    return result;
}
