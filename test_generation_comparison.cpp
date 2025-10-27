#include "gtest_generator.h"
#include <string>

// Simple test class
class SimpleBox {
private:
    int value_ = 42;
    std::string name_ = "test";

public:
    SimpleBox() = default;
    SimpleBox(int v) : value_(v) {}

    // Grant friend access
    GTESTG_FRIEND_ACCESS_PRIVATE();

    int getValue() const { return value_; }
};

// Test with regular TEST_G to compare
struct RegularGenTest : ::gtest_generator::TestWithGenerator {
    int test_counter = 0;
};

TEST_G(RegularGenTest, RegularFullMode) {
    int x = GENERATOR(1, 2, 3);     // 3 values
    int y = GENERATOR(10, 20);      // 2 values
    std::string z = GENERATOR("A", "B");  // 2 values
        USE_GENERATOR(FULL);
    // Expected: 3 * 2 * 2 = 12 test cases

    printf("Regular TEST_G FULL: x=%d, y=%d, z=%s\n", x, y, z.c_str());

    // Verify we get all combinations
    EXPECT_TRUE(x >= 1 && x <= 3);
    EXPECT_TRUE(y == 10 || y == 20);
    EXPECT_TRUE(z == "A" || z == "B");
}

TEST_G(RegularGenTest, RegularAlignedMode) {
    int a = GENERATOR(1, 2, 3, 4);    // 4 values
    int b = GENERATOR(100, 200);      // 2 values
    std::string c = GENERATOR("X", "Y", "Z");  // 3 values
    USE_GENERATOR(ALIGNED);
    // Expected: max(4, 2, 3) = 4 test cases

    printf("Regular TEST_G ALIGNED: a=%d, b=%d, c=%s\n", a, b, c.c_str());

    EXPECT_TRUE(a >= 1 && a <= 4);
    EXPECT_TRUE(b == 100 || b == 200);
    EXPECT_TRUE(c == "X" || c == "Y" || c == "Z");
}

// Now test with TEST_G_FRIEND
struct FriendGenTest : ::gtest_generator::TestWithGenerator {
    SimpleBox box{100};
};

TEST_G_FRIEND(FriendGenTest, FriendFullMode) {
    int x = GENERATOR(1, 2, 3);     // 3 values
    int y = GENERATOR(10, 20);      // 2 values
    std::string z = GENERATOR("A", "B");  // 2 values
    USE_GENERATOR(FULL);
    // Expected: 3 * 2 * 2 = 12 test cases

    // Access private member
    EXPECT_EQ(box.value_, 100);

    printf("TEST_G_FRIEND FULL: x=%d, y=%d, z=%s, private_value=%d\n",
           x, y, z.c_str(), box.value_);

    // Verify we get all combinations
    EXPECT_TRUE(x >= 1 && x <= 3);
    EXPECT_TRUE(y == 10 || y == 20);
    EXPECT_TRUE(z == "A" || z == "B");
}

TEST_G_FRIEND(FriendGenTest, FriendAlignedMode) {
    int a = GENERATOR(1, 2, 3, 4);    // 4 values
    int b = GENERATOR(100, 200);      // 2 values
    std::string c = GENERATOR("X", "Y", "Z");  // 3 values
    USE_GENERATOR(ALIGNED);
    // Expected: max(4, 2, 3) = 4 test cases

    // Access private member
    EXPECT_EQ(box.value_, 100);

    printf("TEST_G_FRIEND ALIGNED: a=%d, b=%d, c=%s, private_value=%d\n",
           a, b, c.c_str(), box.value_);

    EXPECT_TRUE(a >= 1 && a <= 4);
    EXPECT_TRUE(b == 100 || b == 200);
    EXPECT_TRUE(c == "X" || c == "Y" || c == "Z");
}