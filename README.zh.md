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
    // ⚠️ 重要: GENERATOR()调用必须在顶部，在USE_GENERATOR()之前
    // 这个顺序是强制性的 - 库在执行测试之前需要计数生成器
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

### 在表达式中使用生成的值
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
}
```

### 复杂的测试逻辑
```cpp
TEST_G(MyTest, ConditionalLogic) {
    int mode = GENERATOR(0, 1, 2);     // 3种模式
    int value = GENERATOR(100, 200);   // 2个值
    USE_GENERATOR();

    int result;
    switch (mode) {
        case 0: result = value + 10; break;
        case 1: result = value * 2; break;
        case 2: result = value - 50; break;
    }

    EXPECT_GT(result, 0);
    printf("模式 %d 使用值 %d 得到结果 %d\n", mode, value, result);
}
```

## 高级示例

### 使用类和对象

#### 直接对象生成
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

#### 在构造函数参数中使用GENERATOR
```cpp
TEST_G(MyTest, ConstructorWithGenerators) {
    // GENERATOR值用作构造函数参数
    int val1 = GENERATOR(1, 2);
    int val2 = GENERATOR(10, 20);
    USE_GENERATOR();

    TestObject objects[] = {
        TestObject(val1, "test"),
        TestObject(val2, "demo")
    };

    EXPECT_LT(objects[0].value, objects[1].value);
    printf("数组对象: [0]={%d,%s}, [1]={%d,%s}\n",
           objects[0].value, objects[0].name.c_str(),
           objects[1].value, objects[1].name.c_str());
}
```

### 使用指针和动态内存

#### 生成对象指针
```cpp
TEST_G(MyTest, PointerGeneration) {
    // 生成指向不同对象的指针
    // 注意：小心内存管理
    auto* ptr1 = GENERATOR(new TestObject(1, "第一"),
                          new TestObject(2, "第二"));
    auto* ptr2 = GENERATOR(new TestObject(10, "十"),
                          new TestObject(20, "二十"));
    USE_GENERATOR();

    EXPECT_LT(*ptr1, *ptr2);
    printf("指针: ptr1={%d, %s}, ptr2={%d, %s}\n",
           ptr1->value, ptr1->name.c_str(),
           ptr2->value, ptr2->name.c_str());

    // 清理
    delete ptr1;
    delete ptr2;
}
```

#### 嵌套GENERATOR调用（高级）
```cpp
TEST_G(MyTest, NestedGenerators) {
    // 复杂的嵌套生成 - 每个外部GENERATOR包含内部GENERATOR调用
    int inner1 = GENERATOR(1, 2);
    int inner2 = GENERATOR(3, 4);
    int inner3 = GENERATOR(10, 20);
    int inner4 = GENERATOR(30, 40);
    USE_GENERATOR();

    auto* obj1 = new TestObject(inner1, "第一");
    auto* obj2 = new TestObject(inner3, "第二");

    EXPECT_LT(obj1->value, obj2->value);
    printf("嵌套: obj1={%d}, obj2={%d}\n", obj1->value, obj2->value);

    delete obj1;
    delete obj2;
}
```

### 使用STL容器

#### 生成容器大小和内容
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

    printf("向量: 大小=%d, 乘数=%d, 元素=[", size, multiplier);
    for (int v : vec) printf("%d ", v);
    printf("]\n");
}
```

#### 生成字符串组合
```cpp
TEST_G(MyTest, StringCombinations) {
    auto prefix_choice = GENERATOR(0, 1);
    auto suffix_choice = GENERATOR(0, 1);
    auto repeat = GENERATOR(1, 2);
    USE_GENERATOR();

    std::string prefix = prefix_choice ? "你好" : "嗨";
    std::string suffix = suffix_choice ? "世界" : "大家";

    std::string result;
    for (int i = 0; i < repeat; ++i) {
        if (i > 0) result += " ";
        result += prefix + " " + suffix;
    }

    EXPECT_FALSE(result.empty());
    printf("字符串: prefix='%s', suffix='%s', repeat=%d => '%s'\n",
           prefix.c_str(), suffix.c_str(), repeat, result.c_str());
}
```

### 使用智能指针

