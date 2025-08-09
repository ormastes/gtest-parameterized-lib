#include "gtest_generator.h"
// Fixture class using TestWithParam<int>
class MyTest : public ::gtest_generator::TestWithGenerator {
};

TEST_G(MyTest, SimpleCase) {
    auto a = GENERATOR(1, 2);
    auto b = GENERATOR(10, 20);
    USE_GENERATOR() 

    EXPECT_LT(a, b);
    printf("Test case: a=%d, b=%d\n", a, b);
}
ENABLE_GENERATOR(MyTest, SimpleCase);


TEST_G(MyTest, SimpleCase2) {
    auto a = GENERATOR(1, 2);
    auto b = GENERATOR(10, 20, 30);
    USE_GENERATOR() 
    
    EXPECT_LT(a, b);
    printf("Test case: a=%d, b=%d\n", a, b);
}
ENABLE_GENERATOR(MyTest, SimpleCase2);


TEST_G(MyTest, SimpleCase3) {
    auto a = GENERATOR(1, 2);
    auto b = GENERATOR(10);
    USE_GENERATOR() 
    
    EXPECT_LT(a, b);
    printf("Test case: a=%d, b=%d\n", a, b);
}
ENABLE_GENERATOR(MyTest, SimpleCase3);

struct Simple {
    int a;
    int b;
};

TEST_G(MyTest, StructCase) {
    auto a = GENERATOR(Simple{1, 2}, Simple{3, 4});
    auto b = GENERATOR(Simple{10, 20}, Simple{30, 40});
    USE_GENERATOR() 
    
    EXPECT_LT(a.a, b.a);
    EXPECT_LT(a.b, b.b);
    printf("Struct case: a={%d, %d}, b={%d, %d}\n", 
           a.a, a.b, b.a, b.b);
}
ENABLE_GENERATOR(MyTest, StructCase);

class TestObject {
    public:
    TestObject(int v, const std::string& n) : value(v), name(n) {}
    int value;
    std::string name;
    bool operator<(const TestObject& other) const {
        return value < other.value;
    }
    bool operator>=(const TestObject& other) const {
        return value >= other.value;
    }
    bool operator==(const TestObject& other) const {
        return value == other.value && name == other.name;
    }
};
TEST_G(MyTest, ObjectCase) {
    TestObject obj1(GENERATOR(1, 2), "first");
    TestObject obj2(GENERATOR(10, 20), "second");
    USE_GENERATOR() 
    
    EXPECT_LT(obj1, obj2);
    printf("Object case: obj1={%d, %s}, obj2={%d, %s}\n", 
           obj1.value, obj1.name.c_str(), 
           obj2.value, obj2.name.c_str());
}
ENABLE_GENERATOR(MyTest, ObjectCase);

TEST_G(MyTest, ObjectCase2) {
    auto obj1 = GENERATOR(TestObject(1, "first"), TestObject(2, "second"));
    auto obj2 = GENERATOR(TestObject(10, "ten"), TestObject(20, "twenty"));
    USE_GENERATOR() 
    
    EXPECT_LT(obj1, obj2);
    printf("Object case 2: obj1={%d, %s}, obj2={%d, %s}\n", 
           obj1.value, obj1.name.c_str(), 
           obj2.value, obj2.name.c_str());  
}
ENABLE_GENERATOR(MyTest, ObjectCase2);

TEST_G(MyTest, ObjectCase3) {
    auto obj1 = GENERATOR(new TestObject(GENERATOR(1, 2), "first"),
                          new TestObject(GENERATOR(3, 4), "second"));
    auto obj2 = GENERATOR(new TestObject(GENERATOR(10, 20), "ten"),
                          new TestObject(GENERATOR(30, 40), "twenty"));
    USE_GENERATOR() 
    
    EXPECT_LT(*obj1, *obj2);
    printf("Object case 2: obj1={%d, %s}, obj2={%d, %s}\n", 
           obj1->value, obj1->name.c_str(), 
           obj2->value, obj2->name.c_str());  
}
ENABLE_GENERATOR(MyTest, ObjectCase3);