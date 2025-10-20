#include "gtest_generator.h"
#include <string>

// ============================================================================
// Demonstrate the new TEST_FRIEND and TEST_G_FRIEND macros
// GTESTG_FRIEND_ACCESS_PRIVATE() provides BOTH:
//   - Class-based friend access (VirtualAccessor for TEST_FRIEND/TEST_G_FRIEND)
//   - Function-based friend access (gtestg_private_accessMember for GTESTG_PRIVATE_MEMBER)
// ============================================================================

class Widget {
private:
    int secret_ = 42;
    std::string name_ = "widget";

public:
    Widget() = default;
    Widget(int s, const std::string& n) : secret_(s), name_(n) {}

    // Grant friend access for both class-based and function-based approaches
    GTESTG_FRIEND_ACCESS_PRIVATE();

    int getPublic() const { return secret_ / 2; }
};

// Declare function-based accessors
GTESTG_PRIVATE_DECLARE_MEMBER(Widget, secret_);
GTESTG_PRIVATE_DECLARE_MEMBER(Widget, name_);

// ============================================================================
// TEST_FRIEND: TEST_F-style with VirtualAccessor infrastructure
// Use GTESTG_PRIVATE_MEMBER for actual private access
// ============================================================================
struct WidgetTest : ::testing::Test {
    Widget w;
};

TEST_FRIEND(WidgetTest, AccessPrivateViaFunctionMacros) {
    // Access private members using function-based macros
    int& secret = GTESTG_PRIVATE_MEMBER(Widget, secret_, &w);
    std::string& name = GTESTG_PRIVATE_MEMBER(Widget, name_, &w);

    EXPECT_EQ(secret, 42);
    EXPECT_EQ(name, "widget");

    // Modify
    secret = 100;
    EXPECT_EQ(secret, 100);

    printf("TEST_FRIEND: secret=%d, name=%s\n", secret, name.c_str());
}

// ============================================================================
// TEST_G_FRIEND: Generator-based with VirtualAccessor infrastructure
// Use GTESTG_PRIVATE_MEMBER for actual private access
// ============================================================================
struct WidgetGenTest : ::gtest_generator::TestWithGenerator {
    Widget w{999, "generated"};
};

TEST_G_FRIEND(WidgetGenTest, AccessWithGenerator) {
    int factor = GENERATOR(1, 2, 5);
    USE_GENERATOR();

    // Access private members using function-based macros
    int& secret = GTESTG_PRIVATE_MEMBER(Widget, secret_, &w);
    std::string& name = GTESTG_PRIVATE_MEMBER(Widget, name_, &w);

    EXPECT_EQ(secret, 999);
    EXPECT_EQ(name, "generated");

    printf("TEST_G_FRIEND: factor=%d, secret=%d, name=%s\n",
           factor, secret, name.c_str());
}

TEST_G_FRIEND(WidgetGenTest, ModifyWithGenerator) {
    int newSecret = GENERATOR(10, 20, 30);
    USE_GENERATOR();

    // Access and modify
    int& secret = GTESTG_PRIVATE_MEMBER(Widget, secret_, &w);

    // Reset to known value
    secret = 999;
    EXPECT_EQ(secret, 999);

    // Modify to generated value
    secret = newSecret;
    EXPECT_EQ(secret, newSecret);

    printf("TEST_G_FRIEND: Set secret=%d\n", newSecret);
}
