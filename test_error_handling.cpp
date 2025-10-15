#include "gtest_generator.h"

// Test fixture for error handling
class ErrorHandlingTest : public gtest_generator::TestWithGenerator {
};

// Test 1: Single value generator
TEST_G(ErrorHandlingTest, SingleValueGenerator) {
    auto val = GENERATOR(42);
    USE_GENERATOR();

    EXPECT_EQ(val, 42);
    printf("Single value test: val=%d\n", val);
}

// Test 2: Generator before USE_GENERATOR (correct usage)
TEST_G(ErrorHandlingTest, GeneratorBeforeUseGenerator) {
    auto a = GENERATOR(1, 2);
    auto b = GENERATOR(10, 20);
    USE_GENERATOR();

    EXPECT_TRUE(a >= 1 && a <= 2);
    EXPECT_TRUE(b >= 10 && b <= 20);
    printf("Correct usage test: a=%d, b=%d\n", a, b);
}

// Test 3: Empty initializer list edge case
TEST_G(ErrorHandlingTest, MinimalGenerator) {
    auto x = GENERATOR(0);
    USE_GENERATOR();

    EXPECT_EQ(x, 0);
}

// Test 4: ALIGNED mode with single generator
TEST_G(ErrorHandlingTest, AlignedSingleGenerator) {
    auto val = GENERATOR(100, 200, 300);
    USE_GENERATOR(ALIGNED);

    int param = GetParam();
    if (param == 0) EXPECT_EQ(val, 100);
    else if (param == 1) EXPECT_EQ(val, 200);
    else if (param == 2) EXPECT_EQ(val, 300);

    printf("ALIGNED single generator: param=%d, val=%d\n", param, val);
}

// Test 5: FULL mode explicit
TEST_G(ErrorHandlingTest, FullModeExplicit) {
    auto x = GENERATOR(1, 2);
    auto y = GENERATOR(10, 20);
    USE_GENERATOR(FULL);

    EXPECT_GT(x, 0);
    EXPECT_GT(y, 0);
    printf("FULL mode: x=%d, y=%d\n", x, y);
}
