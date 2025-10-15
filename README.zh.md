# GTest Generator - 参数化测试库

一个简单的仅头文件库，为Google Test参数化测试提供类似Catch2风格的`GENERATE`语法。

## 主要特性

- 类似Catch2的`GENERATE`的简单直观语法
- 仅头文件实现
- 自动生成所有测试组合
- 与标准Google Test `TEST_P`宏兼容

## 快速开始

```cpp
#include "gtest_generator.h"

// 使用TestWithGenerator定义测试夹具
class MyTest : public ::gtest_generator::TestWithGenerator {
};

// 使用TEST_G宏编写参数化测试
TEST_G(MyTest, SimpleCase) {
    // 重要：所有GENERATOR()调用必须在USE_GENERATOR()之前
    int a = GENERATOR(1, 2);      // 将是1或2
    int b = GENERATOR(10, 20);    // 将是10或20
    USE_GENERATOR();              // 必须在所有GENERATOR()调用之后调用

    EXPECT_LT(a, b);
    printf("测试: a=%d, b=%d\n", a, b);
}
// TEST_G自动处理测试生成 - 不需要ENABLE_GENERATOR！
```

## 安装

这是一个仅头文件库。只需将`gtest_generator.h`复制到您的项目并包含它。

## 构建项目

### 先决条件

- CMake 3.14或更高版本
- Google Test库
- C++14兼容编译器

### 构建说明

```bash
# 创建构建目录
mkdir build && cd build

# 使用CMake配置
cmake ..

# 构建项目
cmake --build .

# 运行测试
./mygen_test
```

## 更多示例

### 多个值

```cpp
TEST_G(MyTest, MultipleValues) {
    int x = GENERATOR(1, 2, 3);       // 3个值
    int y = GENERATOR(10, 20, 30);    // 3个值
    USE_GENERATOR();
    
    // 生成9个测试组合
    EXPECT_LT(x, y);
}
```

### 不同数量的值

```cpp
TEST_G(MyTest, AsymmetricCombinations) {
    int size = GENERATOR(1, 2, 3);     // 3个值
    int scale = GENERATOR(10, 100);    // 2个值
    USE_GENERATOR();
    
    // 生成6个测试组合
    int result = size * scale;
    EXPECT_GT(result, 0);
}
```

## 高级示例

### 使用类和对象

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
    // 直接生成完整对象
    auto obj1 = GENERATOR(TestObject(1, "第一"), TestObject(2, "第二"));
    auto obj2 = GENERATOR(TestObject(10, "十"), TestObject(20, "二十"));
    USE_GENERATOR();
    
    EXPECT_LT(obj1, obj2);
    printf("对象: obj1={%d, %s}, obj2={%d, %s}\n", 
           obj1.value, obj1.name.c_str(), 
           obj2.value, obj2.name.c_str());
}
```

### 使用STL容器

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
    printf("向量: 大小=%d, 乘数=%d\n", size, multiplier);
}
```

## 采样模式

该库支持两种采样模式来生成测试组合：

### FULL模式（默认 - 笛卡尔乘积）
默认模式生成所有可能的值组合（笛卡尔乘积）。这是保证完整测试覆盖的传统行为。

```cpp
TEST_G(MyTest, FullMode) {
    USE_GENERATOR();  // 默认是FULL模式
    // 或显式地： USE_GENERATOR(FULL);
    
    auto x = GENERATOR(1, 2);      // 2个值
    auto y = GENERATOR(10, 20);    // 2个值
    auto z = GENERATOR(100, 200);  // 2个值
    
    // 生成 8 次测试运行： 2 × 2 × 2 = 8 个组合
    // (1,10,100), (1,10,200), (1,20,100), (1,20,200),
    // (2,10,100), (2,10,200), (2,20,100), (2,20,200)
}
```

### ALIGNED模式（并行迭代）
ALIGNED模式像拉链一样并行遍历所有列。每列在每次运行时都会前进到下一个值，达到末尾时会回到开头。总运行次数等于最大列的大小。

```cpp
TEST_G(MyTest, AlignedMode) {
    USE_GENERATOR(ALIGNED);  // 启用ALIGNED模式
    
    auto x = GENERATOR(1, 2);           // 2个值
    auto y = GENERATOR(10, 20, 30, 40); // 4个值  
    auto z = GENERATOR(100, 200, 300);  // 3个值
    
    // 生成 4 次测试运行（最大列大小）：
    // 运行 0: (1, 10, 100)  - 全部在索引 0
    // 运行 1: (2, 20, 200)  - 全部在索引 1
    // 运行 2: (1, 30, 300)  - x 回到 0，其他在索引 2
    // 运行 3: (2, 40, 100)  - x 在 1，y 在 3，z 回到 0
}
```

