# GTest Generator - Parameterized Test Library

*Read this in other languages: [한국어](README.ko.md), [日本語](README.ja.md), [中文](README.zh.md), [Español](README.es.md), [Français](README.fr.md), [Deutsch](README.de.md), [Italiano](README.it.md), [Português](README.pt-BR.md)*

A simple header-only library that provides Catch2-style `GENERATE` syntax for Google Test parameterized tests.

## Features

- Simple, intuitive syntax similar to Catch2's `GENERATE`
- Header-only implementation
- Automatic generation of all test combinations
- Compatible with standard Google Test `TEST_P` macros

## Quick Start

```cpp
#include "gtest_generator.h"

// Define test fixture using TestWithGenerator
class MyTest : public ::gtest_generator::TestWithGenerator {
};

// Write parameterized test using TEST_G macro
TEST_G(MyTest, SimpleCase) {
    // IMPORTANT: All GENERATOR() calls must come BEFORE USE_GENERATOR()
    int a = GENERATOR(1, 2);      // Will be 1 or 2
    int b = GENERATOR(10, 20);    // Will be 10 or 20
    USE_GENERATOR();              // Must be called AFTER all GENERATOR() calls

    EXPECT_LT(a, b);
    printf("Test: a=%d, b=%d\n", a, b);
}
// TEST_G automatically handles test generation - no ENABLE_GENERATOR needed!
```

## Installation

This is a header-only library. Simply copy `gtest_generator.h` to your project and include it.

## Building the Project

### Prerequisites

- CMake 3.14 or higher
- Google Test library
- C++14 compatible compiler

### Build Instructions

```bash
# Create build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build the project
cmake --build .

# Run the tests
./mygen_test
```

### Alternative Build Commands

```bash
# For parallel builds
cmake --build . -j$(nproc)

# For Debug build
cmake -DCMAKE_BUILD_TYPE=Debug ..

# For Release build
cmake -DCMAKE_BUILD_TYPE=Release ..
```

## More Examples

### Multiple Values
```cpp
TEST_G(MyTest, MultipleValues) {
    int x = GENERATOR(1, 2, 3);       // 3 values
    int y = GENERATOR(10, 20, 30);    // 3 values
    USE_GENERATOR();
    
    // Generates 9 test combinations
    EXPECT_LT(x, y);
}
```

### Different Number of Values
```cpp
TEST_G(MyTest, AsymmetricCombinations) {
    int size = GENERATOR(1, 2, 3);     // 3 values
    int scale = GENERATOR(10, 100);    // 2 values
    USE_GENERATOR();
    
    // Generates 6 test combinations
    int result = size * scale;
    EXPECT_GT(result, 0);
}
```

### Using Generated Values in Expressions
```cpp
TEST_G(MyTest, ComputedValues) {
    int base = GENERATOR(1, 2);
    int multiplier = GENERATOR(10, 20, 30);
    USE_GENERATOR();
    
    std::vector<int> data;
    for (int i = 0; i < base; ++i) {
        data.push_back(i * multiplier);
    }
    
    EXPECT_EQ(data.size(), base);
    if (!data.empty()) {
        EXPECT_EQ(data.back(), (base - 1) * multiplier);
    }
}MyTest, ComputedValues);
```

### Complex Test Logic
```cpp
TEST_G(MyTest, ConditionalLogic) {
    int mode = GENERATOR(0, 1, 2);     // 3 modes
    int value = GENERATOR(100, 200);   // 2 values
    USE_GENERATOR();
    
    int result;
    switch (mode) {
        case 0: result = value + 10; break;
        case 1: result = value * 2; break;
        case 2: result = value - 50; break;
    }
    
    EXPECT_GT(result, 0);
    printf("Mode %d with value %d gives result %d\n", mode, value, result);
}MyTest, ConditionalLogic);
```

## Advanced Examples

### Working with Classes and Objects