#### 使用unique_ptr和GENERATOR
```cpp
TEST_G(MyTest, SmartPointers) {
    auto value1 = GENERATOR(1, 2);
    auto value2 = GENERATOR(10, 20);
    USE_GENERATOR();

    auto ptr1 = std::make_unique<TestObject>(value1, "第一");
    auto ptr2 = std::make_unique<TestObject>(value2, "第二");

    EXPECT_LT(*ptr1, *ptr2);
    printf("智能指针: ptr1={%d, %s}, ptr2={%d, %s}\n",
           ptr1->value, ptr1->name.c_str(),
           ptr2->value, ptr2->name.c_str());
}
```

### 复杂结构体示例

#### 生成具有多个字段的结构体
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
    printf("点: p1=(%d,%d), p2=(%d,%d), 距离=%d\n",
           p1.x, p1.y, p2.x, p2.y, distance);
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

### ⚠️ 重要: GENERATOR和USE_GENERATOR顺序

**非常重要:** 所有`GENERATOR()`调用**必须**在测试用例的**顶部**，在`USE_GENERATOR()`**之前**。这个顺序对于库的正确工作是强制性的。

✅ **正确**:
```cpp
TEST_G(MyTest, Example) {
    // 步骤1: 所有GENERATOR()调用首先在顶部
    int a = GENERATOR(1, 2);
    int b = GENERATOR(10, 20);

    // 步骤2: 在所有生成器之后USE_GENERATOR()
    USE_GENERATOR();

    // 步骤3: 这里是测试逻辑
    EXPECT_LT(a, b);
}
```

❌ **错误 - 不会工作**:
```cpp
TEST_G(MyTest, Example) {
    USE_GENERATOR();             // ❌ 错误！必须在生成器之后
    int a = GENERATOR(1, 2);     // ❌ 太晚了！
    int b = GENERATOR(10, 20);
}
```

❌ **错误 - 不会工作**:
```cpp
TEST_G(MyTest, Example) {
    int a = GENERATOR(1, 2);
    USE_GENERATOR();             // ❌ 错误！必须在所有生成器之后
    int b = GENERATOR(10, 20);   // ❌ 这个生成器在USE_GENERATOR()之后
}
```

**为什么这很重要:**
- 库在预执行阶段计数生成器
- `USE_GENERATOR()`标记生成器声明阶段的结束
- 在`USE_GENERATOR()`之后声明的生成器将被忽略或导致错误

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

该库提供了一种简单、干净的方式,使用显式friend声明在测试中访问私有成员。

### 快速示例

```cpp
// 在您的生产类中
class MyClass {
private:
    int privateValue;
    std::string privateName;

    int computeSecret(int x) const { return privateValue * x; }

public:
    MyClass(int v, const std::string& n) : privateValue(v), privateName(n) {}

    // 授予特定测试的friend访问权限
    GTESTG_FRIEND_TEST(MyClassTest, AccessPrivateMembers);
    GTESTG_FRIEND_TEST(MyClassTest, ModifyPrivateMembers);
};

// 在您的测试文件中
struct MyClassTest : ::testing::Test {
    MyClass obj{42, "secret"};
};

TEST_F_FRIEND(MyClassTest, AccessPrivateMembers) {
    // 直接访问私有成员!
    EXPECT_EQ(obj.privateValue, 42);
    EXPECT_EQ(obj.privateName, "secret");
}

TEST_F_FRIEND(MyClassTest, ModifyPrivateMembers) {
    // 可以修改私有成员
    obj.privateValue = 100;
    EXPECT_EQ(obj.privateValue, 100);

    // 可以调用私有方法
    int result = obj.computeSecret(2);
    EXPECT_EQ(result, 200);
}
```

### 主要特性

- **简单干净**: 使用标准C++ friend声明
- **选择性访问**: 仅授予需要的特定测试访问权限
- **零开销**: 纯编译时机制,无运行时成本
- **类型安全**: 编译器保证的类型安全
- **生产安全**: friend声明不会影响运行时

### API参考

#### 目标类宏

| 宏 | 目的 | 用法 |
|-------|---------|-------|
| `GTESTG_FRIEND_TEST(Suite, TestName)` | 授予特定TEST_F测试的friend访问权限 | 放置在类定义中 |
| `GTESTG_FRIEND_TEST_G(TestClassName, TestName)` | 授予TEST_G测试的friend访问权限 | 放置在类定义中 |
| `GTESTG_FRIEND_TEST_SUITE(Suite)` | 授予套件中所有测试的friend访问权限 | 放置在类定义中 |

