// test_friend_access.cpp
// Test file to verify the new TEST_FRIEND and TEST_G_FRIEND macros work correctly
// with private member access

#include "gtest_generator.h"
#include <gtest/gtest.h>

// ============================================================================
// Sample classes with private members for testing
// ============================================================================

// Simple class with private members
class SecretKeeper {
private:
    int secret_value_ = 42;
    std::string secret_message_ = "Top Secret";
    static int static_secret_;

    int computeSecret(int x) const { return secret_value_ * x; }

protected:
    double protected_value_ = 3.14;

public:
    SecretKeeper() = default;
    SecretKeeper(int val) : secret_value_(val) {}

    // Grant friend access - enables both TEST_FRIEND and GTESTG_PRIVATE_MEMBER approaches
    GTESTG_FRIEND_ACCESS_PRIVATE();

    // Public interface
    int getPublicValue() const { return 100; }
};

// Initialize static member
int SecretKeeper::static_secret_ = 999;

// Another class to test with complex private state
class ComplexPrivate {
private:
    std::vector<int> private_data_ = {1, 2, 3, 4, 5};
    std::map<std::string, double> private_map_ = {{"alpha", 1.1}, {"beta", 2.2}};

    bool checkData() const { return !private_data_.empty(); }

public:
    ComplexPrivate() = default;

    // Grant friend access - enables both TEST_FRIEND and GTESTG_PRIVATE_MEMBER approaches
    GTESTG_FRIEND_ACCESS_PRIVATE();

    size_t getSize() const { return private_data_.size(); }
};

// ============================================================================
// TEST_FRIEND tests - Standard fixture with private access
// ============================================================================

// Test fixture
struct SecretKeeperTest : ::testing::Test {
    SecretKeeper keeper;
    SecretKeeper keeper_custom{77};
};

// Test accessing private members using TEST_FRIEND
TEST_FRIEND(SecretKeeperTest, AccessPrivateMembers) {
    // Access private member variables
    EXPECT_EQ(keeper.secret_value_, 42);
    EXPECT_EQ(keeper.secret_message_, "Top Secret");

    // Access protected members too
    EXPECT_DOUBLE_EQ(keeper.protected_value_, 3.14);

    // Access public members (still works)
    EXPECT_EQ(keeper.getPublicValue(), 100);

    // Test custom initialized object
    EXPECT_EQ(keeper_custom.secret_value_, 77);
}

TEST_FRIEND(SecretKeeperTest, ModifyPrivateMembers) {
    // We can also modify private members
    keeper.secret_value_ = 123;
    EXPECT_EQ(keeper.secret_value_, 123);

    keeper.secret_message_ = "Modified Secret";
    EXPECT_EQ(keeper.secret_message_, "Modified Secret");

    keeper.protected_value_ = 2.71;
    EXPECT_DOUBLE_EQ(keeper.protected_value_, 2.71);
}

TEST_FRIEND(SecretKeeperTest, CallPrivateMethods) {
    // Access and call private methods
    int result = keeper.computeSecret(10);
    EXPECT_EQ(result, 420);  // 42 * 10

    keeper.secret_value_ = 5;
    result = keeper.computeSecret(7);
    EXPECT_EQ(result, 35);  // 5 * 7
}

TEST_FRIEND(SecretKeeperTest, AccessStaticPrivateMembers) {
    // Access static private members
    EXPECT_EQ(SecretKeeper::static_secret_, 999);

    // Modify static private member
    SecretKeeper::static_secret_ = 1234;
    EXPECT_EQ(SecretKeeper::static_secret_, 1234);

    // Reset for other tests
    SecretKeeper::static_secret_ = 999;
}

// Test fixture for ComplexPrivate
struct ComplexPrivateTest : ::testing::Test {
    ComplexPrivate complex;
};

TEST_FRIEND(ComplexPrivateTest, AccessComplexPrivateData) {
    // Access private vector
    ASSERT_EQ(complex.private_data_.size(), 5);
    EXPECT_EQ(complex.private_data_[0], 1);
    EXPECT_EQ(complex.private_data_[4], 5);

    // Modify private vector
    complex.private_data_.push_back(6);
    EXPECT_EQ(complex.private_data_.size(), 6);
    EXPECT_EQ(complex.private_data_[5], 6);

    // Access private map
    EXPECT_EQ(complex.private_map_.size(), 2);
    EXPECT_DOUBLE_EQ(complex.private_map_["alpha"], 1.1);
    EXPECT_DOUBLE_EQ(complex.private_map_["beta"], 2.2);

    // Add to private map
    complex.private_map_["gamma"] = 3.3;
    EXPECT_EQ(complex.private_map_.size(), 3);

    // Call private method
    EXPECT_TRUE(complex.checkData());
}

// ============================================================================
// TEST_G_FRIEND tests - Generator tests with private access
// ============================================================================

// Generator test fixture
struct SecretGeneratorTest : ::gtest_generator::TestWithGenerator {
    SecretKeeper keeper;

    SecretGeneratorTest() : keeper(10) {}  // Initialize with custom value
};

