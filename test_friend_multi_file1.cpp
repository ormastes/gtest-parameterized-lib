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

// Declare function-based accessors
GTESTG_PRIVATE_DECLARE_MEMBER(BoxA, value_);

// Test fixture
struct BoxATest : ::testing::Test {
    BoxA box{42};
};

// TEST_FRIEND in file 1
TEST_FRIEND(BoxATest, AccessFromFile1) {
    int& val = GTESTG_PRIVATE_MEMBER(BoxA, value_, &box);
    EXPECT_EQ(val, 42);
    printf("File1 TEST_FRIEND: value=%d\n", val);
}

// TEST_G_FRIEND in file 1
struct BoxAGenTest : ::gtest_generator::TestWithGenerator {
    BoxA box{200};
};

TEST_G_FRIEND(BoxAGenTest, GeneratorFromFile1) {
    int factor = GENERATOR(1, 2);
    USE_GENERATOR();

    int& val = GTESTG_PRIVATE_MEMBER(BoxA, value_, &box);
    EXPECT_EQ(val, 200);

    printf("File1 TEST_G_FRIEND: factor=%d, value=%d\n", factor, val);
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

// Declare accessors (will be used in both files)
GTESTG_PRIVATE_DECLARE_MEMBER(SharedBox, name_);
GTESTG_PRIVATE_DECLARE_MEMBER(SharedBox, count_);

struct SharedBoxTest : ::gtest_generator::TestWithGenerator {
    SharedBox box{"file1", 10};
};

TEST_G_FRIEND(SharedBoxTest, FromFile1) {
    int val = GENERATOR(1, 2, 3);
    USE_GENERATOR();

    std::string& name = GTESTG_PRIVATE_MEMBER(SharedBox, name_, &box);
    int& count = GTESTG_PRIVATE_MEMBER(SharedBox, count_, &box);

    EXPECT_EQ(name, "file1");
    EXPECT_EQ(count, 10);

    printf("File1 SharedBox: val=%d, name=%s, count=%d\n", val, name.c_str(), count);
}