#### 测试文件宏

| 宏 | 目的 | 用法 |
|-------|---------|-------|
| `TEST_F_FRIEND(Suite, TestName)` | 定义具有friend访问权限的测试 | 与TEST_F相同 |
| `TEST_G_FRIEND(TestClassName, TestName)` | 定义具有friend访问权限的生成器测试 | 与TEST_G相同 |

### 使用示例

#### 基本私有访问
```cpp
// 目标类
class Widget {
private:
    int secret_ = 42;
public:
    // 授予特定测试的访问权限
    GTESTG_FRIEND_TEST(WidgetTest, CheckSecret);
};

// 测试文件
struct WidgetTest : ::testing::Test {
    Widget w;
};

TEST_F_FRIEND(WidgetTest, CheckSecret) {
    EXPECT_EQ(w.secret_, 42);  // 直接访问私有成员
}
```

#### 具有私有访问的生成器测试
```cpp
// 目标类
class Calculator {
private:
    int factor_ = 10;
    int multiply(int x) const { return x * factor_; }
public:
    // 授予生成器测试的访问权限
    GTESTG_FRIEND_TEST_G(CalcTest, TestMultiply);
};

// 测试文件
struct CalcTest : ::gtest_generator::TestWithGenerator {
    Calculator calc;
};

TEST_G_FRIEND(CalcTest, TestMultiply) {
    int input = GENERATOR(1, 2, 3);
    USE_GENERATOR();

    // 访问私有方法和成员
    int result = calc.multiply(input);
    EXPECT_EQ(result, input * calc.factor_);
}
```

#### 授予套件中所有测试的访问权限
```cpp
// 目标类 - 授予MyTestSuite中所有测试的访问权限
class MyClass {
private:
    int value_ = 100;
public:
    // 授予整个测试套件的访问权限
    GTESTG_FRIEND_TEST_SUITE(MyTestSuite);
};

// 测试文件 - 此套件中的所有测试都有访问权限
struct MyTestSuite : ::testing::Test {
    MyClass obj;
};

TEST_F(MyTestSuite, Test1) {
    EXPECT_EQ(obj.value_, 100);  // 有访问权限
}

TEST_F(MyTestSuite, Test2) {
    obj.value_ = 200;  // 也有访问权限
    EXPECT_EQ(obj.value_, 200);
}
```

#### 使用继承
```cpp
// 基类
class Base {
private:
    int base_secret_ = 10;
public:
    GTESTG_FRIEND_TEST(DerivedTest, AccessBoth);
};

// 派生类
class Derived : public Base {
private:
    int derived_secret_ = 20;
public:
    GTESTG_FRIEND_TEST(DerivedTest, AccessBoth);
};

// 测试
struct DerivedTest : ::testing::Test {
    Derived d;
};

TEST_F_FRIEND(DerivedTest, AccessBoth) {
    EXPECT_EQ(d.base_secret_, 10);     // 访问基类私有成员
    EXPECT_EQ(d.derived_secret_, 20);  // 访问派生类私有成员
}
```

### 重要说明

1. **需要显式授权**: 每个需要私有访问的测试必须在目标类中显式列出
2. **没有魔法**: 使用标准C++ friend声明 - 简单且可预测
3. **TEST_F_FRIEND是可选的**: `TEST_F_FRIEND`只是一个映射到`TEST_F`的便利宏。如果类有适当的`GTESTG_FRIEND_TEST`声明,您可以使用常规`TEST_F`
4. **编译时安全**: 如果测试尝试在未授予friend访问权限的情况下访问私有成员,您将收到编译错误
5. **维护**: 添加需要私有访问的新测试时,记得向目标类添加相应的`GTESTG_FRIEND_TEST`声明

### 何时使用此功能

在以下情况使用私有成员访问:
- 测试未通过公共接口公开的内部状态
- 验证复杂的私有逻辑
- 为测试设置特定的内部状态
- 测试无法轻松重构的遗留代码

在以下情况避免使用:
- 对私有访问的需求表明设计不佳
- 公共接口测试就足够了
- 会在测试和实现之间创建紧密耦合

有关完整示例,请参见`test_friend_access.cpp`。