TEST_G_FRIEND(SecretGeneratorTest, GeneratorWithPrivateAccess) {
    USE_GENERATOR();

    // Generate test values
    auto multiplier = GENERATOR(1, 2, 3, 4, 5);

    // Access private member in generator test
    int expected = keeper.secret_value_ * multiplier;  // 10 * multiplier
    int actual = keeper.computeSecret(multiplier);

    EXPECT_EQ(actual, expected);
    EXPECT_EQ(keeper.secret_value_, 10);

    // Verify we can still access other private members
    EXPECT_EQ(keeper.secret_message_, "Top Secret");
    EXPECT_DOUBLE_EQ(keeper.protected_value_, 3.14);
}

TEST_G_FRIEND(SecretGeneratorTest, MultipleGeneratorsWithPrivate) {
    USE_GENERATOR();

    auto base = GENERATOR(10, 20, 30);
    auto offset = GENERATOR(1, 2);

    // Modify private value based on generator values
    keeper.secret_value_ = base + offset;

    // Verify the modification worked
    EXPECT_GE(keeper.secret_value_, 11);  // min: 10+1
    EXPECT_LE(keeper.secret_value_, 32);  // max: 30+2

    // Use private method with the modified value
    int result = keeper.computeSecret(2);
    EXPECT_EQ(result, keeper.secret_value_ * 2);
}

// Test with ALIGNED mode
struct AlignedPrivateTest : ::gtest_generator::TestWithGenerator {
    ComplexPrivate complex;
};

// Suppress the uninstantiated test warning - this test uses generator counting
GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(AlignedPrivateTest__AlignedModeWithPrivate);

// Using TEST_G_FRIEND to get private access via GTESTG_FRIEND_ACCESS_PRIVATE()
TEST_G_FRIEND(AlignedPrivateTest, AlignedModeWithPrivate) {
    USE_GENERATOR(ALIGNED);

    auto index = GENERATOR(0, 1, 2, 3, 4);
    auto value = GENERATOR(10, 20, 30);

    // Access private vector with generated index
    if (index < complex.private_data_.size()) {
        int original = complex.private_data_[index];
        complex.private_data_[index] = value;

        EXPECT_EQ(complex.private_data_[index], value);

        // Restore original
        complex.private_data_[index] = original;
    }

    // Verify private method still works
    EXPECT_TRUE(complex.checkData());
}

// ============================================================================
// Regular tests to verify non-FRIEND tests can't access private members
// ============================================================================

// This test should NOT be able to access private members
TEST(RegularTest, PublicAccessOnly) {
    SecretKeeper keeper;

    // Can only access public members
    EXPECT_EQ(keeper.getPublicValue(), 100);

    // The following would cause compilation errors if uncommented:
    // keeper.secret_value_;  // Error: 'secret_value_' is private
    // keeper.secret_message_;  // Error: 'secret_message_' is private
    // keeper.protected_value_;  // Error: 'protected_value_' is protected
    // keeper.computeSecret(5);  // Error: 'computeSecret' is private
}

// Regular TEST_G should also not have private access
struct RegularGeneratorTest : ::gtest_generator::TestWithGenerator {
    SecretKeeper keeper;
};

TEST_G(RegularGeneratorTest, NoPrivateAccess) {
    USE_GENERATOR();

    auto val = GENERATOR(1, 2, 3);

    // Can only use public interface
    EXPECT_EQ(keeper.getPublicValue(), 100);

    // The following would cause compilation errors if uncommented:
    // keeper.secret_value_;  // Error: 'secret_value_' is private
}

// ============================================================================
// Edge cases and additional tests
// ============================================================================

// Test with inheritance
class DerivedSecret : public SecretKeeper {
public:
    DerivedSecret() : SecretKeeper(55) {}

    // Grant friend access - enables both TEST_FRIEND and GTESTG_PRIVATE_MEMBER approaches
    GTESTG_FRIEND_ACCESS_PRIVATE();

private:
    int derived_secret_ = 88;
};

struct DerivedTest : ::testing::Test {
    DerivedSecret derived;
};

TEST_FRIEND(DerivedTest, AccessDerivedAndBasePrivates) {
    // Access base class privates
    EXPECT_EQ(derived.secret_value_, 55);
    EXPECT_EQ(derived.secret_message_, "Top Secret");

    // Access derived class privates
    EXPECT_EQ(derived.derived_secret_, 88);

    // Modify both
    derived.secret_value_ = 100;
    derived.derived_secret_ = 200;

    EXPECT_EQ(derived.secret_value_, 100);
    EXPECT_EQ(derived.derived_secret_, 200);
}

// Test multiple objects in same test
TEST_FRIEND(SecretKeeperTest, MultipleObjects) {
    SecretKeeper k1(10);
    SecretKeeper k2(20);
    SecretKeeper k3(30);

    EXPECT_EQ(k1.secret_value_, 10);
    EXPECT_EQ(k2.secret_value_, 20);
    EXPECT_EQ(k3.secret_value_, 30);

    // Modify all
    k1.secret_value_ = 100;
    k2.secret_value_ = 200;
    k3.secret_value_ = 300;

    EXPECT_EQ(k1.secret_value_, 100);
    EXPECT_EQ(k2.secret_value_, 200);
    EXPECT_EQ(k3.secret_value_, 300);
}

// Main function
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);

    std::cout << "\n========================================\n";
    std::cout << "Testing TEST_FRIEND and TEST_G_FRIEND\n";
    std::cout << "Private Member Access Feature\n";
    std::cout << "========================================\n\n";

    return RUN_ALL_TESTS();
}