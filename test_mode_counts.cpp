#include "gtest_generator.h"
#include <vector>
#include <tuple>
#include <set>

// Test fixture
class ModeCountTest : public ::gtest_generator::TestWithGenerator {
};

// Global counters and result storage
static int g_full_3x2x2_count = 0;
static int g_aligned_3x2x2_count = 0;
static int g_full_2x3x4_count = 0;
static int g_aligned_2x3x4_count = 0;
static int g_full_5x1x3_count = 0;
static int g_aligned_5x1x3_count = 0;

static std::vector<std::tuple<int, int, int>> g_full_3x2x2_results;
static std::vector<std::tuple<int, int, int>> g_aligned_3x2x2_results;
static std::vector<std::tuple<int, int, int>> g_full_2x3x4_results;
static std::vector<std::tuple<int, int, int>> g_aligned_2x3x4_results;
static std::vector<std::tuple<int, int, int>> g_full_5x1x3_results;
static std::vector<std::tuple<int, int, int>> g_aligned_5x1x3_results;

// Test 1: FULL mode with 3x2x2 columns (should generate 12 test cases)
TEST_G(ModeCountTest, Full_3x2x2) {
    int a = GENERATOR(1, 2, 3);        // 3 values
    int b = GENERATOR(10, 20);         // 2 values
    int c = GENERATOR(100, 200);       // 2 values
    USE_GENERATOR(FULL);

    g_full_3x2x2_count++;
    g_full_3x2x2_results.push_back({a, b, c});

    printf("[FULL 3x2x2] Run %2d: a=%d, b=%d, c=%d\n", GetParam(), a, b, c);
}

// Test 2: ALIGNED mode with 3x2x2 columns (should generate 3 test cases - max column size)
TEST_G(ModeCountTest, Aligned_3x2x2) {
    int a = GENERATOR(1, 2, 3);        // 3 values
    int b = GENERATOR(10, 20);         // 2 values
    int c = GENERATOR(100, 200);       // 2 values
    USE_GENERATOR(ALIGNED);

    g_aligned_3x2x2_count++;
    g_aligned_3x2x2_results.push_back({a, b, c});

    printf("[ALIGNED 3x2x2] Run %2d: a=%d, b=%d, c=%d\n", GetParam(), a, b, c);
}

// Test 3: FULL mode with 2x3x4 columns (should generate 24 test cases)
TEST_G(ModeCountTest, Full_2x3x4) {
    int x = GENERATOR(5, 6);                    // 2 values
    int y = GENERATOR(50, 60, 70);              // 3 values
    int z = GENERATOR(500, 600, 700, 800);      // 4 values
    USE_GENERATOR(FULL);

    g_full_2x3x4_count++;
    g_full_2x3x4_results.push_back({x, y, z});

    printf("[FULL 2x3x4] Run %2d: x=%d, y=%d, z=%d\n", GetParam(), x, y, z);
}

// Test 4: ALIGNED mode with 2x3x4 columns (should generate 4 test cases - max column size)
TEST_G(ModeCountTest, Aligned_2x3x4) {
    int x = GENERATOR(5, 6);                    // 2 values
    int y = GENERATOR(50, 60, 70);              // 3 values
    int z = GENERATOR(500, 600, 700, 800);      // 4 values
    USE_GENERATOR(ALIGNED);

    g_aligned_2x3x4_count++;
    g_aligned_2x3x4_results.push_back({x, y, z});

    printf("[ALIGNED 2x3x4] Run %2d: x=%d, y=%d, z=%d\n", GetParam(), x, y, z);
}

// Test 5: FULL mode with 5x1x3 columns (should generate 15 test cases)
TEST_G(ModeCountTest, Full_5x1x3) {
    int p = GENERATOR(1, 2, 3, 4, 5);           // 5 values
    int q = GENERATOR(99);                       // 1 value
    int r = GENERATOR(11, 22, 33);              // 3 values
    USE_GENERATOR(FULL);

    g_full_5x1x3_count++;
    g_full_5x1x3_results.push_back({p, q, r});

    printf("[FULL 5x1x3] Run %2d: p=%d, q=%d, r=%d\n", GetParam(), p, q, r);
}

