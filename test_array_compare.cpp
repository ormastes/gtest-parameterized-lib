#include "gtest_generator.h"
#include <cmath>

// Test fixture for array comparisons
class ArrayCompareTest : public ::gtest_generator::TestWithGenerator {
};

// Test EXPECT_ARRAY_EQ with integer arrays
TEST_G(ArrayCompareTest, IntArrayEqual) {
    int size = GENERATOR(3, 5, 7);
    USE_GENERATOR();

    std::vector<int> expected(size);
    std::vector<int> actual(size);

    for (int i = 0; i < size; ++i) {
        expected[i] = i * 10;
        actual[i] = i * 10;
    }

    EXPECT_ARRAY_EQ(expected.data(), actual.data(), size);
    printf("Test passed for array size=%d\n", size);
}

// Test EXPECT_ARRAY_EQ with failing case - verify it correctly detects mismatch
TEST_G(ArrayCompareTest, IntArrayNotEqual) {
    USE_GENERATOR();

    int expected[] = {1, 2, 3, 4, 5};
    int actual[] = {1, 2, 99, 4, 5};

    // Verify that EXPECT_ARRAY_EQ correctly detects the mismatch at index 2
    try {
        EXPECT_ARRAY_EQ(expected, actual, 5);
    } catch (...) {
        printf("Correctly detected arrays not near\n");
        return;
    }
    EXPECT_TRUE(false, "Arrays should not be near at index 2");
}

// Test ASSERT_ARRAY_EQ (fatal assertion)
TEST_G(ArrayCompareTest, IntArrayAssert) {
    USE_GENERATOR();

    int expected[] = {10, 20, 30};
    int actual[] = {10, 20, 30};

    ASSERT_ARRAY_EQ(expected, actual, 3);
    printf("ASSERT_ARRAY_EQ passed\n");
}

// Test EXPECT_ARRAY_NEAR with floating-point arrays
TEST_G(ArrayCompareTest, FloatArrayNear) {
    int size = GENERATOR(3, 5);
    double tolerance = GENERATOR(0.01, 0.1);
    USE_GENERATOR();

    std::vector<double> expected(size);
    std::vector<double> actual(size);

    for (int i = 0; i < size; ++i) {
        expected[i] = i * 1.5;
        actual[i] = i * 1.5 + 0.001; // Slightly different
    }

    EXPECT_ARRAY_NEAR(expected.data(), actual.data(), size, tolerance);
    printf("Test passed for size=%d, tolerance=%f\n", size, tolerance);
}

// Test EXPECT_ARRAY_NEAR with failing case
TEST_G(ArrayCompareTest, FloatArrayNotNear) {
    USE_GENERATOR();

    double expected[] = {1.0, 2.0, 3.0};
    double actual[] = {1.0, 2.0, 3.5};

    // This will fail at index 2 (difference 0.5 > tolerance 0.1)
    try {
        EXPECT_ARRAY_NEAR(expected, actual, 3, 0.1);
    } catch (...) {
        printf("Correctly detected arrays not near\n");
        return;
    }
    EXPECT_TRUE(false, "Arrays should not be near at index 2");
}

// Test ASSERT_ARRAY_NEAR (fatal assertion)
TEST_G(ArrayCompareTest, FloatArrayAssertNear) {
    USE_GENERATOR();

    double expected[] = {1.5, 2.5, 3.5};
    double actual[] = {1.501, 2.499, 3.502};

    ASSERT_ARRAY_NEAR(expected, actual, 3, 0.01);
    printf("ASSERT_ARRAY_NEAR passed\n");
}

// Test EXPECT_ARRAY_DOUBLE_EQ
TEST_G(ArrayCompareTest, DoubleArrayEqual) {
    int size = GENERATOR(4, 6);
    USE_GENERATOR();

    std::vector<double> expected(size);
    std::vector<double> actual(size);

    for (int i = 0; i < size; ++i) {
        expected[i] = i * 2.5;
        actual[i] = i * 2.5;
    }

    EXPECT_ARRAY_DOUBLE_EQ(expected.data(), actual.data(), size);
    printf("EXPECT_ARRAY_DOUBLE_EQ passed for size=%d\n", size);
}

// Test EXPECT_ARRAY_FLOAT_EQ
TEST_G(ArrayCompareTest, FloatArrayEqual) {
    int size = GENERATOR(3, 5);
    USE_GENERATOR();

    std::vector<float> expected(size);
    std::vector<float> actual(size);

    for (int i = 0; i < size; ++i) {
        expected[i] = static_cast<float>(i * 1.25f);
        actual[i] = static_cast<float>(i * 1.25f);
    }

    EXPECT_ARRAY_FLOAT_EQ(expected.data(), actual.data(), size);
    printf("EXPECT_ARRAY_FLOAT_EQ passed for size=%d\n", size);
}

// Test with string arrays
TEST_G(ArrayCompareTest, StringArrayEqual) {
    int size = GENERATOR(2, 4);
    USE_GENERATOR();

    std::vector<std::string> expected(size);
    std::vector<std::string> actual(size);

    for (int i = 0; i < size; ++i) {
        expected[i] = "str" + std::to_string(i);
        actual[i] = "str" + std::to_string(i);
    }

    EXPECT_ARRAY_EQ(expected.data(), actual.data(), size);
    printf("String array comparison passed for size=%d\n", size);
}

// Test with char arrays
TEST_G(ArrayCompareTest, CharArrayEqual) {
    USE_GENERATOR();

    char expected[] = {'a', 'b', 'c', 'd'};
    char actual[] = {'a', 'b', 'c', 'd'};

    EXPECT_ARRAY_EQ(expected, actual, 4);
    printf("Char array comparison passed\n");
}

// Test edge case: empty arrays
TEST_G(ArrayCompareTest, EmptyArray) {
    USE_GENERATOR();

    int expected[] = {};
    int actual[] = {};

    EXPECT_ARRAY_EQ(expected, actual, 0);
    printf("Empty array comparison passed\n");
}

// Test edge case: single element
TEST_G(ArrayCompareTest, SingleElement) {
    int value = GENERATOR(42, 100, 999);
    USE_GENERATOR();

    int expected[] = {value};
    int actual[] = {value};

    EXPECT_ARRAY_EQ(expected, actual, 1);
    printf("Single element comparison passed for value=%d\n", value);
}
