#include "gtest_generator.h"
#include <vector>

// Test fixture
class AlignedDebugTest : public gtest_generator::TestWithGenerator {
};

// Test to understand actual ALIGNED behavior
TEST_G(AlignedDebugTest, TwoGenerators_SameSize) {
    auto a = GENERATOR(1, 2, 3);
    auto b = GENERATOR(10, 20, 30);
    USE_GENERATOR(ALIGNED);

    printf("Param=%d: a=%d, b=%d\n", GetParam(), a, b);

    // Don't assert anything, just observe
}

TEST_G(AlignedDebugTest, TwoGenerators_DifferentSize) {
    auto x = GENERATOR(1, 2);
    auto y = GENERATOR(10, 20, 30, 40);
    USE_GENERATOR(ALIGNED);

    printf("Param=%d: x=%d, y=%d\n", GetParam(), x, y);
}

TEST_G(AlignedDebugTest, ThreeGenerators_SameSize) {
    auto a = GENERATOR(1, 2);
    auto b = GENERATOR(10, 20);
    auto c = GENERATOR(100, 200);
    USE_GENERATOR(ALIGNED);

    printf("Param=%d: a=%d, b=%d, c=%d\n", GetParam(), a, b, c);
}