### TEST_F_FRIEND和TEST_G_FRIEND宏

库提供了`TEST_F_FRIEND`和`TEST_G_FRIEND`宏，它们创建具有VirtualAccessor模式内置支持的测试基础设施。这些宏与`GTESTG_FRIEND_ACCESS_PRIVATE()`声明无缝协作。

**关键点:**
- `GTESTG_FRIEND_ACCESS_PRIVATE()`为基于类的(VirtualAccessor)和基于函数的(gtestg_private_accessMember)方法**都**授予friend访问权限
- 对于常规TEST_F风格的测试使用`TEST_F_FRIEND`
- 对于基于生成器的参数化测试使用`TEST_G_FRIEND`
- 继续使用`GTESTG_PRIVATE_MEMBER`宏访问私有成员

**TEST_F_FRIEND示例:**
```cpp
class Widget {
private:
    int secret_ = 42;
public:
    Widget() = default;

    // 单个宏授予两种类型的friend访问权限
    GTESTG_FRIEND_ACCESS_PRIVATE();
};

// 声明访问器
GTESTG_PRIVATE_DECLARE_MEMBER(Widget, secret_);

struct WidgetTest : ::testing::Test {
    Widget w;
};

TEST_F_FRIEND(WidgetTest, AccessPrivate) {
    // 使用基于函数的访问器访问私有成员
    int& secret = GTESTG_PRIVATE_MEMBER(Widget, secret_, &w);
    EXPECT_EQ(secret, 42);
    secret = 100;
    EXPECT_EQ(secret, 100);
}
```

**TEST_G_FRIEND示例:**
```cpp
struct WidgetGenTest : ::gtest_generator::TestWithGenerator {
    Widget w{999};
};

TEST_G_FRIEND(WidgetGenTest, GeneratorTest) {
    int factor = GENERATOR(1, 2, 5);
    USE_GENERATOR();

    int& secret = GTESTG_PRIVATE_MEMBER(Widget, secret_, &w);
    EXPECT_EQ(secret, 999);

    printf("factor=%d, secret=%d\n", factor, secret);
}
```

**多文件支持:**
`TEST_F_FRIEND`和`TEST_G_FRIEND`在多个.cpp文件中定义测试并链接到同一可执行文件时正确工作，就像常规`TEST_G`一样。有关示例，请参见`test_friend_multi_file1.cpp`和`test_friend_multi_file2.cpp`。

### 统一私有成员访问系统

该库提供了一个统一系统，用于访问测试中的私有和保护成员。通过向类中添加单个宏`GTESTG_FRIEND_ACCESS_PRIVATE()`，您可以启用私有成员访问的**两种互补方法**:

1. **通过TEST_F_FRIEND/TEST_G_FRIEND直接访问** - 推荐用于大多数情况
2. **通过GTESTG_PRIVATE_MEMBER宏的基于函数的访问** - 用于更明确的控制

这两种方法可以无缝协作并在同一测试中使用。

#### 核心: GTESTG_FRIEND_ACCESS_PRIVATE()

向类中添加此单个宏以启用私有成员访问:

```cpp
class MyClass {
private:
    int privateValue = 42;
    std::string privateName = "secret";
public:
    // 一个宏启用两种访问方法
    GTESTG_FRIEND_ACCESS_PRIVATE();
};
```

此宏授予以下friend访问权限:
- **VirtualAccessor模板** - 由TEST_F_FRIEND和TEST_G_FRIEND使用
- **gtestg_private_accessMember函数** - 由GTESTG_PRIVATE_MEMBER宏使用

#### 方法1: 使用TEST_F_FRIEND和TEST_G_FRIEND (推荐)

对于简单情况，使用`TEST_F_FRIEND`或`TEST_G_FRIEND`创建可以直接访问私有成员的测试:

**TEST_F_FRIEND示例:**
```cpp
struct WidgetTest : ::testing::Test {
    Widget w;
};

TEST_F_FRIEND(WidgetTest, AccessPrivate) {
    // 直接访问私有成员(通过VirtualAccessor特化)
    EXPECT_EQ(w.secret_, 42);
    w.secret_ = 100;
    EXPECT_EQ(w.secret_, 100);
}
```

