#include "gtest_generator.h"
#include <string>
#include <vector>

// Test class with private members for testing friend access
class PrivateBox {
private:
    int value_ = 100;
    std::string name_ = "private";
    std::vector<int> data_ = {1, 2, 3};

    int computeSum() const {
        int sum = value_;
        for (int d : data_) sum += d;
        return sum;
    }

public:
    PrivateBox() = default;
    PrivateBox(int v, const std::string& n) : value_(v), name_(n) {}

    // Grant friend access for testing
    GTESTG_FRIEND_ACCESS_PRIVATE();

    int getPublicValue() const { return value_ / 2; }
};

// Test fixture for FULL mode (Cartesian product)
struct PrivateBoxFullTest : ::gtest_generator::TestWithGenerator {
    PrivateBox box{500, "full_test"};
};

// TEST_G_FRIEND with FULL mode - generates all combinations
TEST_G_FRIEND(PrivateBoxFullTest, FullModeGeneration) {
    int x = GENERATOR(1, 2, 3);     // 3 values
    int y = GENERATOR(10, 20);      // 2 values
    std::string mode = GENERATOR("A", "B");  // 2 values
    USE_GENERATOR(FULL);  // Explicit FULL mode (default)
    // Total: 3 * 2 * 2 = 12 test cases

    // Verify friend access works
    EXPECT_EQ(box.value_, 500);
    EXPECT_EQ(box.name_, "full_test");
    EXPECT_EQ(box.data_.size(), 3);

    // Use the generated values
    int result = x * y;

    // Call private method
    int sum = box.computeSum();
    EXPECT_EQ(sum, 506);  // 500 + 1 + 2 + 3

    printf("FULL mode: x=%d, y=%d, mode=%s, result=%d, private_value=%d\n",
           x, y, mode.c_str(), result, box.value_);
}

// Test fixture for ALIGNED mode
struct PrivateBoxAlignedTest : ::gtest_generator::TestWithGenerator {
    PrivateBox box{700, "aligned_test"};
};

// TEST_G_FRIEND with ALIGNED mode - generates aligned combinations
TEST_G_FRIEND(PrivateBoxAlignedTest, AlignedModeGeneration) {
    int a = GENERATOR(1, 2, 3, 4);    // 4 values
    int b = GENERATOR(100, 200);      // 2 values
    std::string c = GENERATOR("X", "Y", "Z");  // 3 values
    USE_GENERATOR(ALIGNED);  // Explicit ALIGNED mode
    // Total: max(4, 2, 3) = 4 test cases

    // Verify friend access works
    EXPECT_EQ(box.value_, 700);
    EXPECT_EQ(box.name_, "aligned_test");
    EXPECT_EQ(box.data_.size(), 3);

    // Use the generated values
    int product = a * b;

    // Modify private member
    box.value_ = 800 + a;

    // Call private method
    int sum = box.computeSum();
    EXPECT_GT(sum, 800);  // Should be > 800 since we added 'a'

    printf("ALIGNED mode: a=%d, b=%d, c=%s, product=%d, modified_value=%d\n",
           a, b, c.c_str(), product, box.value_);
}

// Mixed test - multiple generators with different sizes
struct PrivateBoxMixedTest : ::gtest_generator::TestWithGenerator {
    PrivateBox box{999, "mixed_test"};
};

// Another FULL mode test with different generator sizes
TEST_G_FRIEND(PrivateBoxMixedTest, MixedGeneration) {
    int val1 = GENERATOR(5);           // 1 value
    int val2 = GENERATOR(10, 20, 30);  // 3 values
    USE_GENERATOR();  // Default is FULL mode
    // Total: 1 * 3 = 3 test cases

    // Access private members
    EXPECT_EQ(box.value_, 999);
    EXPECT_EQ(box.name_, "mixed_test");

    // Modify private data
    box.data_.push_back(val2);
    EXPECT_EQ(box.data_.size(), 4);

    printf("Mixed FULL: val1=%d, val2=%d, data_size=%zu\n",
           val1, val2, box.data_.size());
}

// Test to verify the count difference between ALIGNED and FULL modes
struct CountVerificationTest : ::gtest_generator::TestWithGenerator {
    PrivateBox box{1234, "count_test"};
    int test_count = 0;
};

// This should generate 2*3*4 = 24 tests in FULL mode
TEST_G_FRIEND(CountVerificationTest, FullModeCount) {
    int x = GENERATOR(1, 2);        // 2 values
    int y = GENERATOR(10, 20, 30);  // 3 values
    int z = GENERATOR(100, 200, 300, 400);  // 4 values
    USE_GENERATOR(FULL);
    // Expected: 2 * 3 * 4 = 24 test cases

    // Access private members to verify friend access
    EXPECT_EQ(box.name_, "count_test");
    box.value_ = x + y + z;

    printf("FULL count test: x=%d, y=%d, z=%d, sum=%d\n", x, y, z, box.value_);
}

// This should generate max(2,3,4) = 4 tests in ALIGNED mode
TEST_G_FRIEND(CountVerificationTest, AlignedModeCount) {
    int x = GENERATOR(1, 2);        // 2 values
    int y = GENERATOR(10, 20, 30);  // 3 values
    int z = GENERATOR(100, 200, 300, 400);  // 4 values
    USE_GENERATOR(ALIGNED);
    // Expected: max(2, 3, 4) = 4 test cases

    // Access private members to verify friend access
    EXPECT_EQ(box.name_, "count_test");
    box.value_ = x + y + z;

    printf("ALIGNED count test: x=%d, y=%d, z=%d, sum=%d\n", x, y, z, box.value_);
}