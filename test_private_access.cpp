#include "gtest_generator.h"
#include <string>

// Example class with private members
class MyClass {
private:
    int privateValue;
    std::string privateName;

public:
    MyClass(int v, const std::string& n) : privateValue(v), privateName(n) {}

    // Make gtestg_private_accessMember a friend
    GTESTG_FRIEND_ACCESS_PRIVATE();
};

// Test fixture
class PrivateAccessTest : public ::gtest_generator::TestWithGenerator {
};

// Declare accessor for privateValue - simplified API
GTESTG_PRIVATE_DECLARE_MEMBER(MyClass, privateValue);

// Declare accessor for privateName
GTESTG_PRIVATE_DECLARE_MEMBER(MyClass, privateName);

// Another test class
class AnotherClass {
private:
    double secretData;

public:
    AnotherClass(double d) : secretData(d) {}

    GTESTG_FRIEND_ACCESS_PRIVATE();
};

GTESTG_PRIVATE_DECLARE_MEMBER(AnotherClass, secretData);

// Test accessing private integer member
TEST_G(PrivateAccessTest, AccessPrivateInt) {
    int testValue = GENERATOR(10, 20, 30);
    USE_GENERATOR();

    MyClass obj(testValue, "test");

    // Access private member - simplified API
    int& privateRef = GTESTG_PRIVATE_MEMBER(MyClass, privateValue, &obj);

    EXPECT_EQ(privateRef, testValue);

    // Modify it
    privateRef = 100;
    EXPECT_EQ(privateRef, 100);

    printf("Test with value=%d, modified to=%d\n", testValue, privateRef);
}

// Test accessing private string member
TEST_G(PrivateAccessTest, AccessPrivateString) {
    int value = GENERATOR(1, 2);
    USE_GENERATOR();

    MyClass obj(value, "secret");

    // Access privateName
    std::string& nameRef = GTESTG_PRIVATE_MEMBER(MyClass, privateName, &obj);

    EXPECT_EQ(nameRef, "secret");

    nameRef = "modified";
    EXPECT_EQ(nameRef, "modified");

    printf("Modified private string\n");
}

// Test accessing different class
TEST_G(PrivateAccessTest, AccessDifferentClass) {
    double testData = GENERATOR(1.5, 2.5, 3.5);
    USE_GENERATOR();

    AnotherClass obj(testData);

    double& secretRef = GTESTG_PRIVATE_MEMBER(AnotherClass, secretData, &obj);

    EXPECT_DOUBLE_EQ(secretRef, testData);

    printf("Accessed secretData=%f\n", secretRef);
}

// Test with default nullptr parameter
TEST_G(PrivateAccessTest, DefaultNullptr) {
    int value = GENERATOR(5, 10);
    USE_GENERATOR();

    MyClass obj(value, "demo");

    // Simplified member access
    int& ref = GTESTG_PRIVATE_MEMBER(MyClass, privateValue, &obj);

    EXPECT_EQ(ref, value);

    printf("Value=%d\n", value);
}