**TEST_G_FRIEND示例:**
```cpp
struct WidgetGenTest : ::gtest_generator::TestWithGenerator {
    Widget w{999};
};

TEST_G_FRIEND(WidgetGenTest, GeneratorTest) {
    int factor = GENERATOR(1, 2, 5);
    USE_GENERATOR();

    // 参数化测试中也可以直接访问
    EXPECT_EQ(w.secret_, 999);
    printf("factor=%d, secret=%d\n", factor, w.secret_);
}
```

**多文件支持:**
`TEST_F_FRIEND`和`TEST_G_FRIEND`在测试跨多个.cpp文件定义并链接到同一可执行文件时正确工作。有关示例，请参见`test_friend_multi_file1.cpp`和`test_friend_multi_file2.cpp`。

**工作原理:**
- `TEST_F_FRIEND`和`TEST_G_FRIEND`在`gtestg_detail`命名空间内创建`VirtualAccessor<Suite, TestName>`的显式模板特化
- 此特化通过`GTESTG_FRIEND_ACCESS_PRIVATE()`授予friend访问权限
- 因为`VirtualAccessor`是friend并继承自您的测试fixture，它可以访问目标类的私有成员
- 测试体在此friend类的上下文中执行，从而启用直接私有成员访问
- 每个测试获得唯一的标签类型以创建单独的特化，避免命名冲突

#### 方法2: 使用GTESTG_PRIVATE_MEMBER宏 (显式控制)

对于需要更多控制或使用常规`TEST_F`/`TEST_G`宏时，使用基于函数的访问器宏。此方法需要为要访问的每个成员声明访问权限。

**步骤1: 在类外部声明访问(在测试文件中):**
```cpp
// 声明要访问的成员
GTESTG_PRIVATE_DECLARE_MEMBER(Widget, secret_);
GTESTG_PRIVATE_DECLARE_MEMBER(Widget, privateName);
```

**步骤2: 在测试中访问成员:**
```cpp
TEST_F_FRIEND(WidgetTest, AccessPrivate) {
    // 使用宏访问
    int& secret = GTESTG_PRIVATE_MEMBER(Widget, secret_, &w);
    EXPECT_EQ(secret, 42);
    secret = 100;
    EXPECT_EQ(secret, 100);
}
```

此方法适用于:
- 您希望明确记录正在访问哪些成员
- 您需要访问静态成员
- 您需要具有附加逻辑的自定义访问器函数

#### 结合两种方法

您可以在同一测试中使用两种方法:

```cpp
class Widget {
private:
    int secret_ = 42;
    static int counter_;
public:
    GTESTG_FRIEND_ACCESS_PRIVATE();  // 启用两种方法
};

int Widget::counter_ = 0;

// 为静态成员声明访问
GTESTG_PRIVATE_DECLARE_STATIC(Widget, counter_);

TEST_G_FRIEND(WidgetTest, CombinedAccess) {
    int value = GENERATOR(10, 20);
    USE_GENERATOR();

    Widget w;

    // 方法1: 直接访问实例成员
    w.secret_ = value;
    EXPECT_EQ(w.secret_, value);

    // 方法2: 使用宏访问静态成员
    int& count = GTESTG_PRIVATE_STATIC(Widget, counter_);
    count++;
}
```

### GTESTG_PRIVATE_MEMBER宏的完整API参考

本节提供基于函数的私有成员访问宏(方法2)的详细参考。

#### 声明成员访问

将这些声明放在类**外部**，通常在测试文件中。这些声明告诉系统您想要访问哪些私有成员:

| 宏 | 用途 | 参数 | 示例 |
|-------|---------|------------|---------|
| `GTESTG_PRIVATE_DECLARE_MEMBER` | 声明访问实例成员 | Target, MemberName | `GTESTG_PRIVATE_DECLARE_MEMBER(MyClass, privateField)` |
| `GTESTG_PRIVATE_DECLARE_STATIC` | 声明访问静态成员 | Target, MemberName | `GTESTG_PRIVATE_DECLARE_STATIC(MyClass, staticCounter)` |
| `GTESTG_PRIVATE_DECLARE_FUNCTION` | 声明自定义访问器函数 | ThisClass, Target, FuncName | `GTESTG_PRIVATE_DECLARE_FUNCTION(MyTest, MyClass, GetSum)` |

#### 访问成员的宏

