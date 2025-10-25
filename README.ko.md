# GTest Generator - 매개변수화된 테스트 라이브러리

Google Test 매개변수화된 테스트를 위한 Catch2 스타일의 `GENERATE` 구문을 제공하는 간단한 헤더 전용 라이브러리입니다.

## 주요 기능

- Catch2의 `GENERATE`와 유사한 간단하고 직관적인 구문
- 헤더 전용 구현
- 모든 테스트 조합 자동 생성
- 표준 Google Test `TEST_P` 매크로와 호환

## 빠른 시작

```cpp
#include "gtest_generator.h"

// TestWithGenerator를 사용하여 테스트 픽스처 정의
class MyTest : public ::gtest_generator::TestWithGenerator {
};

// TEST_G 매크로를 사용하여 매개변수화된 테스트 작성
TEST_G(MyTest, SimpleCase) {
    // ⚠️ 중요: GENERATOR() 호출은 반드시 맨 위에, USE_GENERATOR() 이전에 와야 합니다
    // 이 순서는 필수입니다 - 라이브러리가 테스트를 실행하기 전에 생성기를 계산합니다
    int a = GENERATOR(1, 2);      // 1 또는 2가 됩니다
    int b = GENERATOR(10, 20);    // 10 또는 20이 됩니다
    USE_GENERATOR();              // 반드시 모든 GENERATOR() 호출 후에 와야 합니다

    EXPECT_LT(a, b);
    printf("테스트: a=%d, b=%d\n", a, b);
}
// TEST_G가 자동으로 테스트 생성을 처리합니다 - ENABLE_GENERATOR가 필요 없습니다!
```

## 설치

이것은 헤더 전용 라이브러리입니다. 단순히 `gtest_generator.h`를 프로젝트에 복사하고 포함시키면 됩니다.

## 프로젝트 빌드

### 필수 조건

- CMake 3.14 이상
- Google Test 라이브러리
- C++14 호환 컴파일러

### 빌드 방법

```bash
# 빌드 디렉토리 생성
mkdir build && cd build

# CMake로 구성
cmake ..

# 프로젝트 빌드
cmake --build .

# 테스트 실행
./mygen_test
```

## 더 많은 예제

### 여러 값 사용

```cpp
TEST_G(MyTest, MultipleValues) {
    int x = GENERATOR(1, 2, 3);       // 3개 값
    int y = GENERATOR(10, 20, 30);    // 3개 값
    USE_GENERATOR();
    
    // 9개의 테스트 조합 생성
    EXPECT_LT(x, y);
}
```

### 다른 개수의 값

```cpp
TEST_G(MyTest, AsymmetricCombinations) {
    int size = GENERATOR(1, 2, 3);     // 3개 값
    int scale = GENERATOR(10, 100);    // 2개 값
    USE_GENERATOR();

    // 6개의 테스트 조합 생성
    int result = size * scale;
    EXPECT_GT(result, 0);
}
```

### 표현식에서 생성된 값 사용
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

### 복잡한 테스트 로직
```cpp
TEST_G(MyTest, ConditionalLogic) {
    int mode = GENERATOR(0, 1, 2);     // 3가지 모드
    int value = GENERATOR(100, 200);   // 2개 값
    USE_GENERATOR();

    int result;
    switch (mode) {
        case 0: result = value + 10; break;
        case 1: result = value * 2; break;
        case 2: result = value - 50; break;
    }

    EXPECT_GT(result, 0);
    printf("모드 %d에 값 %d를 적용한 결과 %d\n", mode, value, result);
}
```

## 고급 예제

### 클래스와 객체 작업

#### 직접 객체 생성
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
    // 완전한 객체를 직접 생성
    auto obj1 = GENERATOR(TestObject(1, "첫번째"), TestObject(2, "두번째"));
    auto obj2 = GENERATOR(TestObject(10, "열"), TestObject(20, "스물"));
    USE_GENERATOR();

    EXPECT_LT(obj1, obj2);
    printf("객체: obj1={%d, %s}, obj2={%d, %s}\n",
           obj1.value, obj1.name.c_str(),
           obj2.value, obj2.name.c_str());
}
```

#### 생성자 인수에서 GENERATOR 사용
```cpp
TEST_G(MyTest, ConstructorWithGenerators) {
    // 생성자 인수로 사용되는 GENERATOR 값
    int val1 = GENERATOR(1, 2);
    int val2 = GENERATOR(10, 20);
    USE_GENERATOR();

    TestObject objects[] = {
        TestObject(val1, "test"),
        TestObject(val2, "demo")
    };

    EXPECT_LT(objects[0].value, objects[1].value);
    printf("배열 객체: [0]={%d,%s}, [1]={%d,%s}\n",
           objects[0].value, objects[0].name.c_str(),
           objects[1].value, objects[1].name.c_str());
}
```

### 포인터와 동적 메모리 작업

#### 객체에 대한 포인터 생성
```cpp
TEST_G(MyTest, PointerGeneration) {
    // 다른 객체에 대한 포인터 생성
    // 참고: 메모리 관리에 주의하세요
    auto* ptr1 = GENERATOR(new TestObject(1, "첫번째"),
                          new TestObject(2, "두번째"));
    auto* ptr2 = GENERATOR(new TestObject(10, "열"),
                          new TestObject(20, "스물"));
    USE_GENERATOR();

    EXPECT_LT(*ptr1, *ptr2);
    printf("포인터: ptr1={%d, %s}, ptr2={%d, %s}\n",
           ptr1->value, ptr1->name.c_str(),
           ptr2->value, ptr2->name.c_str());

    // 정리
    delete ptr1;
    delete ptr2;
}
```

#### 중첩 GENERATOR 호출 (고급)
```cpp
TEST_G(MyTest, NestedGenerators) {
    // 복잡한 중첩 생성 - 각 외부 GENERATOR에 내부 GENERATOR 호출이 포함됨
    int inner1 = GENERATOR(1, 2);
    int inner2 = GENERATOR(3, 4);
    int inner3 = GENERATOR(10, 20);
    int inner4 = GENERATOR(30, 40);
    USE_GENERATOR();

    auto* obj1 = new TestObject(inner1, "첫번째");
    auto* obj2 = new TestObject(inner3, "두번째");

    EXPECT_LT(obj1->value, obj2->value);
    printf("중첩: obj1={%d}, obj2={%d}\n", obj1->value, obj2->value);

    delete obj1;
    delete obj2;
}
```

### STL 컨테이너 작업

#### 컨테이너 크기 및 내용 생성
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

    printf("벡터: 크기=%d, 배수=%d, 요소=[", size, multiplier);
    for (int v : vec) printf("%d ", v);
    printf("]\n");
}
```

