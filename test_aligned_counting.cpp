#include "gtest_generator.h"

// This test demonstrates that during the ALIGNED mode counting phase,
// each GENERATOR returns its second value (if available)

class AlignedCountingTest : public ::gtest_generator::TestWithGenerator {
};

TEST_G(AlignedCountingTest, SecondValueDuringCounting) {
    int a = GENERATOR(100, 200, 300);  // Second value: 200
    int b = GENERATOR(10, 20);          // Second value: 20
    int c = GENERATOR(1);               // Only one value: 1
    USE_GENERATOR(ALIGNED);

    // At runtime, values follow ALIGNED mode pattern:
    // Run 0: (100, 10, 1)
    // Run 1: (200, 20, 1)
    // Run 2: (300, 10, 1)

    printf("Run %d: a=%d, b=%d, c=%d\n", GetParam(), a, b, c);

    // Verify ALIGNED mode behavior
    int expected_a[] = {100, 200, 300};
    int expected_b[] = {10, 20, 10};
    int expected_c[] = {1, 1, 1};

    EXPECT_EQ(a, expected_a[GetParam()]);
    EXPECT_EQ(b, expected_b[GetParam()]);
    EXPECT_EQ(c, expected_c[GetParam()]);
}
