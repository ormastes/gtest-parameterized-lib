#include "gtest_generator.h"
#include <string>

// Example class with private members
class MyClass {
private:
    int privateValue;
    std::string privateName;

public:
    MyClass(int v, const std::string& n) : privateValue(v), privateName(n) {}

    // Make accessPrivateMember a friend
    FRIEND_ACCESS_PRIVATE();
};

// Test fixture (declare before DECLARE_ACCESS_PRIVATE)
class PrivateAccessTest : public ::gtest_generator::TestWithGenerator {
};

// Use 'using' to avoid :: in template parameters for CONCAT
using TestBase = gtest_generator::TestWithGenerator;

// Declare accessor for privateValue - pass just the field name
DECLARE_ACCESS_PRIVATE(ignored, TestBase, MyClass, privateValue);

// Declare accessor for privateName
DECLARE_ACCESS_PRIVATE(ignored2, TestBase, MyClass, privateName);

// Another test class
class AnotherClass {
private:
    double secretData;

public:
    AnotherClass(double d) : secretData(d) {}

    FRIEND_ACCESS_PRIVATE();
};

DECLARE_ACCESS_PRIVATE(ignored3, TestBase, AnotherClass, secretData);

// Test accessing private integer member
TEST_G(PrivateAccessTest, AccessPrivateInt) {
    int testValue = GENERATOR(10, 20, 30);
    USE_GENERATOR();

    MyClass obj(testValue, "test");

    // Access private member - automatically passes 'this' as first parameter
    int& privateRef = ACCESS_PRIVATE(TestBase, TestBase_MyClass_privateValue, MyClass, &obj);

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
    std::string& nameRef = ACCESS_PRIVATE(TestBase, TestBase_MyClass_privateName, MyClass, &obj);

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

    double& secretRef = ACCESS_PRIVATE(TestBase, TestBase_AnotherClass_secretData, AnotherClass, &obj);

    EXPECT_DOUBLE_EQ(secretRef, testData);

    printf("Accessed secretData=%f\n", secretRef);
}

// Test with default nullptr parameter
TEST_G(PrivateAccessTest, DefaultNullptr) {
    int value = GENERATOR(5, 10);
    USE_GENERATOR();

    MyClass obj(value, "demo");

    // Can omit second parameter (defaults to nullptr)
    int& ref = ACCESS_PRIVATE(TestBase, TestBase_MyClass_privateValue, MyClass, &obj);

    EXPECT_EQ(ref, value);

    printf("Value=%d\n", value);
}