#### 문자열 조합 생성
```cpp
TEST_G(MyTest, StringCombinations) {
    auto prefix_choice = GENERATOR(0, 1);
    auto suffix_choice = GENERATOR(0, 1);
    auto repeat = GENERATOR(1, 2);
    USE_GENERATOR();

    std::string prefix = prefix_choice ? "안녕" : "하이";
    std::string suffix = suffix_choice ? "세계" : "여러분";

    std::string result;
    for (int i = 0; i < repeat; ++i) {
        if (i > 0) result += " ";
        result += prefix + " " + suffix;
    }

    EXPECT_FALSE(result.empty());
    printf("문자열: prefix='%s', suffix='%s', repeat=%d => '%s'\n",
           prefix.c_str(), suffix.c_str(), repeat, result.c_str());
}
```

### 스마트 포인터 작업

#### unique_ptr과 GENERATOR 사용
```cpp
TEST_G(MyTest, SmartPointers) {
    auto value1 = GENERATOR(1, 2);
    auto value2 = GENERATOR(10, 20);
    USE_GENERATOR();

    auto ptr1 = std::make_unique<TestObject>(value1, "첫번째");
    auto ptr2 = std::make_unique<TestObject>(value2, "두번째");

    EXPECT_LT(*ptr1, *ptr2);
    printf("스마트 포인터: ptr1={%d, %s}, ptr2={%d, %s}\n",
           ptr1->value, ptr1->name.c_str(),
           ptr2->value, ptr2->name.c_str());
}
```

### 복잡한 구조체 예제

#### 여러 필드가 있는 구조체 생성
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
    printf("점: p1=(%d,%d), p2=(%d,%d), 거리=%d\n",
           p1.x, p1.y, p2.x, p2.y, distance);
}
```

## 샘플링 모드

라이브러리는 테스트 조합 생성을 위한 두 가지 샘플링 모드를 지원합니다:

### FULL 모드 (기본 - 데카르트 곱)
기본 모드는 모든 가능한 값의 조합(데카르트 곱)을 생성합니다. 이는 완전한 테스트 커버리지를 보장하는 전통적인 방식입니다.

```cpp
TEST_G(MyTest, FullMode) {
    USE_GENERATOR();  // 기본값은 FULL 모드
    // 또는 명시적으로: USE_GENERATOR(FULL);
    
    auto x = GENERATOR(1, 2);      // 2개 값
    auto y = GENERATOR(10, 20);    // 2개 값
    auto z = GENERATOR(100, 200);  // 2개 값
    
    // 8번의 테스트 실행 생성: 2 × 2 × 2 = 8개 조합
    // (1,10,100), (1,10,200), (1,20,100), (1,20,200),
    // (2,10,100), (2,10,200), (2,20,100), (2,20,200)
}
```

### ALIGNED 모드 (병렬 반복)
ALIGNED 모드는 지퍼처럼 모든 열을 병렬로 반복합니다. 각 열은 각 실행마다 다음 값으로 진행하며, 끝에 도달하면 처음부터 다시 시작합니다. 총 실행 횟수는 가장 큰 열의 크기와 같습니다.

```cpp
TEST_G(MyTest, AlignedMode) {
    USE_GENERATOR(ALIGNED);  // ALIGNED 모드 활성화
    
    auto x = GENERATOR(1, 2);           // 2개 값
    auto y = GENERATOR(10, 20, 30, 40); // 4개 값  
    auto z = GENERATOR(100, 200, 300);  // 3개 값
    
    // 4번의 테스트 실행 생성 (최대 열 크기):
    // 실행 0: (1, 10, 100)  - 모든 인덱스 0
    // 실행 1: (2, 20, 200)  - 모든 인덱스 1
    // 실행 2: (1, 30, 300)  - x는 0으로 순환, 나머지는 인덱스 2
    // 실행 3: (2, 40, 100)  - x는 1, y는 3, z는 0으로 순환
}
```

#### ALIGNED 모드의 주요 특성:
- **결정적**: 값은 순서대로(0, 1, 2, ...) 선택되며 순환함
- **선언 순서**: 열은 선언된 순서대로 처리됨
- **더 적은 실행**: 총 실행 = 최대 열 크기 (곱이 아님)
- **균형 잡힌 커버리지**: 각 열의 각 값이 거의 동일하게 사용됨

#### 구현 세부사항:
라이브러리는 Google Test의 `GTEST_SKIP()` 메커니즘을 사용하여 ALIGNED 모드를 효율적으로 구현합니다:
1. 테스트 등록 중에 라이브러리가 각 GENERATOR 열의 크기를 계산합니다
2. 런타임에 라이브러리가 최대 열 크기를 계산합니다
3. 최대 크기를 초과하는 테스트 실행은 `GTEST_SKIP()`를 사용하여 건너뜁니다
4. 각 열은 끝에 도달하면 처음부터 다시 시작합니다 (모듈로 연산자 사용)

이 접근 방식은 다음을 보장합니다:
- 필요한 테스트 실행만 수행됩니다 (모든 데카르트 곱이 아님)
- Google Test가 보고하는 테스트 수는 실제로 실행된 테스트를 반영합니다
- 불필요한 조합 생성으로 인한 성능 저하가 없습니다

#### 각 모드를 언제 사용할지:
- **FULL 모드**: 모든 조합의 철저한 테스트가 필요할 때
- **ALIGNED 모드**: 더 적은 테스트 실행으로 대표적인 샘플링을 원할 때

#### 예시 비교:
```cpp
// FULL 모드: 3 × 2 × 2 = 12번 실행
TEST_G(MyTest, FullExample) {
    USE_GENERATOR(FULL);
    auto a = GENERATOR(1, 2, 3);
    auto b = GENERATOR(10, 20);
    auto c = GENERATOR(100, 200);
    // 12개 조합 모두 생성
}