// Test 6: ALIGNED mode with 5x1x3 columns (should generate 5 test cases - max column size)
TEST_G(ModeCountTest, Aligned_5x1x3) {
    int p = GENERATOR(1, 2, 3, 4, 5);           // 5 values
    int q = GENERATOR(99);                       // 1 value
    int r = GENERATOR(11, 22, 33);              // 3 values
    USE_GENERATOR(ALIGNED);

    g_aligned_5x1x3_count++;
    g_aligned_5x1x3_results.push_back({p, q, r});

    printf("[ALIGNED 5x1x3] Run %2d: p=%d, q=%d, r=%d\n", GetParam(), p, q, r);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    int result = RUN_ALL_TESTS();

    printf("\n");
    printf("========================================\n");
    printf("=== Mode Count Verification Summary ===\n");
    printf("========================================\n\n");

    // Test 1: FULL 3x2x2
    printf("Test 1: FULL mode 3x2x2\n");
    printf("  Expected: 12 test cases (3 × 2 × 2)\n");
    printf("  Actual:   %d test cases\n", g_full_3x2x2_count);
    printf("  Status:   %s\n", g_full_3x2x2_count == 12 ? "✓ PASS" : "✗ FAIL");
    printf("  Unique combinations: %zu\n", std::set<std::tuple<int,int,int>>(
        g_full_3x2x2_results.begin(), g_full_3x2x2_results.end()).size());
    printf("\n");

    // Test 2: ALIGNED 3x2x2
    printf("Test 2: ALIGNED mode 3x2x2\n");
    printf("  Expected: 3 test cases (max column size)\n");
    printf("  Actual:   %d test cases\n", g_aligned_3x2x2_count);
    printf("  Status:   %s\n", g_aligned_3x2x2_count == 3 ? "✓ PASS" : "✗ FAIL");
    printf("  Results:\n");
    for (size_t i = 0; i < g_aligned_3x2x2_results.size(); i++) {
        auto [a, b, c] = g_aligned_3x2x2_results[i];
        printf("    Run %zu: (%d, %d, %d)\n", i, a, b, c);
    }
    printf("\n");

    // Test 3: FULL 2x3x4
    printf("Test 3: FULL mode 2x3x4\n");
    printf("  Expected: 24 test cases (2 × 3 × 4)\n");
    printf("  Actual:   %d test cases\n", g_full_2x3x4_count);
    printf("  Status:   %s\n", g_full_2x3x4_count == 24 ? "✓ PASS" : "✗ FAIL");
    printf("  Unique combinations: %zu\n", std::set<std::tuple<int,int,int>>(
        g_full_2x3x4_results.begin(), g_full_2x3x4_results.end()).size());
    printf("\n");

    // Test 4: ALIGNED 2x3x4
    printf("Test 4: ALIGNED mode 2x3x4\n");
    printf("  Expected: 4 test cases (max column size)\n");
    printf("  Actual:   %d test cases\n", g_aligned_2x3x4_count);
    printf("  Status:   %s\n", g_aligned_2x3x4_count == 4 ? "✓ PASS" : "✗ FAIL");
    printf("  Results:\n");
    for (size_t i = 0; i < g_aligned_2x3x4_results.size(); i++) {
        auto [x, y, z] = g_aligned_2x3x4_results[i];
        printf("    Run %zu: (%d, %d, %d)\n", i, x, y, z);
    }
    printf("\n");

    // Test 5: FULL 5x1x3
    printf("Test 5: FULL mode 5x1x3\n");
    printf("  Expected: 15 test cases (5 × 1 × 3)\n");
    printf("  Actual:   %d test cases\n", g_full_5x1x3_count);
    printf("  Status:   %s\n", g_full_5x1x3_count == 15 ? "✓ PASS" : "✗ FAIL");
    printf("  Unique combinations: %zu\n", std::set<std::tuple<int,int,int>>(
        g_full_5x1x3_results.begin(), g_full_5x1x3_results.end()).size());
    printf("\n");

    // Test 6: ALIGNED 5x1x3
    printf("Test 6: ALIGNED mode 5x1x3\n");
    printf("  Expected: 5 test cases (max column size)\n");
    printf("  Actual:   %d test cases\n", g_aligned_5x1x3_count);
    printf("  Status:   %s\n", g_aligned_5x1x3_count == 5 ? "✓ PASS" : "✗ FAIL");
    printf("  Results:\n");
    for (size_t i = 0; i < g_aligned_5x1x3_results.size(); i++) {
        auto [p, q, r] = g_aligned_5x1x3_results[i];
        printf("    Run %zu: (%d, %d, %d)\n", i, p, q, r);
    }
    printf("\n");

    // Overall verification
    bool all_pass =
        g_full_3x2x2_count == 12 &&
        g_aligned_3x2x2_count == 3 &&
        g_full_2x3x4_count == 24 &&
        g_aligned_2x3x4_count == 4 &&
        g_full_5x1x3_count == 15 &&
        g_aligned_5x1x3_count == 5;

    printf("========================================\n");
    printf("Overall Result: %s\n", all_pass ? "✓ ALL TESTS PASSED" : "✗ SOME TESTS FAILED");
    printf("========================================\n\n");

    // Verify FULL mode generates all unique combinations
    bool full_unique_ok =
        std::set<std::tuple<int,int,int>>(g_full_3x2x2_results.begin(), g_full_3x2x2_results.end()).size() == 12 &&
        std::set<std::tuple<int,int,int>>(g_full_2x3x4_results.begin(), g_full_2x3x4_results.end()).size() == 24 &&
        std::set<std::tuple<int,int,int>>(g_full_5x1x3_results.begin(), g_full_5x1x3_results.end()).size() == 15;

    printf("FULL mode uniqueness check: %s\n", full_unique_ok ? "✓ PASS" : "✗ FAIL");
    printf("(All FULL mode combinations should be unique)\n\n");

    return all_pass && full_unique_ok ? result : 1;
}