#### Direct Object Generation
```cpp
class TestObject {
public:
    TestObject(int v, const std::string& n) : value(v), name(n) {}
    int value;
    std::string name;
    bool operator<(const TestObject& other) const {
        return value < other.value;
    }
};

TEST_G(MyTest, ObjectGeneration) {
    // Generate complete objects directly
    auto obj1 = GENERATOR(TestObject(1, "first"), TestObject(2, "second"));
    auto obj2 = GENERATOR(TestObject(10, "ten"), TestObject(20, "twenty"));
    USE_GENERATOR();
    
    EXPECT_LT(obj1, obj2);
    printf("Objects: obj1={%d, %s}, obj2={%d, %s}\n", 
           obj1.value, obj1.name.c_str(), 
           obj2.value, obj2.name.c_str());
}MyTest, ObjectGeneration);
```

#### Using GENERATOR in Constructor Arguments
```cpp
TEST_G(MyTest, ConstructorWithGenerators) {
    // GENERATOR values used as constructor arguments
    int val1 = GENERATOR(1, 2);
    int val2 = GENERATOR(10, 20);
    USE_GENERATOR();
    
    TestObject objects[] = {
        TestObject(val1, "test"),
        TestObject(val2, "demo")
    };
    
    EXPECT_LT(objects[0].value, objects[1].value);
    printf("Array objects: [0]={%d,%s}, [1]={%d,%s}\n",
           objects[0].value, objects[0].name.c_str(),
           objects[1].value, objects[1].name.c_str());
}MyTest, ConstructorWithGenerators);
```

### Working with Pointers and Dynamic Memory

#### Generating Pointers to Objects
```cpp
TEST_G(MyTest, PointerGeneration) {
    // Generate pointers to different objects
    // Note: Be careful with memory management
    auto* ptr1 = GENERATOR(new TestObject(1, "first"), 
                          new TestObject(2, "second"));
    auto* ptr2 = GENERATOR(new TestObject(10, "ten"), 
                          new TestObject(20, "twenty"));
    USE_GENERATOR();
    
    EXPECT_LT(*ptr1, *ptr2);
    printf("Pointers: ptr1={%d, %s}, ptr2={%d, %s}\n", 
           ptr1->value, ptr1->name.c_str(), 
           ptr2->value, ptr2->name.c_str());
    
    // Clean up
    delete ptr1;
    delete ptr2;
}MyTest, PointerGeneration);
```

#### Nested GENERATOR Calls (Advanced)
```cpp
TEST_G(MyTest, NestedGenerators) {
    // Complex nested generation - each outer GENERATOR contains inner GENERATOR calls
    int inner1 = GENERATOR(1, 2);
    int inner2 = GENERATOR(3, 4);
    int inner3 = GENERATOR(10, 20);
    int inner4 = GENERATOR(30, 40);
    USE_GENERATOR();
    
    auto* obj1 = new TestObject(inner1, "first");
    auto* obj2 = new TestObject(inner3, "second");
    
    EXPECT_LT(obj1->value, obj2->value);
    printf("Nested: obj1={%d}, obj2={%d}\n", obj1->value, obj2->value);
    
    delete obj1;
    delete obj2;
}MyTest, NestedGenerators);
```

### Working with STL Containers

#### Generating Container Sizes and Contents
```cpp
TEST_G(MyTest, STLContainers) {
    auto size = GENERATOR(1, 2, 3);
    auto multiplier = GENERATOR(10, 100);
    USE_GENERATOR();
    
    std::vector<int> vec;
    for (int i = 0; i < size; ++i) {
        vec.push_back(i * multiplier);
    }
    
    EXPECT_EQ(vec.size(), size);
    if (!vec.empty()) {
        EXPECT_EQ(vec.back(), (size - 1) * multiplier);
    }
    
    printf("Vector: size=%d, multiplier=%d, elements=[", size, multiplier);
    for (int v : vec) printf("%d ", v);
    printf("]\n");
}MyTest, STLContainers);
```

#### Generating String Combinations
```cpp
TEST_G(MyTest, StringCombinations) {
    auto prefix_choice = GENERATOR(0, 1);
    auto suffix_choice = GENERATOR(0, 1);
    auto repeat = GENERATOR(1, 2);
    USE_GENERATOR();
    
    std::string prefix = prefix_choice ? "Hello" : "Hi";
    std::string suffix = suffix_choice ? "World" : "There";
    
    std::string result;
    for (int i = 0; i < repeat; ++i) {
        if (i > 0) result += " ";
        result += prefix + " " + suffix;
    }
    
    EXPECT_FALSE(result.empty());
    printf("String: prefix='%s', suffix='%s', repeat=%d => '%s'\n",
           prefix.c_str(), suffix.c_str(), repeat, result.c_str());
}MyTest, StringCombinations);
```

