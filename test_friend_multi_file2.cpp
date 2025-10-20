#include "gtest_generator.h"
#include <string>

// ============================================================================
// Second file - defines different target classes and tests
// ============================================================================

class BoxB {
private:
    double data_ = 3.14;

public:
    BoxB() = default;
    BoxB(double d) : data_(d) {}

    GTESTG_FRIEND_ACCESS_PRIVATE();
};

// Declare function-based accessors
GTESTG_PRIVATE_DECLARE_MEMBER(BoxB, data_);

// Test fixture
struct BoxBTest : ::testing::Test {
    BoxB box{2.5};
};

// TEST_FRIEND in file 2
TEST_FRIEND(BoxBTest, AccessFromFile2) {
    double& data = GTESTG_PRIVATE_MEMBER(BoxB, data_, &box);
    EXPECT_DOUBLE_EQ(data, 2.5);
    printf("File2 TEST_FRIEND: data=%.2f\n", data);
}

// TEST_G_FRIEND in file 2
struct BoxBGenTest : ::gtest_generator::TestWithGenerator {
    BoxB box{9.99};
};

TEST_G_FRIEND(BoxBGenTest, GeneratorFromFile2) {
    int multiplier = GENERATOR(10, 20, 30);
    USE_GENERATOR();

    double& data = GTESTG_PRIVATE_MEMBER(BoxB, data_, &box);
    EXPECT_DOUBLE_EQ(data, 9.99);

    printf("File2 TEST_G_FRIEND: multiplier=%d, data=%.2f\n", multiplier, data);
}

// ============================================================================
// Use the SharedBox from file1
// ============================================================================
class SharedBox {
private:
    std::string name_;
    int count_;

public:
    SharedBox() : name_("default"), count_(0) {}
    SharedBox(const std::string& n, int c) : name_(n), count_(c) {}

    GTESTG_FRIEND_ACCESS_PRIVATE();
};

// Declare accessors for SharedBox (same as file1)
GTESTG_PRIVATE_DECLARE_MEMBER(SharedBox, name_);
GTESTG_PRIVATE_DECLARE_MEMBER(SharedBox, count_);

struct SharedBoxTest2 : ::gtest_generator::TestWithGenerator {
    SharedBox box{"file2", 20};
};

TEST_G_FRIEND(SharedBoxTest2, FromFile2) {
    int val = GENERATOR(5, 10);
    USE_GENERATOR();

    std::string& name = GTESTG_PRIVATE_MEMBER(SharedBox, name_, &box);
    int& count = GTESTG_PRIVATE_MEMBER(SharedBox, count_, &box);

    EXPECT_EQ(name, "file2");
    EXPECT_EQ(count, 20);

    printf("File2 SharedBox: val=%d, name=%s, count=%d\n", val, name.c_str(), count);
}