#### ALIGNED模式的关键特征：
- **确定性**：值按顺序选择（0, 1, 2, ...）并循环
- **声明顺序**：列按声明顺序处理
- **更少运行**：总运行次数 = 最大列大小（不是乘积）
- **平衡覆盖**：每列中的每个值都被近似平等使用

#### 实现细节：
库使用Google Test的`GTEST_SKIP()`机制高效实现ALIGNED模式：
1. 在测试注册期间，库统计每个GENERATOR列的大小
2. 在运行时，库计算最大列大小
3. 超出最大大小的测试运行使用`GTEST_SKIP()`跳过
4. 每列到达末尾时循环回到开头（使用模运算符）

这种方法确保：
- 只执行必要的测试运行（而不是所有笛卡尔乘积）
- Google Test报告的测试数量反映实际执行的测试
- 没有因生成不必要组合而产生的性能损失

#### 何时使用各种模式：
- **FULL模式**：当您需要所有组合的穷尽测试时
- **ALIGNED模式**：当您希望用更少的测试运行进行代表性采样时

#### 比较示例：
```cpp
// FULL模式： 3 × 2 × 2 = 12 次运行
TEST_G(MyTest, FullExample) {
    USE_GENERATOR(FULL);
    auto a = GENERATOR(1, 2, 3);
    auto b = GENERATOR(10, 20);
    auto c = GENERATOR(100, 200);
    // 生成所有 12 个组合
}

// ALIGNED模式： max(3, 2, 2) = 3 次运行
TEST_G(MyTest, AlignedExample) {
    USE_GENERATOR(ALIGNED);
    auto a = GENERATOR(1, 2, 3);
    auto b = GENERATOR(10, 20);
    auto c = GENERATOR(100, 200);
    // 只生成 3 个组合：
    // (1, 10, 100), (2, 20, 200), (3, 10, 100)
}
```

#### 验证测试示例：
项目包含`test_mode_counts.cpp`，全面验证两种模式的正确工作：

```cpp
// 使用3x2x2配置进行测试
TEST_G(ModeCountTest, Full_3x2x2) {
    int a = GENERATOR(1, 2, 3);        // 3个值
    int b = GENERATOR(10, 20);         // 2个值
    int c = GENERATOR(100, 200);       // 2个值
    USE_GENERATOR(FULL);

    // 验证：生成恰好12个测试用例 (3 × 2 × 2)
    // 所有组合都是唯一的
}

TEST_G(ModeCountTest, Aligned_3x2x2) {
    int a = GENERATOR(1, 2, 3);        // 3个值
    int b = GENERATOR(10, 20);         // 2个值
    int c = GENERATOR(100, 200);       // 2个值
    USE_GENERATOR(ALIGNED);

    // 验证：生成恰好3个测试用例（最大列大小）
    // 结果：(1,10,100), (2,20,200), (3,10,100)
}
```

运行验证测试以查看详细输出：
```bash
./build/test_mode_counts
```

预期输出显示：
- FULL模式 3x2x2：12次运行（所有唯一组合）
- ALIGNED模式 3x2x2：3次运行（最大列大小）
- FULL模式 2x3x4：24次运行（所有唯一组合）
- ALIGNED模式 2x3x4：4次运行（最大列大小）
- FULL模式 5x1x3：15次运行（所有唯一组合）
- ALIGNED模式 5x1x3：5次运行（最大列大小）

## API参考

### 宏

- **`TEST_G(TestClassName, TestName)`** - 定义带有自动测试生成的参数化测试。替换TEST_P和单独的ENABLE_GENERATOR调用的需要。

- **`GENERATOR(...)`** - 定义测试参数的值。每次调用都会创建测试组合的新维度。
  ```cpp
  int value = GENERATOR(1, 2, 3);  // 创建3个变体
  ```
  **重要**: 所有GENERATOR()调用必须在USE_GENERATOR()之前

- **`USE_GENERATOR()`** - 必须在每个TEST_G中调用一次，在所有GENERATOR()调用之后。默认使用FULL模式。

- **`USE_GENERATOR(mode)`** - 必须在每个TEST_G中调用一次，在所有GENERATOR()调用之后。指定采样模式：
  - `USE_GENERATOR(FULL)` - 所有值的笛卡尔乘积（与默认相同）
  - `USE_GENERATOR(ALIGNED)` - 通过列的并行迭代

