#include "gtest_generator.h"

// Simple test class
class TestBox {
private:
    int value_ = 42;

public:
    TestBox() = default;
    TestBox(int v) : value_(v) {}

    // Grant friend access
    GTESTG_FRIEND_ACCESS_PRIVATE();
};

// Test with TEST_G_FRIEND - simple case
struct SimpleGenTest : ::gtest_generator::TestWithGenerator {
    TestBox box{100};
};

TEST_G_FRIEND(SimpleGenTest, SimpleMultiGen) {
    int x = GENERATOR(1, 2);     // 2 values
    int y = GENERATOR(10, 20, 30);  // 3 values
    USE_GENERATOR();  // No mode specified - default is FULL
    // Expected: 2 * 3 = 6 test cases

    // Access private member
    EXPECT_EQ(box.value_, 100);

    printf("TEST_G_FRIEND Simple: x=%d, y=%d, private_value=%d\n",
           x, y, box.value_);
}