// test_friend_comparison.cpp
// Comparison test showing the differences between TEST, TEST_F, TEST_FRIEND, TEST_F_FRIEND

#include "gtest_generator.h"
#include <gtest/gtest.h>

// Test class for friend access
class TestTarget {
private:
    int private_value_ = 42;
    std::string private_message_ = "Secret";

public:
    TestTarget() = default;
    TestTarget(int val) : private_value_(val) {}
    
    // Grant friend access
    GTESTG_FRIEND_ACCESS_PRIVATE();
    
    int getPublicValue() const { return private_value_; }
};

// Test fixture for TEST_F and TEST_F_FRIEND
struct TestTargetFixture : ::testing::Test {
    TestTarget obj;
    TestTarget obj_custom{99};
};

// ============================================================================
// Standard Google Test macros - NO private access
// ============================================================================

TEST(RegularTest, CanOnlyAccessPublic) {
    TestTarget obj;
    EXPECT_EQ(obj.getPublicValue(), 42);
    
    // Cannot access private members:
    // obj.private_value_;     // Compile error
    // obj.private_message_;   // Compile error
}

TEST_F(TestTargetFixture, CanOnlyAccessPublicWithFixture) {
    EXPECT_EQ(obj.getPublicValue(), 42);
    EXPECT_EQ(obj_custom.getPublicValue(), 99);
    
    // Cannot access private members:
    // obj.private_value_;     // Compile error
    // obj.private_message_;   // Compile error
}

// ============================================================================
// Friend-enabled macros - CAN access private members
// ============================================================================

TEST_FRIEND(CanAccessPrivateMembers) {
    TestTarget obj;
    TestTarget obj_custom(123);
    
    // Can access both public and private members
    EXPECT_EQ(obj.getPublicValue(), 42);
    EXPECT_EQ(obj.private_value_, 42);
    EXPECT_EQ(obj.private_message_, "Secret");
    
    EXPECT_EQ(obj_custom.getPublicValue(), 123);
    EXPECT_EQ(obj_custom.private_value_, 123);
    
    // Can modify private members
    obj.private_value_ = 999;
    obj.private_message_ = "Modified";
    
    EXPECT_EQ(obj.private_value_, 999);
    EXPECT_EQ(obj.private_message_, "Modified");
    EXPECT_EQ(obj.getPublicValue(), 999);
}

TEST_F_FRIEND(TestTargetFixture, CanAccessPrivateMembersWithFixture) {
    // Can access both public and private members of fixture objects
    EXPECT_EQ(obj.getPublicValue(), 42);
    EXPECT_EQ(obj.private_value_, 42);
    EXPECT_EQ(obj.private_message_, "Secret");
    
    EXPECT_EQ(obj_custom.getPublicValue(), 99);
    EXPECT_EQ(obj_custom.private_value_, 99);
    
    // Can modify private members of fixture objects
    obj.private_value_ = 777;
    obj_custom.private_message_ = "Changed";
    
    EXPECT_EQ(obj.private_value_, 777);
    EXPECT_EQ(obj_custom.private_message_, "Changed");
}

// Main function
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    
    std::cout << "\n========================================\n";
    std::cout << "Comparison: TEST vs TEST_FRIEND\n";
    std::cout << "           TEST_F vs TEST_F_FRIEND\n";
    std::cout << "========================================\n\n";
    
    return RUN_ALL_TESTS();
}