// ALIGNED 모드: max(3, 2, 2) = 3번 실행
TEST_G(MyTest, AlignedExample) {
    USE_GENERATOR(ALIGNED);
    auto a = GENERATOR(1, 2, 3);
    auto b = GENERATOR(10, 20);
    auto c = GENERATOR(100, 200);
    // 3개 조합만 생성:
    // (1, 10, 100), (2, 20, 200), (3, 10, 100)
}
```

#### 검증 테스트 예제:
프로젝트에는 두 모드가 올바르게 작동하는지 포괄적으로 검증하는 `test_mode_counts.cpp`가 포함되어 있습니다:

```cpp
// 3x2x2 구성으로 테스트
TEST_G(ModeCountTest, Full_3x2x2) {
    int a = GENERATOR(1, 2, 3);        // 3개 값
    int b = GENERATOR(10, 20);         // 2개 값
    int c = GENERATOR(100, 200);       // 2개 값
    USE_GENERATOR(FULL);

    // 검증: 정확히 12개 테스트 케이스 생성 (3 × 2 × 2)
    // 모든 조합이 고유함
}

TEST_G(ModeCountTest, Aligned_3x2x2) {
    int a = GENERATOR(1, 2, 3);        // 3개 값
    int b = GENERATOR(10, 20);         // 2개 값
    int c = GENERATOR(100, 200);       // 2개 값
    USE_GENERATOR(ALIGNED);

    // 검증: 정확히 3개 테스트 케이스 생성 (최대 열 크기)
    // 결과: (1,10,100), (2,20,200), (3,10,100)
}
```

검증 테스트를 실행하여 자세한 출력을 확인하세요:
```bash
./build/test_mode_counts
```

예상 출력은 다음을 보여줍니다:
- FULL 모드 3x2x2: 12번 실행 (모든 고유한 조합)
- ALIGNED 모드 3x2x2: 3번 실행 (최대 열 크기)
- FULL 모드 2x3x4: 24번 실행 (모든 고유한 조합)
- ALIGNED 모드 2x3x4: 4번 실행 (최대 열 크기)
- FULL 모드 5x1x3: 15번 실행 (모든 고유한 조합)
- ALIGNED 모드 5x1x3: 5번 실행 (최대 열 크기)

## API 레퍼런스

### 매크로

- **`TEST_G(TestClassName, TestName)`** - 자동 테스트 생성을 포함한 매개변수화된 테스트를 정의합니다. TEST_P와 별도의 ENABLE_GENERATOR 호출 필요성을 모두 대체합니다.

- **`GENERATOR(...)`** - 테스트 매개변수의 값을 정의합니다. 각 호출은 테스트 조합의 새로운 차원을 생성합니다.
  ```cpp
  int value = GENERATOR(1, 2, 3);  // 3개 변형 생성
  ```
  **중요**: 모든 GENERATOR() 호출은 USE_GENERATOR() 이전에 와야 합니다

- **`USE_GENERATOR()`** - 각 TEST_G에서 한 번 호출되어야 하며, 모든 GENERATOR() 호출 이후에 와야 합니다. 기본적으로 FULL 모드를 사용합니다.

- **`USE_GENERATOR(mode)`** - 각 TEST_G에서 한 번 호출되어야 하며, 모든 GENERATOR() 호출 이후에 와야 합니다. 샘플링 모드를 지정합니다:
  - `USE_GENERATOR(FULL)` - 모든 값의 데카르트 곱 (기본값과 동일)
  - `USE_GENERATOR(ALIGNED)` - 열을 통한 병렬 반복

## 작동 원리

라이브러리는 테스트 인스턴스화 간에 생성기 상태를 유지하기 위해 thread-local static 변수를 사용합니다. `GENERATOR`가 호출될 때:

1. 해당 생성기의 가능한 값들을 등록합니다
2. 현재 테스트 매개변수 인덱스를 기반으로 반환할 값을 계산합니다
3. 모든 조합이 자동으로 생성됩니다

## 중요 사용 참고사항

### ⚠️ 중요: GENERATOR와 USE_GENERATOR 순서

**매우 중요:** 모든 `GENERATOR()` 호출은 **반드시** 테스트 케이스의 **맨 위**에서 `USE_GENERATOR()` **이전**에 와야 합니다. 이 순서는 라이브러리가 올바르게 작동하기 위해 필수입니다.

✅ **올바른 예**:
```cpp
TEST_G(MyTest, Example) {
    // 1단계: 맨 위에 모든 GENERATOR() 호출을 먼저
    int a = GENERATOR(1, 2);
    int b = GENERATOR(10, 20);

    // 2단계: 모든 생성기 후에 USE_GENERATOR()
    USE_GENERATOR();

    // 3단계: 여기에 테스트 로직
    EXPECT_LT(a, b);
}
```

❌ **잘못된 예 - 작동하지 않습니다**:
```cpp
TEST_G(MyTest, Example) {
    USE_GENERATOR();             // ❌ 잘못됨! 생성기 이후에 와야 합니다
    int a = GENERATOR(1, 2);     // ❌ 너무 늦었습니다!
    int b = GENERATOR(10, 20);
}
```

❌ **잘못된 예 - 작동하지 않습니다**:
```cpp
TEST_G(MyTest, Example) {
    int a = GENERATOR(1, 2);
    USE_GENERATOR();             // ❌ 잘못됨! 모든 생성기 이후에 와야 합니다
    int b = GENERATOR(10, 20);   // ❌ 이 생성기는 USE_GENERATOR() 이후입니다
}
```

**이것이 중요한 이유:**
- 라이브러리는 사전 실행 단계에서 생성기를 계산합니다
- `USE_GENERATOR()`는 생성기 선언 단계의 끝을 표시합니다
- `USE_GENERATOR()` 이후에 선언된 생성기는 무시되거나 오류를 발생시킵니다

## 예제 출력

```
[==========] 4개 테스트 실행 중 (1개 테스트 스위트).
[----------] Generator/MyTest__SimpleCase의 4개 테스트
[ RUN      ] Generator/MyTest__SimpleCase.__/0
테스트: a=1, b=10
[       OK ] Generator/MyTest__SimpleCase.__/0 (0 ms)
[ RUN      ] Generator/MyTest__SimpleCase.__/1
테스트: a=2, b=10
[       OK ] Generator/MyTest__SimpleCase.__/1 (0 ms)
[ RUN      ] Generator/MyTest__SimpleCase.__/2
테스트: a=1, b=20
[       OK ] Generator/MyTest__SimpleCase.__/2 (0 ms)
[ RUN      ] Generator/MyTest__SimpleCase.__/3
테스트: a=2, b=20
[       OK ] Generator/MyTest__SimpleCase.__/3 (0 ms)
[==========] 1개 테스트 스위트의 4개 테스트 실행 완료. (0 ms 총 소요)
[  통과  ] 4개 테스트.
```

## 제한사항

- 테스트 픽스처는 `::gtest_generator::TestWithGenerator`를 상속해야 합니다
- 표준 `TEST_P` 대신 `TEST_G` 매크로를 사용해야 합니다
- 모든 `GENERATOR()` 호출은 `USE_GENERATOR()` 이전에 와야 합니다
- 복잡한 타입(객체, 포인터)은 GENERATOR와 작동하지만 적절한 템플릿 인스턴스화가 필요할 수 있습니다
- `new`로 포인터를 생성할 때 메모리 관리는 사용자의 책임입니다

## 테스트를 위한 Private 멤버 접근

라이브러리는 명시적인 friend 선언을 사용하여 테스트에서 private 멤버에 접근하는 간단하고 깔끔한 방법을 제공합니다.

### 빠른 예제

```cpp
// 프로덕션 클래스에서
class MyClass {
private:
    int privateValue;
    std::string privateName;

