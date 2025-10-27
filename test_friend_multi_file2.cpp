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

// Test fixture
struct BoxBTest : ::testing::Test {
    BoxB box{2.5};
};

// TEST_F_FRIEND in file 2
TEST_F_FRIEND(BoxBTest, AccessFromFile2) {
    EXPECT_DOUBLE_EQ(box.data_, 2.5);
    printf("File2 TEST_F_FRIEND: data=%.2f\n", box.data_);
}

// TEST_G_FRIEND in file 2
struct BoxBGenTest : ::gtest_generator::TestWithGenerator {
    BoxB box{9.99};
};

TEST_G_FRIEND(BoxBGenTest, GeneratorFromFile2) {
    int multiplier = GENERATOR(10, 20, 30);
    USE_GENERATOR();

    EXPECT_DOUBLE_EQ(box.data_, 9.99);

    printf("File2 TEST_G_FRIEND: multiplier=%d, data=%.2f\n", multiplier, box.data_);
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

struct SharedBoxTest2 : ::gtest_generator::TestWithGenerator {
    SharedBox box{"file2", 20};
};

TEST_G_FRIEND(SharedBoxTest2, FromFile2) {
    int val = GENERATOR(5, 10);
    USE_GENERATOR();

    EXPECT_EQ(box.name_, "file2");
    EXPECT_EQ(box.count_, 20);

    printf("File2 SharedBox: val=%d, name=%s, count=%d\n", val, box.name_.c_str(), box.count_);
}
