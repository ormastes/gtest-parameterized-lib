#include "gtest_generator.h"
#include <string>

// Additional test fixture
class AdditionalTest : public ::gtest_generator::TestWithGenerator {
};

TEST_G(AdditionalTest, MathOperations) {
    int x = GENERATOR(5, 10, 15);
    int y = GENERATOR(2, 3);
    USE_GENERATOR();

    int sum = x + y;
    int product = x * y;

    EXPECT_GT(sum, 0);
    EXPECT_GT(product, 0);
    EXPECT_GT(product, sum);

    printf("Math test: %d + %d = %d, %d * %d = %d\n", x, y, sum, x, y, product);
}

TEST_G(AdditionalTest, StringLength) {
    std::string str = GENERATOR("hello", "world", "test");
    int multiplier = GENERATOR(1, 2);
    USE_GENERATOR();

    std::string result = "";
    for (int i = 0; i < multiplier; ++i) {
        result += str;
    }

    EXPECT_EQ(result.length(), str.length() * multiplier);
    printf("String length test: '%s' * %d = '%s' (length: %zu)\n",
           str.c_str(), multiplier, result.c_str(), result.length());
}

TEST_G(AdditionalTest, BooleanLogic) {
    bool a = GENERATOR(true, false);
    bool b = GENERATOR(true, false);
    USE_GENERATOR();

    bool and_result = a && b;
    bool or_result = a || b;

    EXPECT_EQ(and_result, a && b);
    EXPECT_EQ(or_result, a || b);

    printf("Boolean test: %d AND %d = %d, %d OR %d = %d\n",
           a, b, and_result, a, b, or_result);
}