    int computeSecret(int x) const { return privateValue * x; }

public:
    MyClass(int v, const std::string& n) : privateValue(v), privateName(n) {}

    // 특정 테스트에 friend 접근 권한 부여
    GTESTG_FRIEND_TEST(MyClassTest, AccessPrivateMembers);
    GTESTG_FRIEND_TEST(MyClassTest, ModifyPrivateMembers);
};

// 테스트 파일에서
struct MyClassTest : ::testing::Test {
    MyClass obj{42, "secret"};
};

TEST_FRIEND(MyClassTest, AccessPrivateMembers) {
    // private 멤버에 직접 접근!
    EXPECT_EQ(obj.privateValue, 42);
    EXPECT_EQ(obj.privateName, "secret");
}

TEST_FRIEND(MyClassTest, ModifyPrivateMembers) {
    // private 멤버 수정 가능
    obj.privateValue = 100;
    EXPECT_EQ(obj.privateValue, 100);

    // private 메서드 호출 가능
    int result = obj.computeSecret(2);
    EXPECT_EQ(result, 200);
}
```

### 주요 기능

- **간단하고 깔끔함**: 표준 C++ friend 선언 사용
- **선택적 접근**: 필요한 특정 테스트에만 접근 권한 부여
- **제로 오버헤드**: 순수 컴파일 타임 메커니즘, 런타임 비용 없음
- **타입 안전**: 컴파일러가 보장하는 타입 안전성
- **프로덕션 안전**: friend 선언은 런타임에 영향 없음

### API 레퍼런스

#### 대상 클래스용 매크로

| 매크로 | 목적 | 사용법 |
|-------|---------|-------|
| `GTESTG_FRIEND_TEST(Suite, TestName)` | 특정 TEST_F 테스트에 friend 접근 권한 부여 | 클래스 정의 내에 배치 |
| `GTESTG_FRIEND_TEST_G(TestClassName, TestName)` | TEST_G 테스트에 friend 접근 권한 부여 | 클래스 정의 내에 배치 |
| `GTESTG_FRIEND_TEST_SUITE(Suite)` | 스위트의 모든 테스트에 friend 접근 권한 부여 | 클래스 정의 내에 배치 |

#### 테스트 파일용 매크로

| 매크로 | 목적 | 사용법 |
|-------|---------|-------|
| `TEST_FRIEND(Suite, TestName)` | friend 접근 권한이 있는 테스트 정의 | TEST_F와 동일 |
| `TEST_G_FRIEND(TestClassName, TestName)` | friend 접근 권한이 있는 생성기 테스트 정의 | TEST_G와 동일 |

### 사용 예제

#### 기본 Private 접근
```cpp
// 대상 클래스
class Widget {
private:
    int secret_ = 42;
public:
    // 특정 테스트에 접근 권한 부여
    GTESTG_FRIEND_TEST(WidgetTest, CheckSecret);
};

