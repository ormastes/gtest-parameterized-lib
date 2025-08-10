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

## API参考

### 宏

- **`TEST_G(TestClassName, TestName)`** - 定义带有自动测试生成的参数化测试。替换TEST_P和单独的ENABLE_GENERATOR调用的需要。

- **`GENERATOR(...)`** - 定义测试参数的值。每次调用都会创建测试组合的新维度。
  ```cpp
  int value = GENERATOR(1, 2, 3);  // 创建3个变体
  ```
  **重要**: 所有GENERATOR()调用必须在USE_GENERATOR()之前

- **`USE_GENERATOR()`** - 必须在每个TEST_G中调用一次，在所有GENERATOR()调用之后。

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

## 许可证

此项目按原样提供，用于教育和开发目的。