## 工作原理

库使用线程局部静态变量来维护测试实例化之间的生成器状态。当调用`GENERATOR`时：

1. 它注册该生成器的可能值
2. 基于当前测试参数索引，计算要返回的值
3. 自动生成所有组合

## 重要使用说明

### 正确的顺序至关重要

✅ **正确**:
```cpp
TEST_G(MyTest, Example) {
    int a = GENERATOR(1, 2);     // 首先：定义生成器
    int b = GENERATOR(10, 20);   // 
    USE_GENERATOR();             // 然后：调用USE_GENERATOR()
    // 这里是测试逻辑
}
```

❌ **错误**:
```cpp
TEST_G(MyTest, Example) {
    USE_GENERATOR();             // 错误！这必须在GENERATOR调用之后
    int a = GENERATOR(1, 2);     
    int b = GENERATOR(10, 20);   
    // 这里是测试逻辑
}
```

## 示例输出

```
[==========] 从1个测试套件运行4个测试。
[----------] 来自Generator/MyTest__SimpleCase的4个测试
[ RUN      ] Generator/MyTest__SimpleCase.__/0
测试: a=1, b=10
[       OK ] Generator/MyTest__SimpleCase.__/0 (0 ms)
[ RUN      ] Generator/MyTest__SimpleCase.__/1
测试: a=2, b=10
[       OK ] Generator/MyTest__SimpleCase.__/1 (0 ms)
[ RUN      ] Generator/MyTest__SimpleCase.__/2
测试: a=1, b=20
[       OK ] Generator/MyTest__SimpleCase.__/2 (0 ms)
[ RUN      ] Generator/MyTest__SimpleCase.__/3
测试: a=2, b=20
[       OK ] Generator/MyTest__SimpleCase.__/3 (0 ms)
[==========] 从1个测试套件运行了4个测试。(总计0 ms)
[  通过  ] 4个测试。
```

## 限制

- 测试夹具必须继承自`::gtest_generator::TestWithGenerator`
- 必须使用`TEST_G`宏而不是标准的`TEST_P`
- 所有`GENERATOR()`调用必须在`USE_GENERATOR()`之前
- 复杂类型（对象、指针）可以与GENERATOR一起使用，但可能需要适当的模板实例化
- 使用`new`生成指针时，内存管理是用户的责任

## 测试中的私有成员访问

该库提供了一种类型安全的方式来访问测试中的私有成员，而无需使用`#define private public`或修改生产代码。

### 快速示例

```cpp
// 在您的生产类中
class MyClass {
private:
    int privateValue;
    std::string privateName;
public:
    MyClass(int v, const std::string& n) : privateValue(v), privateName(n) {}

    // 授予测试的友元访问权限
    GTESTG_FRIEND_ACCESS_PRIVATE();
};

// 在您的测试文件中


// 声明访问器 - 只传递字段名称
GTESTG_PRIVATE_DECLARE_MEMBER(MyClass, privateValue);
GTESTG_PRIVATE_DECLARE_MEMBER(MyClass, privateName);

TEST_G(MyTest, AccessPrivate) {
    int value = GENERATOR(10, 20);
    USE_GENERATOR();

    MyClass obj(value, "test");

    // 访问和修改私有成员
    int& privateRef = GTESTG_PRIVATE_MEMBER(MyClass, privateValue, &obj);
    EXPECT_EQ(privateRef, value);
    privateRef = 100;
    EXPECT_EQ(privateRef, 100);
}
```

### 主要特性

- **类型安全**：使用模板特化和友元声明
- **零开销**：完全的编译时机制
- **生产安全**：在生产构建中，`GTESTG_FRIEND_ACCESS_PRIVATE()`可以定义为空宏
- **可共享**：声明块可以复制到通用头文件中
- **命名空间化**：所有宏和函数都使用`GTESTG_`前缀以防止命名冲突
- **简单的API**：参数最少，语法简洁

### API参考

#### 声明访问

| 宏 | 用途 | 参数 | 示例 |
|-------|---------|------------|---------|
| `GTESTG_PRIVATE_DECLARE_MEMBER` | 访问实例成员 | Target, MemberName | `GTESTG_PRIVATE_DECLARE_MEMBER(MyClass, privateField)` |
| `GTESTG_PRIVATE_DECLARE_STATIC` | 访问静态成员 | Target, MemberName | `GTESTG_PRIVATE_DECLARE_STATIC(MyClass, staticCounter)` |
| `GTESTG_PRIVATE_DECLARE_FUNCTION` | 自定义访问器函数 | ThisClass, Target, FuncName | `GTESTG_PRIVATE_DECLARE_FUNCTION(MyTest, MyClass, GetSum)` |