// 테스트 파일
struct WidgetTest : ::testing::Test {
    Widget w;
};

TEST_FRIEND(WidgetTest, CheckSecret) {
    EXPECT_EQ(w.secret_, 42);  // private 멤버에 직접 접근
}
```

#### Private 접근이 있는 생성기 테스트
```cpp
// 대상 클래스
class Calculator {
private:
    int factor_ = 10;
    int multiply(int x) const { return x * factor_; }
public:
    // 생성기 테스트에 접근 권한 부여
    GTESTG_FRIEND_TEST_G(CalcTest, TestMultiply);
};

// 테스트 파일
struct CalcTest : ::gtest_generator::TestWithGenerator {
    Calculator calc;
};

TEST_G_FRIEND(CalcTest, TestMultiply) {
    int input = GENERATOR(1, 2, 3);
    USE_GENERATOR();

    // private 메서드와 멤버에 접근
    int result = calc.multiply(input);
    EXPECT_EQ(result, input * calc.factor_);
}
```

#### 스위트의 모든 테스트에 접근 권한 부여
```cpp
// 대상 클래스 - MyTestSuite의 모든 테스트에 접근 권한 부여
class MyClass {
private:
    int value_ = 100;
public:
    // 전체 테스트 스위트에 접근 권한 부여
    GTESTG_FRIEND_TEST_SUITE(MyTestSuite);
};

// 테스트 파일 - 이 스위트의 모든 테스트가 접근 가능
struct MyTestSuite : ::testing::Test {
    MyClass obj;
};

TEST_F(MyTestSuite, Test1) {
    EXPECT_EQ(obj.value_, 100);  // 접근 가능
}

TEST_F(MyTestSuite, Test2) {
    obj.value_ = 200;  // 역시 접근 가능
    EXPECT_EQ(obj.value_, 200);
}
```

#### 상속과 함께 작업
```cpp
// 기본 클래스
class Base {
private:
    int base_secret_ = 10;
public:
    GTESTG_FRIEND_TEST(DerivedTest, AccessBoth);
};

// 파생 클래스
class Derived : public Base {
private:
    int derived_secret_ = 20;
public:
    GTESTG_FRIEND_TEST(DerivedTest, AccessBoth);
};

// 테스트
struct DerivedTest : ::testing::Test {
    Derived d;
};

