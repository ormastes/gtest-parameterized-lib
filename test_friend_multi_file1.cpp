#include "gtest_generator.h"
#include <string>

// ============================================================================
// First file - defines target classes and some tests
// ============================================================================

class BoxA {
private:
    int value_ = 100;

public:
    BoxA() = default;
    BoxA(int v) : value_(v) {}

    GTESTG_FRIEND_ACCESS_PRIVATE();

    int getPublic() const { return value_ / 2; }
};

// Test fixture
struct BoxATest : ::testing::Test {
    BoxA box{42};
};

// TEST_F_FRIEND in file 1
TEST_F_FRIEND(BoxATest, AccessFromFile1) {
    EXPECT_EQ(box.value_, 42);
    printf("File1 TEST_F_FRIEND: value=%d\n", box.value_);
}

// TEST_G_FRIEND in file 1
struct BoxAGenTest : ::gtest_generator::TestWithGenerator {
    BoxA box{200};
};

TEST_G_FRIEND(BoxAGenTest, GeneratorFromFile1) {
    int factor = GENERATOR(1, 2);
    USE_GENERATOR();

    EXPECT_EQ(box.value_, 200);

    printf("File1 TEST_G_FRIEND: factor=%d, value=%d\n", factor, box.value_);
}

// ============================================================================
// Shared class used in both files
// ============================================================================
class SharedBox {
private:
    std::string name_ = "shared";
    int count_ = 0;

public:
    SharedBox() = default;
    SharedBox(const std::string& n, int c) : name_(n), count_(c) {}

    GTESTG_FRIEND_ACCESS_PRIVATE();
};

struct SharedBoxTest : ::gtest_generator::TestWithGenerator {
    SharedBox box{"file1", 10};
};

TEST_G_FRIEND(SharedBoxTest, FromFile1) {
    int val = GENERATOR(1, 2, 3);
    USE_GENERATOR();

    EXPECT_EQ(box.name_, "file1");
    EXPECT_EQ(box.count_, 10);

    printf("File1 SharedBox: val=%d, name=%s, count=%d\n", val, box.name_.c_str(), box.count_);
}
