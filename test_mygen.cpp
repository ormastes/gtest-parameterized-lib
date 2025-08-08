#include "gtest_generator.h"

// Fixture class using TestWithParam<int>
class MyTest : public ::testing::TestWithParam<int> {
};

// Use TEST_P as usual
TEST_P(MyTest, SimpleCase) {
    int a = GENERATOR(1, 2);
    int b = GENERATOR(10, 20);
    USE_GENERATOR();

    EXPECT_LT(a, b);
    printf("Test case: a=%d, b=%d\n", a, b);
}

// Instantiate the combinations
ENABLE_GENERATOR(MyTest)