#### 访问成员

| 宏 | 用途 | 参数 | 示例 |
|-------|---------|------------|---------|
| `GTESTG_PRIVATE_MEMBER` | 访问实例成员 | Target, MemberName, &obj | `GTESTG_PRIVATE_MEMBER(MyClass, privateField, &obj)` |
| `GTESTG_PRIVATE_STATIC` | 访问静态成员 | Target, MemberName | `GTESTG_PRIVATE_STATIC(MyClass, staticCounter)` |
| `GTESTG_PRIVATE_CALL` | 使用显式测试对象调用自定义函数 | Target, FuncName, test_obj, &obj | `GTESTG_PRIVATE_CALL(MyClass, GetSum, *this, &obj)` |
| `GTESTG_PRIVATE_CALL_ON_TEST` | 调用自定义函数（使用隐式'this'） | ThisClass, Target, FuncName, &obj | `GTESTG_PRIVATE_CALL_ON_TEST(MyTest, MyClass, GetSum, &obj)` |

### 使用示例

**实例成员：**
```cpp
// 声明
GTESTG_PRIVATE_DECLARE_MEMBER(MyClass, privateField);

// 在测试中访问
int& value = GTESTG_PRIVATE_MEMBER(MyClass, privateField, &obj);
value = 42;  // 可以修改
```

**静态成员：**
```cpp
// 声明
GTESTG_PRIVATE_DECLARE_STATIC(MyClass, staticCounter);

// 在测试中访问
int& count = GTESTG_PRIVATE_STATIC(MyClass, staticCounter);
count++;  // 可以修改
```

**自定义函数：**
```cpp
// 使用自定义逻辑声明
// THIS 提供测试上下文，TARGET 是被访问的对象
GTESTG_PRIVATE_DECLARE_FUNCTION(MyTest, MyClass, GetSum) {
    // 如果需要可以访问测试参数：THIS->GetParam()
    // 访问目标对象：TARGET->field1, TARGET->field2
    return TARGET->field1 + TARGET->field2;
}

// 在 TEST_G(MyTest, ...) 中调用
// 选项1：使用 CALL_ON_TEST 使用隐式 'this'
int sum1 = GTESTG_PRIVATE_CALL_ON_TEST(MyTest, MyClass, GetSum, &obj);

// 选项2：使用 CALL 显式传递测试对象
int sum2 = GTESTG_PRIVATE_CALL(MyClass, GetSum, *this, &obj);
```

**自定义函数中的参数名：**
- `THIS` - 指向测试夹具实例的指针（提供像`GetParam()`这样的测试上下文）
- `TARGET` - 指向您正在访问其私有成员的对象的指针

**实现说明：**
- 库使用带有友元声明的模板特化来实现类型安全访问
- ALIGNED模式中的列索引跟踪在测试参数之间自动重置（在最新版本中已修复）
- 所有宏都使用`GTESTG_`前缀以避免命名冲突

有关完整示例，请参见`test_private_access.cpp`和`example_common_header.h`。

## 数组比较宏

该库提供了方便的宏，用于逐元素比较数组并提供详细的错误消息。这些宏构建在Google Test的断言宏之上。

### 快速示例

```cpp
TEST_G(ArrayTest, CompareArrays) {
    USE_GENERATOR();

    int expected[] = {1, 2, 3, 4, 5};
    int actual[] = {1, 2, 3, 4, 5};

    EXPECT_ARRAY_EQ(expected, actual, 5);  // Non-fatal assertion
}
```

### 可用的宏

#### 整数和通用类型

- **`EXPECT_ARRAY_EQ(expected, actual, size)`** - 非致命：逐元素比较两个数组
  ```cpp
  int expected[] = {1, 2, 3};
  int actual[] = {1, 2, 3};
  EXPECT_ARRAY_EQ(expected, actual, 3);
  ```

- **`ASSERT_ARRAY_EQ(expected, actual, size)`** - 致命：逐元素比较两个数组
  ```cpp
  std::vector<int> expected = {10, 20, 30};
  std::vector<int> actual = {10, 20, 30};
  ASSERT_ARRAY_EQ(expected.data(), actual.data(), 3);  // Test stops if fails
  ```