TEST_FRIEND(DerivedTest, AccessBoth) {
    EXPECT_EQ(d.base_secret_, 10);     // 기본 private에 접근
    EXPECT_EQ(d.derived_secret_, 20);  // 파생 private에 접근
}
```

### 중요 참고사항

1. **명시적 권한 부여 필요**: private 접근이 필요한 각 테스트는 대상 클래스에 명시적으로 나열되어야 합니다
2. **마법 없음**: 표준 C++ friend 선언 사용 - 간단하고 예측 가능
3. **TEST_FRIEND는 선택사항**: `TEST_FRIEND`는 `TEST_F`로 매핑되는 편의 매크로입니다. 클래스에 적절한 `GTESTG_FRIEND_TEST` 선언이 있는 경우 일반 `TEST_F`를 사용할 수 있습니다
4. **컴파일 타임 안전성**: friend 접근 권한이 부여되지 않은 상태에서 테스트가 private 멤버에 접근하려고 하면 컴파일 오류가 발생합니다
5. **유지보수**: private 접근이 필요한 새 테스트를 추가할 때 대상 클래스에 해당 `GTESTG_FRIEND_TEST` 선언을 추가하는 것을 잊지 마세요

### 이 기능을 사용할 때

다음과 같은 경우 private 멤버 접근을 사용하세요:
- 공개 인터페이스를 통해 노출되지 않는 내부 상태 테스트
- 복잡한 private 로직 검증
- 테스트를 위한 특정 내부 상태 설정
- 쉽게 리팩터링할 수 없는 레거시 코드 테스트

다음과 같은 경우 사용을 피하세요:
- private 접근의 필요성이 잘못된 설계를 나타내는 경우
- 공개 인터페이스 테스트로 충분한 경우
- 테스트와 구현 간에 밀접한 결합을 생성하는 경우

전체 예제는 `test_friend_access.cpp`를 참조하세요.

### 통합 Private 멤버 액세스 시스템

라이브러리는 테스트에서 private 및 protected 멤버에 액세스하기 위한 통합 시스템을 제공합니다. 클래스에 단일 매크로 `GTESTG_FRIEND_ACCESS_PRIVATE()`를 추가하면 private 멤버 액세스를 위한 **두 가지 상호 보완적인 접근 방식**이 활성화됩니다:

1. **TEST_FRIEND/TEST_G_FRIEND를 통한 직접 액세스** - 대부분의 경우 권장
2. **GTESTG_PRIVATE_MEMBER 매크로를 통한 함수 기반 액세스** - 더 명시적인 제어를 위해

두 접근 방식은 원활하게 함께 작동하며 동일한 테스트에서 사용할 수 있습니다.

#### 핵심: GTESTG_FRIEND_ACCESS_PRIVATE()

클래스에 이 단일 매크로를 추가하여 private 멤버 액세스를 활성화합니다:

```cpp
class MyClass {
private:
    int privateValue = 42;
    std::string privateName = "secret";
public:
    // 하나의 매크로로 두 가지 액세스 접근 방식을 활성화
    GTESTG_FRIEND_ACCESS_PRIVATE();
};
```

이 매크로는 다음에 대한 friend 액세스를 부여합니다:
- **VirtualAccessor 템플릿** - TEST_FRIEND 및 TEST_G_FRIEND에서 사용
- **gtestg_private_accessMember 함수** - GTESTG_PRIVATE_MEMBER 매크로에서 사용

#### 접근 방식 1: TEST_FRIEND 및 TEST_G_FRIEND 사용 (권장)

간단한 경우 `TEST_FRIEND` 또는 `TEST_G_FRIEND`를 사용하여 private 멤버에 직접 액세스할 수 있는 테스트를 생성합니다:

**TEST_FRIEND 예제:**
```cpp
struct WidgetTest : ::testing::Test {
    Widget w;
};

TEST_FRIEND(WidgetTest, AccessPrivate) {
    // private 멤버에 직접 액세스 (VirtualAccessor 상속을 통해)
    EXPECT_EQ(w.secret_, 42);
    w.secret_ = 100;
    EXPECT_EQ(w.secret_, 100);
}
```

**TEST_G_FRIEND 예제:**
```cpp
struct WidgetGenTest : ::gtest_generator::TestWithGenerator {
    Widget w{999};
};

TEST_G_FRIEND(WidgetGenTest, GeneratorTest) {
    int factor = GENERATOR(1, 2, 5);
    USE_GENERATOR();

    // 파라미터화된 테스트에서도 직접 액세스 가능
    EXPECT_EQ(w.secret_, 999);
    printf("factor=%d, secret=%d\n", factor, w.secret_);
}
```

**다중 파일 지원:**
`TEST_FRIEND` 및 `TEST_G_FRIEND`는 동일한 실행 파일에 링크된 여러 .cpp 파일에서 테스트가 정의된 경우에도 올바르게 작동합니다. 예제는 `test_friend_multi_file1.cpp` 및 `test_friend_multi_file2.cpp`를 참조하세요.

#### 접근 방식 2: GTESTG_PRIVATE_MEMBER 매크로 사용 (명시적 제어)

더 많은 제어가 필요하거나 일반 `TEST_F`/`TEST_G` 매크로를 사용할 때는 함수 기반 액세서 매크로를 사용합니다. 이 접근 방식은 액세스하려는 각 멤버에 대한 선언이 필요합니다.

**1단계: 클래스 외부에 액세스 선언 (테스트 파일에서):**
```cpp
// 액세스하려는 멤버 선언
GTESTG_PRIVATE_DECLARE_MEMBER(Widget, secret_);
GTESTG_PRIVATE_DECLARE_MEMBER(Widget, privateName);
```

**2단계: 테스트에서 멤버에 액세스:**
```cpp
TEST_FRIEND(WidgetTest, AccessPrivate) {
    // 매크로를 사용하여 액세스
    int& secret = GTESTG_PRIVATE_MEMBER(Widget, secret_, &w);
    EXPECT_EQ(secret, 42);
    secret = 100;
    EXPECT_EQ(secret, 100);
}
```

이 접근 방식은 다음과 같은 경우에 유용합니다:
- 액세스되는 멤버에 대한 명시적 문서화가 필요한 경우
- 정적 멤버에 액세스해야 하는 경우
- 추가 로직이 있는 사용자 정의 액세서 함수가 필요한 경우

#### 두 접근 방식 결합

동일한 테스트에서 두 접근 방식을 모두 사용할 수 있습니다:

```cpp
class Widget {
private:
    int secret_ = 42;
    static int counter_;
public:
    GTESTG_FRIEND_ACCESS_PRIVATE();  // 두 접근 방식 모두 활성화
};

int Widget::counter_ = 0;

// 정적 멤버에 대한 액세스 선언
GTESTG_PRIVATE_DECLARE_STATIC(Widget, counter_);