### Working with Smart Pointers

#### Using unique_ptr with GENERATOR
```cpp
TEST_G(MyTest, SmartPointers) {
    auto value1 = GENERATOR(1, 2);
    auto value2 = GENERATOR(10, 20);
    USE_GENERATOR();
    
    auto ptr1 = std::make_unique<TestObject>(value1, "first");
    auto ptr2 = std::make_unique<TestObject>(value2, "second");
    
    EXPECT_LT(*ptr1, *ptr2);
    printf("Smart pointers: ptr1={%d, %s}, ptr2={%d, %s}\n",
           ptr1->value, ptr1->name.c_str(),
           ptr2->value, ptr2->name.c_str());
}MyTest, SmartPointers);
```

### Complex Struct Examples

#### Generating Structs with Multiple Fields
```cpp
struct Point {
    int x, y;
    Point(int x_, int y_) : x(x_), y(y_) {}
};

TEST_G(MyTest, StructGeneration) {
    auto p1 = GENERATOR(Point{0, 0}, Point{1, 1});
    auto p2 = GENERATOR(Point{10, 10}, Point{20, 20});
    USE_GENERATOR();
    
    int distance = abs(p2.x - p1.x) + abs(p2.y - p1.y);
    EXPECT_GT(distance, 0);
    printf("Points: p1=(%d,%d), p2=(%d,%d), distance=%d\n",
           p1.x, p1.y, p2.x, p2.y, distance);
}MyTest, StructGeneration);
```

## Sampling Modes

The library supports two sampling modes for generating test combinations:

### FULL Mode (Default - Cartesian Product)
The default mode generates all possible combinations of values (Cartesian product). This is the traditional behavior that ensures complete test coverage.

```cpp
TEST_G(MyTest, FullMode) {
    USE_GENERATOR();  // Default is FULL mode
    // or explicitly: USE_GENERATOR(FULL);
    
    auto x = GENERATOR(1, 2);      // 2 values
    auto y = GENERATOR(10, 20);    // 2 values
    auto z = GENERATOR(100, 200);  // 2 values
    
    // Generates 8 test runs: 2 × 2 × 2 = 8 combinations
    // (1,10,100), (1,10,200), (1,20,100), (1,20,200),
    // (2,10,100), (2,10,200), (2,20,100), (2,20,200)
}
```

### ALIGNED Mode (Parallel Iteration)
ALIGNED mode iterates through all columns in parallel, like a zipper. Each column advances to its next value on each run, wrapping around when it reaches the end. The total number of runs equals the size of the largest column.

```cpp
TEST_G(MyTest, AlignedMode) {
    USE_GENERATOR(ALIGNED);  // Enable ALIGNED mode
    
    auto x = GENERATOR(1, 2);           // 2 values
    auto y = GENERATOR(10, 20, 30, 40); // 4 values  
    auto z = GENERATOR(100, 200, 300);  // 3 values
    
    // Generates 4 test runs (max column size):
    // Run 0: (1, 10, 100)  - all at index 0
    // Run 1: (2, 20, 200)  - all at index 1
    // Run 2: (1, 30, 300)  - x wraps to 0, others at index 2
    // Run 3: (2, 40, 100)  - x at 1, y at 3, z wraps to 0
}
```

#### Key Characteristics of ALIGNED Mode:
- **Deterministic**: Values are selected in order (0, 1, 2, ...) with wrapping
- **Declaration Order**: Columns are processed in the order they are declared
- **Fewer Runs**: Total runs = maximum column size (not the product)
- **Balanced Coverage**: Each value in each column is used approximately equally

#### When to Use Each Mode:
- **FULL Mode**: When you need exhaustive testing of all combinations
- **ALIGNED Mode**: When you want representative sampling with fewer test runs

