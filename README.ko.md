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

## API 레퍼런스

### 매크로

- **`TEST_G(TestClassName, TestName)`** - 자동 테스트 생성을 포함한 매개변수화된 테스트를 정의합니다. TEST_P와 별도의 ENABLE_GENERATOR 호출 필요성을 모두 대체합니다.

- **`GENERATOR(...)`** - 테스트 매개변수의 값을 정의합니다. 각 호출은 테스트 조합의 새로운 차원을 생성합니다.
  ```cpp
  int value = GENERATOR(1, 2, 3);  // 3개 변형 생성
  ```
  **중요**: 모든 GENERATOR() 호출은 USE_GENERATOR() 이전에 와야 합니다

- **`USE_GENERATOR()`** - 각 TEST_G에서 한 번 호출되어야 하며, 모든 GENERATOR() 호출 이후에 와야 합니다.

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

## 라이선스

이 프로젝트는 교육 및 개발 목적으로 있는 그대로 제공됩니다.