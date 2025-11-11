#include "gtest_generator.h"
#include <gtest/gtest.h>

// Simple test class
class TestBox {
private:
    int value_ = 42;
    std::string message_ = "Private Message";

public:
    TestBox() = default;
    TestBox(int v) : value_(v) {}

    // Grant friend access
    GTESTG_FRIEND_ACCESS_PRIVATE();

    int getPublic() const { return value_; }
private:
    int computeDouble() const { return value_ * 2; }
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

// ============================================================================
// TEST_FRIEND tests - Simple tests without fixtures (like regular TEST())
// ============================================================================

TEST_FRIEND(AccessPrivateMembers) {
    TestBox box;
    
    // Test direct access to private members
    EXPECT_EQ(box.value_, 42);
    EXPECT_EQ(box.message_, "Private Message");
    
    // Test access to public members (should still work)
    EXPECT_EQ(box.getPublic(), 42);
}

TEST_FRIEND(ModifyPrivateMembers) {
    TestBox box(99);
    
    // Verify initial value
    EXPECT_EQ(box.value_, 99);
    
    // Modify private members
    box.value_ = 123;
    box.message_ = "Modified Message";
    
    EXPECT_EQ(box.value_, 123);
    EXPECT_EQ(box.message_, "Modified Message");
    EXPECT_EQ(box.getPublic(), 123);
}

TEST_FRIEND(CallPrivateMethods) {
    TestBox box(25);
    
    // Call private method
    int result = box.computeDouble();
    EXPECT_EQ(result, 50);  // 25 * 2
    
    // Modify value and test again
    box.value_ = 100;
    result = box.computeDouble();
    EXPECT_EQ(result, 200);  // 100 * 2
}

TEST_FRIEND(MultipleObjectsTest) {
    TestBox box1(10);
    TestBox box2(20);
    TestBox box3;  // default constructor
    
    // Access private members of multiple objects
    EXPECT_EQ(box1.value_, 10);
    EXPECT_EQ(box2.value_, 20);
    EXPECT_EQ(box3.value_, 42);  // default value
    
    // Modify all objects
    box1.value_ = 100;
    box2.value_ = 200;
    box3.value_ = 300;
    
    EXPECT_EQ(box1.value_, 100);
    EXPECT_EQ(box2.value_, 200);
    EXPECT_EQ(box3.value_, 300);
}

// Regular TEST to verify non-friend tests cannot access private members
TEST(RegularTest, NoPrivateAccessAllowed) {
    TestBox box;
    
    // Can only access public interface
    EXPECT_EQ(box.getPublic(), 42);
    
    // The following would cause compilation errors if uncommented:
    // EXPECT_EQ(box.value_, 42);     // Error: private member
    // EXPECT_EQ(box.message_, "");   // Error: private member
    // box.computeDouble();           // Error: private method
}

// Main function for standalone execution
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    
    std::cout << "\n========================================\n";
    std::cout << "Testing TEST_FRIEND and TEST_G_FRIEND\n";
    std::cout << "Private Member Access Feature\n";
    std::cout << "========================================\n\n";
    
    return RUN_ALL_TESTS();
}