#### Example Comparison:
```cpp
// FULL mode: 3 × 2 × 2 = 12 runs
TEST_G(MyTest, FullExample) {
    USE_GENERATOR(FULL);
    auto a = GENERATOR(1, 2, 3);
    auto b = GENERATOR(10, 20);
    auto c = GENERATOR(100, 200);
    // Generates all 12 combinations
}

// ALIGNED mode: max(3, 2, 2) = 3 runs  
TEST_G(MyTest, AlignedExample) {
    USE_GENERATOR(ALIGNED);
    auto a = GENERATOR(1, 2, 3);
    auto b = GENERATOR(10, 20);
    auto c = GENERATOR(100, 200);
    // Generates only 3 combinations:
    // (1, 10, 100), (2, 20, 200), (3, 10, 100)
}
```

## API Reference

### Macros

- **`TEST_G(TestClassName, TestName)`** - Defines a parameterized test with automatic test generation. Replaces both TEST_P and the need for separate ENABLE_GENERATOR calls.

- **`GENERATOR(...)`** - Defines values for a test parameter. Each call creates a new dimension of test combinations.
  ```cpp
  int value = GENERATOR(1, 2, 3);  // Creates 3 variants
  ```
  **IMPORTANT**: All GENERATOR() calls must come BEFORE USE_GENERATOR()

- **`USE_GENERATOR()`** - Must be called once in each TEST_G, AFTER all GENERATOR() calls. Uses FULL mode by default.

- **`USE_GENERATOR(mode)`** - Must be called once in each TEST_G, AFTER all GENERATOR() calls. Specifies the sampling mode:
  - `USE_GENERATOR(FULL)` - Cartesian product of all values (same as default)
  - `USE_GENERATOR(ALIGNED)` - Parallel iteration through columns

## How It Works

The library uses thread-local static variables to maintain generator state across test instantiations. When `GENERATOR` is called:

1. It registers the possible values for that generator
2. Based on the current test parameter index, it calculates which value to return
3. All combinations are automatically generated

## Important Usage Notes

### Correct Order is Critical

✅ **CORRECT**:
```cpp
TEST_G(MyTest, Example) {
    int a = GENERATOR(1, 2);     // First: Define generators
    int b = GENERATOR(10, 20);   // 
    USE_GENERATOR();             // Then: Call USE_GENERATOR()
    // Test logic here
}
```

❌ **INCORRECT**:
```cpp
TEST_G(MyTest, Example) {
    USE_GENERATOR();             // Wrong! This must come after GENERATOR calls
    int a = GENERATOR(1, 2);     
    int b = GENERATOR(10, 20);   
    // Test logic here
}
```

## Example Output

```
[==========] Running 4 tests from 1 test suite.
[----------] 4 tests from Generator/MyTest__SimpleCase
[ RUN      ] Generator/MyTest__SimpleCase.__/0
Test: a=1, b=10
[       OK ] Generator/MyTest__SimpleCase.__/0 (0 ms)
[ RUN      ] Generator/MyTest__SimpleCase.__/1
Test: a=2, b=10
[       OK ] Generator/MyTest__SimpleCase.__/1 (0 ms)
[ RUN      ] Generator/MyTest__SimpleCase.__/2
Test: a=1, b=20
[       OK ] Generator/MyTest__SimpleCase.__/2 (0 ms)
[ RUN      ] Generator/MyTest__SimpleCase.__/3
Test: a=2, b=20
[       OK ] Generator/MyTest__SimpleCase.__/3 (0 ms)
[==========] 4 tests from 1 test suite ran. (0 ms total)
[  PASSED  ] 4 tests.
```

## Limitations

- Test fixture must inherit from `::gtest_generator::TestWithGenerator`
- Must use `TEST_G` macro instead of standard `TEST_P`
- All `GENERATOR()` calls must come before `USE_GENERATOR()`
- Complex types (objects, pointers) work with GENERATOR but may require proper template instantiation
- When using GENERATOR in constructor arguments, store the generated value in a variable first
- Memory management is user's responsibility when generating pointers with `new`
- Thread-local storage is used, so behavior in multi-threaded test environments may need consideration

## Future Improvements

- Dynamic calculation of total combinations
- Support for different data types in generators
- Named test instantiations
- Support for more complex value patterns

## License

This project is provided as-is for educational and development purposes.