在测试函数**内部**使用这些宏访问私有成员:

| 宏 | 用途 | 参数 | 示例 |
|-------|---------|------------|---------|
| `GTESTG_PRIVATE_MEMBER` | 访问实例成员 | Target, MemberName, &obj | `GTESTG_PRIVATE_MEMBER(MyClass, privateField, &obj)` |
| `GTESTG_PRIVATE_STATIC` | 访问静态成员 | Target, MemberName | `GTESTG_PRIVATE_STATIC(MyClass, staticCounter)` |
| `GTESTG_PRIVATE_CALL` | 使用显式测试对象调用自定义函数 | Target, FuncName, test_obj, &obj | `GTESTG_PRIVATE_CALL(MyClass, GetSum, this, &obj)` |
| `GTESTG_PRIVATE_CALL_ON_TEST` | 调用自定义函数(使用隐式'this') | ThisClass, Target, FuncName, &obj | `GTESTG_PRIVATE_CALL_ON_TEST(MyTest, MyClass, GetSum, &obj)` |

#### 详细使用示例

以下示例演示了GTESTG_PRIVATE_*宏的综合使用模式。

**示例1: 访问实例成员**
```cpp
class MyClass {
private:
    int privateValue = 42;
    std::string privateName = "secret";
public:
    GTESTG_FRIEND_ACCESS_PRIVATE();
};

// 声明访问(在测试文件中)
GTESTG_PRIVATE_DECLARE_MEMBER(MyClass, privateValue);
GTESTG_PRIVATE_DECLARE_MEMBER(MyClass, privateName);

TEST_G(MyTest, AccessPrivate) {
    int value = GENERATOR(10, 20, 30);
    USE_GENERATOR();

    MyClass obj;

    // 访问和修改私有成员
    int& val = GTESTG_PRIVATE_MEMBER(MyClass, privateValue, &obj);
    EXPECT_EQ(val, 42);
    val = value;  // 可以修改
    EXPECT_EQ(val, value);

    std::string& name = GTESTG_PRIVATE_MEMBER(MyClass, privateName, &obj);
    EXPECT_EQ(name, "secret");
    name = "modified";
}
```

**示例2: 访问静态成员**
```cpp
class MyClass {
private:
    static int staticCounter;
public:
    GTESTG_FRIEND_ACCESS_PRIVATE();
};

int MyClass::staticCounter = 100;

// 声明访问静态成员
GTESTG_PRIVATE_DECLARE_STATIC(MyClass, staticCounter);

TEST_G(MyTest, AccessStatic) {
    USE_GENERATOR();

    // 访问静态成员(不需要对象)
    int& count = GTESTG_PRIVATE_STATIC(MyClass, staticCounter);
    EXPECT_EQ(count, 100);
    count = 200;  // 可以修改
    EXPECT_EQ(count, 200);
}
```

**示例3: 自定义访问器函数**
```cpp
class MyClass {
private:
    int field1 = 10;
    int field2 = 20;
public:
    GTESTG_FRIEND_ACCESS_PRIVATE();
};

class MyTest : public ::gtest_generator::TestWithGenerator {};

// 声明具有测试上下文和私有成员访问权限的自定义函数
// THIS = 测试对象, TARGET = 被访问的对象
GTESTG_PRIVATE_DECLARE_FUNCTION(MyTest, MyClass, GetSum) {
    // 可以访问测试上下文: THIS->GetParam()
    // 可以访问私有成员: TARGET->field1, TARGET->field2
    return TARGET->field1 + TARGET->field2;
}

TEST_G(MyTest, CustomFunction) {
    int multiplier = GENERATOR(1, 2, 3);
    USE_GENERATOR();

    MyClass obj;

    // 使用CALL_ON_TEST调用自定义函数(使用隐式'this')
    int sum = GTESTG_PRIVATE_CALL_ON_TEST(MyTest, MyClass, GetSum, &obj);
    EXPECT_EQ(sum, 30);  // 10 + 20

    // 替代方法: 显式传递测试对象
    int sum2 = GTESTG_PRIVATE_CALL(MyClass, GetSum, static_cast<MyTest*>(this), &obj);
    EXPECT_EQ(sum2, 30);
}
```

有关完整示例，请参见`test_private_access.cpp`和`test_define_macros.cpp`。

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