#### 浮点类型

- **`EXPECT_ARRAY_NEAR(expected, actual, size, abs_error)`** - 非致命：使用容差比较浮点数组
  ```cpp
  double expected[] = {1.0, 2.0, 3.0};
  double actual[] = {1.001, 1.999, 3.002};
  EXPECT_ARRAY_NEAR(expected, actual, 3, 0.01);  // Tolerance: 0.01
  ```

- **`ASSERT_ARRAY_NEAR(expected, actual, size, abs_error)`** - 致命：使用容差比较浮点数组
  ```cpp
  float expected[] = {1.5f, 2.5f, 3.5f};
  float actual[] = {1.501f, 2.499f, 3.502f};
  ASSERT_ARRAY_NEAR(expected, actual, 3, 0.01f);
  ```

- **`EXPECT_ARRAY_DOUBLE_EQ(expected, actual, size)`** - 非致命：使用默认容差比较double数组
  ```cpp
  double expected[] = {1.5, 2.5, 3.5};
  double actual[] = {1.5, 2.5, 3.5};
  EXPECT_ARRAY_DOUBLE_EQ(expected, actual, 3);
  ```

- **`EXPECT_ARRAY_FLOAT_EQ(expected, actual, size)`** - 非致命：使用默认容差比较float数组
  ```cpp
  float expected[] = {1.25f, 2.25f, 3.25f};
  float actual[] = {1.25f, 2.25f, 3.25f};
  EXPECT_ARRAY_FLOAT_EQ(expected, actual, 3);
  ```

### 错误消息

当数组不同时，宏会提供详细的错误消息：

```cpp
int expected[] = {1, 2, 3, 4, 5};
int actual[] = {1, 2, 99, 4, 5};

EXPECT_ARRAY_EQ(expected, actual, 5);
// Output:
// Expected equality of these values:
//   (expected)[i]
//     Which is: 3
//   (actual)[i]
//     Which is: 99
// Arrays differ at index 2
```

### 使用不同的容器类型

```cpp
TEST_G(ArrayTest, DifferentContainers) {
    int size = GENERATOR(3, 5, 7);
    USE_GENERATOR();

    // C-style arrays
    int arr1[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int arr2[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    EXPECT_ARRAY_EQ(arr1, arr2, size);

    // std::vector
    std::vector<int> vec1(size);
    std::vector<int> vec2(size);
    for (int i = 0; i < size; ++i) {
        vec1[i] = i * 10;
        vec2[i] = i * 10;
    }
    EXPECT_ARRAY_EQ(vec1.data(), vec2.data(), size);

    // std::array
    std::array<int, 5> arr3 = {1, 2, 3, 4, 5};
    std::array<int, 5> arr4 = {1, 2, 3, 4, 5};
    EXPECT_ARRAY_EQ(arr3.data(), arr4.data(), std::min(size, 5));
}
```

### 与GENERATOR结合使用

```cpp
TEST_G(ArrayTest, ParameterizedArrayTest) {
    int size = GENERATOR(3, 5, 10);
    int multiplier = GENERATOR(1, 10, 100);
    USE_GENERATOR();

    std::vector<int> expected(size);
    std::vector<int> actual(size);

    for (int i = 0; i < size; ++i) {
        expected[i] = i * multiplier;
        actual[i] = i * multiplier;
    }

    EXPECT_ARRAY_EQ(expected.data(), actual.data(), size);
}
```

### 主要特性

- **逐元素比较**：每个元素都单独比较
- **详细的错误消息**：显示哪个索引不同以及对应的值
- **适用于任何可比较类型**：int、float、double、string、具有operator==的自定义类型
- **成功消息**：当所有元素匹配时显示"Arrays are equal"
- **兼容向量和数组**：适用于C风格数组、std::vector、std::array

### 重要说明

1. **必须提供大小参数**：您必须明确提供数组大小
2. **致命 vs 非致命**：致命断言使用ASSERT_*，非致命使用EXPECT_*
3. **浮点比较**：浮点值使用NEAR、FLOAT_EQ或DOUBLE_EQ
4. **自定义类型**：使用EXPECT_ARRAY_EQ需要类型定义operator==
5. **零大小数组**：空数组（大小 = 0）可正常工作

有关完整示例，请参见`test_array_compare.cpp`。

## 未来改进

- 动态计算总组合数
- 支持生成器中的不同数据类型
- 命名测试实例化
- 支持更复杂的值模式

## 许可证

本项目以教育和开发目的提供。
