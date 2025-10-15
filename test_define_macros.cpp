#include "gtest_generator.h"
#include <string>

// Test class with private members
class TestDefineClass {
private:
    int value;
    static int staticValue;
public:
    TestDefineClass(int v) : value(v) {}

    GTESTG_FRIEND_ACCESS_PRIVATE();
};

int TestDefineClass::staticValue = 42;

// Test fixture
class DefineTest : public ::gtest_generator::TestWithGenerator {
};

// Test using DECLARE macros
GTESTG_PRIVATE_DECLARE_MEMBER(TestDefineClass, value);
GTESTG_PRIVATE_DECLARE_STATIC(TestDefineClass, staticValue);

// Custom function that uses test context
GTESTG_PRIVATE_DECLARE_FUNCTION(DefineTest, TestDefineClass, GetDoubleValue) {
    // Can access THIS for test context if needed
    return TARGET->value * 2;
}

TEST_G(DefineTest, UseDECLARE_Macros) {
    int testValue = GENERATOR(100, 200);
    USE_GENERATOR();

    TestDefineClass obj(testValue);

    // Access using the DECLARE macros
    int& val = GTESTG_PRIVATE_MEMBER(TestDefineClass, value, &obj);
    EXPECT_EQ(val, testValue);

    // Access static member - note: static members return by reference
    decltype(auto) staticVal = GTESTG_PRIVATE_STATIC(TestDefineClass, staticValue);
    EXPECT_EQ(staticVal, 42);

    // Modify static value
    staticVal = 100;
    EXPECT_EQ(staticVal, 100);

    // Reset for other tests
    staticVal = 42;

    printf("Test with DECLARE macros: value=%d\n", val);
}

TEST_G(DefineTest, TestCALL_Macros) {
    int testValue = GENERATOR(50, 75);
    USE_GENERATOR();

    TestDefineClass obj(testValue);

    // Test GTESTG_PRIVATE_CALL_ON_TEST (uses 'this')
    int doubled1 = GTESTG_PRIVATE_CALL_ON_TEST(DefineTest, TestDefineClass, GetDoubleValue, &obj);
    EXPECT_EQ(doubled1, testValue * 2);

    // Test GTESTG_PRIVATE_CALL (passes test object explicitly as third parameter)
    // Need to cast to base fixture type since function was declared with DefineTest
    int doubled2 = GTESTG_PRIVATE_CALL(TestDefineClass, GetDoubleValue, *static_cast<DefineTest*>(this), &obj);
    EXPECT_EQ(doubled2, testValue * 2);

    printf("Test CALL macros: value=%d, doubled=%d\n", testValue, doubled1);
}
