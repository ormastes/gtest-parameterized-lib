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
    // 중요: 모든 GENERATOR() 호출은 USE_GENERATOR() 이전에 와야 합니다
    int a = GENERATOR(1, 2);      // 1 또는 2가 됩니다
    int b = GENERATOR(10, 20);    // 10 또는 20이 됩니다
    USE_GENERATOR();              // 모든 GENERATOR() 호출 후에 호출해야 합니다

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

## 고급 예제

### 클래스와 객체 작업

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

### STL 컨테이너 작업

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
    printf("벡터: 크기=%d, 배수=%d\n", size, multiplier);
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

### 올바른 순서가 중요합니다

✅ **올바른 예**:
```cpp
TEST_G(MyTest, Example) {
    int a = GENERATOR(1, 2);     // 먼저: 생성기 정의
    int b = GENERATOR(10, 20);   // 
    USE_GENERATOR();             // 그 다음: USE_GENERATOR() 호출
    // 여기에 테스트 로직
}
```

❌ **잘못된 예**:
```cpp
TEST_G(MyTest, Example) {
    USE_GENERATOR();             // 잘못됨! GENERATOR 호출 이후에 와야 합니다
    int a = GENERATOR(1, 2);     
    int b = GENERATOR(10, 20);   
    // 여기에 테스트 로직
}
```

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

이 라이브러리는 `#define private public`을 사용하거나 프로덕션 코드를 수정하지 않고도 테스트에서 private 멤버에 접근할 수 있는 타입 안전한 방법을 제공합니다.

### 빠른 예제

```cpp
// 프로덕션 클래스에서
class MyClass {
private:
    int privateValue;
    std::string privateName;
public:
    MyClass(int v, const std::string& n) : privateValue(v), privateName(n) {}

    // 테스트를 위한 friend 접근 허용
    FRIEND_ACCESS_PRIVATE();
};

// 테스트 파일에서
using TestBase = gtest_generator::TestWithGenerator;

// 접근자 선언 - 필드 이름만 전달
DECLARE_ACCESS_PRIVATE(id1, TestBase, MyClass, privateValue);
DECLARE_ACCESS_PRIVATE(id2, TestBase, MyClass, privateName);

TEST_G(MyTest, AccessPrivate) {
    int value = GENERATOR(10, 20);
    USE_GENERATOR();

    MyClass obj(value, "test");

    // private 멤버 접근 및 수정
    int& privateRef = ACCESS_PRIVATE(TestBase, TestBase_MyClass_privateValue,
                                      MyClass, &obj);
    EXPECT_EQ(privateRef, value);
    privateRef = 100;
    EXPECT_EQ(privateRef, 100);
}
```

### 주요 기능

- **타입 안전**: 템플릿 특수화와 friend 선언 사용
- **제로 오버헤드**: 완전히 컴파일 타임 메커니즘
- **프로덕션 안전**: 프로덕션 빌드에서 `FRIEND_ACCESS_PRIVATE()`를 빈 매크로로 정의 가능
- **공유 가능**: 선언 블록(`gtest_generator.h`의 260-274줄)을 공통 헤더에 복사 가능

### 중요 사항

1. **타입 별칭 사용**: TestCase 매개변수는 `::`를 포함할 수 없으므로 `using TestBase = gtest_generator::TestWithGenerator;` 사용
2. **필드 이름만**: `&MyClass::privateValue`가 아닌 필드 이름(예: `privateValue`)만 전달
3. **자동 생성 ID**: ID는 `TestCase_TargetClass_MemberName` 패턴을 따름(예: `TestBase_MyClass_privateValue`)

### 고급 사용법

**정적 멤버:**
```cpp
DECLARE_ACCESS_PRIVATE_STATIC(TestBase, MyClass, staticCounter);
```

**사용자 정의 접근자 함수:**
```cpp
DECLARE_ACCESS_PRIVATE_FUNCTION(TestBase, MyClass, CustomAccess) {
    return target->privateField1 + target->privateField2;
}
```

전체 예제는 `test_private_access.cpp`와 `example_common_header.h`를 참조하세요.

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

### 중요 사항

1. **크기 매개변수 필수**: 배열 크기를 명시적으로 제공해야 합니다
2. **Fatal vs Non-fatal**: fatal assertion에는 ASSERT_*를, non-fatal에는 EXPECT_*를 사용하세요
3. **부동소수점 비교**: 부동소수점 값에는 NEAR, FLOAT_EQ 또는 DOUBLE_EQ를 사용하세요
4. **사용자 정의 타입**: EXPECT_ARRAY_EQ를 위해 operator==가 정의되어 있어야 합니다
5. **크기가 0인 배열**: 빈 배열(size = 0)과 올바르게 작동합니다

전체 예제는 `test_array_compare.cpp`를 참조하세요.

## 향후 개선 사항

- 전체 조합의 동적 계산
- 생성기에서 다양한 데이터 타입 지원
- 명명된 테스트 인스턴스화
- 더 복잡한 값 패턴 지원

## 라이선스

이 프로젝트는 교육 및 개발 목적으로 있는 그대로 제공됩니다.