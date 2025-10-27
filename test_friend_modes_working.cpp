#include "gtest_generator.h"

// Test class with private members
class ModeTestBox {
private:
    int value_ = 999;

public:
    ModeTestBox() = default;

    // Grant friend access
    GTESTG_FRIEND_ACCESS_PRIVATE();
};

// Test FULL mode - should generate all combinations
struct FullModeTest : ::gtest_generator::TestWithGenerator {
    ModeTestBox box;
};

TEST_G_FRIEND(FullModeTest, TestFullGeneration) {
    int a = GENERATOR(1, 2);        // 2 values
    int b = GENERATOR(10, 20, 30);  // 3 values
    USE_GENERATOR(FULL);            // Explicit FULL mode
    // Expected: 2 * 3 = 6 test cases

    EXPECT_EQ(box.value_, 999);
    printf("FULL mode: a=%d, b=%d, private=%d\n", a, b, box.value_);
}

// Test ALIGNED mode - should generate max number
struct AlignedModeTest : ::gtest_generator::TestWithGenerator {
    ModeTestBox box;
};

TEST_G_FRIEND(AlignedModeTest, TestAlignedGeneration) {
    int x = GENERATOR(1, 2, 3, 4);  // 4 values
    int y = GENERATOR(100, 200);    // 2 values
    int z = GENERATOR(5, 6, 7);     // 3 values
    USE_GENERATOR(ALIGNED);         // Explicit ALIGNED mode
    // Expected: max(4, 2, 3) = 4 test cases

    EXPECT_EQ(box.value_, 999);
    printf("ALIGNED mode: x=%d, y=%d, z=%d, private=%d\n", x, y, z, box.value_);
}