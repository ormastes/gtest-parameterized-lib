# GTest Generator - Parameterized Test Library

A simple header-only library that provides Catch2-style `GENERATE` syntax for Google Test parameterized tests.

## Features

- Simple, intuitive syntax similar to Catch2's `GENERATE`
- Header-only implementation
- Automatic generation of all test combinations
- Compatible with standard Google Test `TEST_P` macros

## Quick Start

```cpp
#include "gtest_generator.h"

// Define test fixture using TestWithParam<int>
class MyTest : public ::testing::TestWithParam<int> {
};

// Write parameterized test
TEST_P(MyTest, SimpleCase) {
    int a = GENERATOR(1, 2);      // Will be 1 or 2
    int b = GENERATOR(10, 20);    // Will be 10 or 20
    USE_GENERATOR();              // Required for proper cleanup

    EXPECT_LT(a, b);
}

// Enable generator (creates 4 test cases: 1,10 / 1,20 / 2,10 / 2,20)
ENABLE_GENERATOR(MyTest)
```

## Installation

This is a header-only library. Simply copy `gtest_generator.h` to your project and include it.

### Building the Example

```bash
mkdir build && cd build
cmake ..
make
./mygen_test
```

## API Reference

### Macros

- **`GENERATOR(...)`** - Defines values for a test parameter. Each call creates a new dimension of test combinations.
  ```cpp
  int value = GENERATOR(1, 2, 3);  // Creates 3 variants
  ```

- **`USE_GENERATOR()`** - Must be called once in each `TEST_P` to ensure proper state management.

- **`ENABLE_GENERATOR(TestClassName)`** - Instantiates all test combinations. Currently hardcoded for 2x2 combinations (4 total tests).

## How It Works

The library uses thread-local static variables to maintain generator state across test instantiations. When `GENERATOR` is called:

1. It registers the possible values for that generator
2. Based on the current test parameter index, it calculates which value to return
3. All combinations are automatically generated

## Example Output

```
[==========] Running 4 tests from 1 test suite.
[----------] 4 tests from Parameterized/MyTest
[ RUN      ] Parameterized/MyTest.SimpleCase/0
Test case: a=1, b=10
[       OK ] Parameterized/MyTest.SimpleCase/0 (0 ms)
[ RUN      ] Parameterized/MyTest.SimpleCase/1
Test case: a=1, b=20
[       OK ] Parameterized/MyTest.SimpleCase/1 (0 ms)
[ RUN      ] Parameterized/MyTest.SimpleCase/2
Test case: a=2, b=10
[       OK ] Parameterized/MyTest.SimpleCase/2 (0 ms)
[ RUN      ] Parameterized/MyTest.SimpleCase/3
Test case: a=2, b=20
[       OK ] Parameterized/MyTest.SimpleCase/3 (0 ms)
[==========] 4 tests from 1 test suite ran. (0 ms total)
[  PASSED  ] 4 tests.
```

## Limitations

- Currently, the total number of combinations is hardcoded in `ENABLE_GENERATOR` macro (set to 4 for 2x2 combinations)
- Only supports `int` values in generators
- Thread-local storage is used, so behavior in multi-threaded test environments may need consideration

## Future Improvements

- Dynamic calculation of total combinations
- Support for different data types in generators
- Named test instantiations
- Support for more complex value patterns

## License

This project is provided as-is for educational and development purposes.