TEST_G_FRIEND(WidgetTest, CombinedAccess) {
    int value = GENERATOR(10, 20);
    USE_GENERATOR();

    Widget w;

    // 접근 방식 1: 인스턴스 멤버에 직접 액세스
    w.secret_ = value;
    EXPECT_EQ(w.secret_, value);

    // 접근 방식 2: 정적 멤버에 매크로 사용
    int& count = GTESTG_PRIVATE_STATIC(Widget, counter_);
    count++;
}
```

### GTESTG_PRIVATE_MEMBER 매크로 완전 API 참조

이 섹션은 함수 기반 private 멤버 액세스 매크로(접근 방식 2)에 대한 상세한 참조를 제공합니다.

#### 멤버에 대한 액세스 선언

클래스 **외부**에, 일반적으로 테스트 파일에 이러한 선언을 배치합니다. 이 선언은 액세스하려는 private 멤버를 시스템에 알려줍니다:

| 매크로 | 용도 | 매개변수 | 예제 |
|-------|------|---------|------|
| `GTESTG_PRIVATE_DECLARE_MEMBER` | 인스턴스 멤버 액세스 선언 | Target, MemberName | `GTESTG_PRIVATE_DECLARE_MEMBER(MyClass, privateField)` |
| `GTESTG_PRIVATE_DECLARE_STATIC` | 정적 멤버 액세스 선언 | Target, MemberName | `GTESTG_PRIVATE_DECLARE_STATIC(MyClass, staticCounter)` |
| `GTESTG_PRIVATE_DECLARE_FUNCTION` | 사용자 정의 액세서 함수 선언 | ThisClass, Target, FuncName | `GTESTG_PRIVATE_DECLARE_FUNCTION(MyTest, MyClass, GetSum)` |

#### 멤버 액세스 매크로

테스트 함수 **내부**에서 이러한 매크로를 사용하여 private 멤버에 액세스합니다:

| 매크로 | 용도 | 매개변수 | 예제 |
|-------|------|---------|------|
| `GTESTG_PRIVATE_MEMBER` | 인스턴스 멤버 액세스 | Target, MemberName, &obj | `GTESTG_PRIVATE_MEMBER(MyClass, privateField, &obj)` |
| `GTESTG_PRIVATE_STATIC` | 정적 멤버 액세스 | Target, MemberName | `GTESTG_PRIVATE_STATIC(MyClass, staticCounter)` |
| `GTESTG_PRIVATE_CALL` | 명시적 테스트 객체로 사용자 정의 함수 호출 | Target, FuncName, test_obj, &obj | `GTESTG_PRIVATE_CALL(MyClass, GetSum, this, &obj)` |
| `GTESTG_PRIVATE_CALL_ON_TEST` | 사용자 정의 함수 호출 (암시적 'this' 사용) | ThisClass, Target, FuncName, &obj | `GTESTG_PRIVATE_CALL_ON_TEST(MyTest, MyClass, GetSum, &obj)` |

#### 상세 사용 예제

다음 예제는 GTESTG_PRIVATE_* 매크로의 포괄적인 사용 패턴을 보여줍니다.

**예제 1: 인스턴스 멤버 액세스**
```cpp
class MyClass {
private:
    int privateValue = 42;
    std::string privateName = "secret";
public:
    GTESTG_FRIEND_ACCESS_PRIVATE();
};

// 액세스 선언 (테스트 파일에서)
GTESTG_PRIVATE_DECLARE_MEMBER(MyClass, privateValue);
GTESTG_PRIVATE_DECLARE_MEMBER(MyClass, privateName);

TEST_G(MyTest, AccessPrivate) {
    int value = GENERATOR(10, 20, 30);
    USE_GENERATOR();

    MyClass obj;

    // private 멤버 액세스 및 수정
    int& val = GTESTG_PRIVATE_MEMBER(MyClass, privateValue, &obj);
    EXPECT_EQ(val, 42);
    val = value;  // 수정 가능
    EXPECT_EQ(val, value);

    std::string& name = GTESTG_PRIVATE_MEMBER(MyClass, privateName, &obj);
    EXPECT_EQ(name, "secret");
    name = "modified";
}
```

**예제 2: 정적 멤버 액세스**
```cpp
class MyClass {
private:
    static int staticCounter;
public:
    GTESTG_FRIEND_ACCESS_PRIVATE();
};

int MyClass::staticCounter = 100;

// 정적 멤버 액세스 선언
GTESTG_PRIVATE_DECLARE_STATIC(MyClass, staticCounter);

TEST_G(MyTest, AccessStatic) {
    USE_GENERATOR();

    // 정적 멤버 액세스 (객체 필요 없음)
    int& count = GTESTG_PRIVATE_STATIC(MyClass, staticCounter);
    EXPECT_EQ(count, 100);
    count = 200;  // 수정 가능
    EXPECT_EQ(count, 200);
}
```

**예제 3: 사용자 정의 액세서 함수**
```cpp
class MyClass {
private:
    int field1 = 10;
    int field2 = 20;
public:
    GTESTG_FRIEND_ACCESS_PRIVATE();
};

class MyTest : public ::gtest_generator::TestWithGenerator {};

// 테스트 컨텍스트와 private 멤버 모두에 액세스하는 사용자 정의 함수 선언
// THIS = 테스트 객체, TARGET = 액세스되는 객체
GTESTG_PRIVATE_DECLARE_FUNCTION(MyTest, MyClass, GetSum) {
    // 테스트 컨텍스트 액세스: THIS->GetParam()
    // private 멤버 액세스: TARGET->field1, TARGET->field2
    return TARGET->field1 + TARGET->field2;
}

