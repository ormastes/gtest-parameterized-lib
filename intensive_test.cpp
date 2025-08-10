#include "gtest_generator.h"
#include <string>
#include <vector>
#include <memory>

// Test fixture for basic integer tests
class IntensiveTest : public ::gtest_generator::TestWithGenerator {
};

// Test fixture for pointer tests
class PointerTest : public ::gtest_generator::TestWithGenerator {
protected:
    void SetUp() override {
        // Allocate test pointers
        ptr1 = new int(100);
        ptr2 = new int(200);
        ptr3 = new int(300);
    }
    
    void TearDown() override {
        delete ptr1;
        delete ptr2;
        delete ptr3;
    }
    
    int* ptr1;
    int* ptr2;
    int* ptr3;
};

// Test fixture for object tests
struct TestObject {
    int value;
    std::string name;
    
    TestObject(int v, const std::string& n) : value(v), name(n) {}
    
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

class ObjectTest : public ::gtest_generator::TestWithGenerator {
protected:
    TestObject obj1{10, "first"};
    TestObject obj2{20, "second"};
    TestObject obj3{30, "third"};
};

// Multiple test cases on same fixture
TEST_G(IntensiveTest, BasicArithmetic) {
    int a = GENERATOR(1, 2, 3);
    int b = GENERATOR(10, 20);
    int op = GENERATOR(0, 1);  // 0=add, 1=multiply
    USE_GENERATOR();
    
    int result = (op == 0) ? (a + b) : (a * b);
    
    if (op == 0) {
        EXPECT_EQ(result, a + b);
        printf("Test: %d + %d = %d\n", a, b, result);
    } else {
        EXPECT_EQ(result, a * b);
        printf("Test: %d * %d = %d\n", a, b, result);
    }
}

TEST_G(IntensiveTest, Comparison) {
    int x = GENERATOR(5, 10, 15);
    int y = GENERATOR(8, 12);
    USE_GENERATOR();
    
    bool less = x < y;
    bool greater = x > y;
    bool equal = x == y;
    
    EXPECT_EQ(less, x < y);
    EXPECT_EQ(greater, x > y);
    EXPECT_EQ(equal, x == y);
    
    printf("Comparison: %d vs %d (less=%d, greater=%d, equal=%d)\n", 
           x, y, less, greater, equal);
}

TEST_G(IntensiveTest, ArrayIndexing) {
    int arr[] = {100, 200, 300, 400, 500};
    int idx1 = GENERATOR(0, 1, 2);
    int idx2 = GENERATOR(3, 4);
    USE_GENERATOR();
    
    EXPECT_LT(arr[idx1], arr[idx2]);
    printf("Array test: arr[%d]=%d < arr[%d]=%d\n", 
           idx1, arr[idx1], idx2, arr[idx2]);
}

// Pointer tests
TEST_G(PointerTest, PointerArithmetic) {
    int* ptrs[] = {ptr1, ptr2, ptr3};
    int idx = GENERATOR(0, 1, 2);
    int offset = GENERATOR(0, 1);
    USE_GENERATOR();
    
    int value = *ptrs[idx];
    int modified = value + offset * 10;
    
    EXPECT_GE(modified, value);
    printf("Pointer test: *ptrs[%d]=%d, modified=%d\n", idx, value, modified);
}

TEST_G(PointerTest, PointerComparison) {
    int values[] = {10, 20, 30};
    int* p1 = &values[GENERATOR(0, 1)];
    int* p2 = &values[GENERATOR(1, 2)];
    USE_GENERATOR();
    
    EXPECT_LE(p1, p2);  // Address comparison
    EXPECT_LE(*p1, *p2); // Value comparison
    
    printf("Pointer comparison: p1=%p(*p1=%d) <= p2=%p(*p2=%d)\n", 
           (void*)p1, *p1, (void*)p2, *p2);
}

// Object tests
TEST_G(ObjectTest, ObjectComparison) {
    TestObject* objs[] = {&obj1, &obj2, &obj3};
    int idx1 = GENERATOR(0, 1);
    int idx2 = GENERATOR(1, 2);
    USE_GENERATOR();
    
    if (idx1 < idx2) {
        EXPECT_LT(*objs[idx1], *objs[idx2]);
    } else {
        EXPECT_GE(*objs[idx1], *objs[idx2]);
    }
    printf("Object comparison: %s(%d) < %s(%d)\n", 
           objs[idx1]->name.c_str(), objs[idx1]->value,
           objs[idx2]->name.c_str(), objs[idx2]->value);
}

TEST_G(ObjectTest, ObjectProperties) {
    TestObject objects[] = {
        TestObject(GENERATOR(1, 2), "test"),
        TestObject(GENERATOR(10, 20), "demo")
    };
    USE_GENERATOR();
    
    EXPECT_LT(objects[0].value, objects[1].value);
    EXPECT_NE(objects[0].name, objects[1].name);
    
    printf("Object properties: obj[0]={%d,%s}, obj[1]={%d,%s}\n",
           objects[0].value, objects[0].name.c_str(),
           objects[1].value, objects[1].name.c_str());
}

// Complex nested generators
class ComplexTest : public ::gtest_generator::TestWithGenerator {
};

TEST_G(ComplexTest, NestedLogic) {
    int category = GENERATOR(0, 1, 2);  // 3 categories
    int subcategory = GENERATOR(0, 1); // 2 subcategories
    int value = GENERATOR(100, 200);   // 2 values
    USE_GENERATOR();
    
    // Complex business logic based on combinations
    int result = 0;
    if (category == 0) {
        result = value + subcategory * 10;
    } else if (category == 1) {
        result = value * (subcategory + 1);
    } else {
        result = value - subcategory * 50;
    }
    
    EXPECT_GT(result, 0);
    printf("Complex test: cat=%d, subcat=%d, val=%d => result=%d\n",
           category, subcategory, value, result);
}

// STL container tests
class STLTest : public ::gtest_generator::TestWithGenerator {
};

TEST_G(STLTest, VectorOperations) {
    std::vector<int> vec;
    int size = GENERATOR(1, 2, 3);
    int multiplier = GENERATOR(10, 100);
    USE_GENERATOR();
    
    // Fill vector
    for (int i = 0; i < size; ++i) {
        vec.push_back(i * multiplier);
    }
    
    EXPECT_EQ(vec.size(), static_cast<size_t>(size));
    if (!vec.empty()) {
        EXPECT_EQ(vec.back(), (size - 1) * multiplier);
    }
    
    printf("Vector test: size=%d, multiplier=%d, elements=[", size, multiplier);
    for (int v : vec) printf("%d ", v);
    printf("]\n");
}

TEST_G(STLTest, StringOperations) {
    std::string prefix = GENERATOR(0, 1) ? "Hello" : "Hi";
    std::string suffix = GENERATOR(0, 1) ? "World" : "There";
    int repeat = GENERATOR(1, 2);
    USE_GENERATOR();
    
    std::string result = "";
    for (int i = 0; i < repeat; ++i) {
        if (i > 0) result += " ";
        result += prefix + " " + suffix;
    }
    
    EXPECT_FALSE(result.empty());
    EXPECT_GT(result.length(), 5u);
    
    printf("String test: prefix='%s', suffix='%s', repeat=%d => '%s'\n",
           prefix.c_str(), suffix.c_str(), repeat, result.c_str());
}

// Edge case tests
class EdgeCaseTest : public ::gtest_generator::TestWithGenerator {
};

TEST_G(EdgeCaseTest, BoundaryValues) {
    int low = GENERATOR(-1, 0, 1);
    int high = GENERATOR(99, 100, 101);
    USE_GENERATOR();
    
    // Test boundary conditions
    bool in_range = (low >= 0 && high <= 100);
    
    printf("Boundary test: low=%d, high=%d, in_range=%d\n", low, high, in_range);
    
    if (in_range) {
        EXPECT_GE(low, 0);
        EXPECT_LE(high, 100);
    }
} 