TEST_G(MyTest, CustomFunction) {
    int multiplier = GENERATOR(1, 2, 3);
    USE_GENERATOR();

    MyClass obj;

    // CALL_ON_TEST를 사용한 사용자 정의 함수 호출 (암시적 'this' 사용)
    int sum = GTESTG_PRIVATE_CALL_ON_TEST(MyTest, MyClass, GetSum, &obj);
    EXPECT_EQ(sum, 30);  // 10 + 20

    // 대안: 테스트 객체를 명시적으로 전달
    int sum2 = GTESTG_PRIVATE_CALL(MyClass, GetSum, static_cast<MyTest*>(this), &obj);
    EXPECT_EQ(sum2, 30);
}
```

전체 예제는 `test_private_access.cpp` 및 `test_define_macros.cpp`를 참조하세요.

## 배열 비교 매크로

라이브러리는 상세한 오류 메시지와 함께 배열을 요소별로 비교할 수 있는 편리한 매크로를 제공합니다. 이러한 매크로는 Google Test의 assertion 매크로를 기반으로 구축되었습니다.

### 빠른 예제

```cpp
TEST_G(ArrayTest, CompareArrays) {
    USE_GENERATOR();

    int expected[] = {1, 2, 3, 4, 5};
    int actual[] = {1, 2, 3, 4, 5};

    EXPECT_ARRAY_EQ(expected, actual, 5);  // Non-fatal assertion
}
```

### 사용 가능한 매크로

#### 정수 및 범용 타입

- **`EXPECT_ARRAY_EQ(expected, actual, size)`** - Non-fatal: 두 배열을 요소별로 비교
  ```cpp
  int expected[] = {1, 2, 3};
  int actual[] = {1, 2, 3};
  EXPECT_ARRAY_EQ(expected, actual, 3);
  ```

- **`ASSERT_ARRAY_EQ(expected, actual, size)`** - Fatal: 두 배열을 요소별로 비교
  ```cpp
  std::vector<int> expected = {10, 20, 30};
  std::vector<int> actual = {10, 20, 30};
  ASSERT_ARRAY_EQ(expected.data(), actual.data(), 3);  // 실패 시 테스트 중단
  ```

#### 부동소수점 타입

- **`EXPECT_ARRAY_NEAR(expected, actual, size, abs_error)`** - Non-fatal: 허용 오차로 부동소수점 배열 비교
  ```cpp
  double expected[] = {1.0, 2.0, 3.0};
  double actual[] = {1.001, 1.999, 3.002};
  EXPECT_ARRAY_NEAR(expected, actual, 3, 0.01);  // 허용 오차: 0.01
  ```

- **`ASSERT_ARRAY_NEAR(expected, actual, size, abs_error)`** - Fatal: 허용 오차로 부동소수점 배열 비교
  ```cpp
  float expected[] = {1.5f, 2.5f, 3.5f};
  float actual[] = {1.501f, 2.499f, 3.502f};
  ASSERT_ARRAY_NEAR(expected, actual, 3, 0.01f);
  ```

- **`EXPECT_ARRAY_DOUBLE_EQ(expected, actual, size)`** - Non-fatal: 기본 허용 오차로 double 배열 비교
  ```cpp
  double expected[] = {1.5, 2.5, 3.5};
  double actual[] = {1.5, 2.5, 3.5};
  EXPECT_ARRAY_DOUBLE_EQ(expected, actual, 3);
  ```

- **`EXPECT_ARRAY_FLOAT_EQ(expected, actual, size)`** - Non-fatal: 기본 허용 오차로 float 배열 비교
  ```cpp
  float expected[] = {1.25f, 2.25f, 3.25f};
  float actual[] = {1.25f, 2.25f, 3.25f};
  EXPECT_ARRAY_FLOAT_EQ(expected, actual, 3);
  ```

### 오류 메시지

배열이 다를 때, 매크로는 상세한 오류 메시지를 제공합니다:

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

### 다양한 컨테이너 타입 작업

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

### GENERATOR와 결합

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

### 주요 기능

- **요소별 비교**: 각 요소를 개별적으로 비교
- **상세한 오류 메시지**: 어떤 인덱스가 다른지와 값을 표시
- **모든 비교 가능한 타입과 작동**: int, float, double, string, operator==를 가진 사용자 정의 타입
- **성공 메시지**: 모든 요소가 일치하면 "Arrays are equal" 표시
- **벡터와 배열과 호환**: C-style 배열, std::vector, std::array와 작동

### 중요 참고사항

1. **크기 매개변수 필수**: 배열 크기를 명시적으로 제공해야 합니다
2. **치명적 vs 비치명적**: 치명적 어설션에는 ASSERT_*를, 비치명적에는 EXPECT_*를 사용하세요
3. **부동소수점 비교**: 부동소수점 값에는 NEAR, FLOAT_EQ, DOUBLE_EQ를 사용하세요
4. **사용자 정의 타입**: EXPECT_ARRAY_EQ를 사용하려면 타입에 operator==가 정의되어 있어야 합니다
5. **크기 0 배열**: 빈 배열(크기 = 0)과 함께 올바르게 작동합니다

전체 예제는 `test_array_compare.cpp`를 참조하세요.

## 향후 개선사항

- 총 조합 수의 동적 계산
- 생성기에서 다양한 데이터 타입 지원
- 명명된 테스트 인스턴스화
- 더 복잡한 값 패턴 지원

## 라이센스

이 프로젝트는 교육 및 개발 목적으로 제공됩니다.
