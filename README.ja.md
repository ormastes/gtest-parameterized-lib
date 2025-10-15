# GTest Generator - パラメータ化テストライブラリ

Google Testのパラメータ化テスト用にCatch2スタイルの`GENERATE`構文を提供するシンプルなヘッダーオンリーライブラリです。

## 主な機能

- Catch2の`GENERATE`に似たシンプルで直感的な構文
- ヘッダーオンリー実装
- すべてのテスト組み合わせの自動生成
- 標準のGoogle Test `TEST_P`マクロと互換性あり

## クイックスタート

```cpp
#include "gtest_generator.h"

// TestWithGeneratorを使用してテストフィクスチャを定義
class MyTest : public ::gtest_generator::TestWithGenerator {
};

// TEST_Gマクロを使用してパラメータ化テストを記述
TEST_G(MyTest, SimpleCase) {
    // 重要：すべてのGENERATOR()呼び出しはUSE_GENERATOR()の前に来る必要があります
    int a = GENERATOR(1, 2);      // 1または2になります
    int b = GENERATOR(10, 20);    // 10または20になります
    USE_GENERATOR();              // すべてのGENERATOR()呼び出しの後に呼び出す必要があります

    EXPECT_LT(a, b);
    printf("テスト: a=%d, b=%d\n", a, b);
}
// TEST_Gが自動的にテスト生成を処理します - ENABLE_GENERATORは不要です！
```

## インストール

これはヘッダーオンリーライブラリです。単に`gtest_generator.h`をプロジェクトにコピーしてインクルードしてください。

## プロジェクトのビルド

### 前提条件

- CMake 3.14以上
- Google Testライブラリ
- C++14互換コンパイラ

### ビルド手順

```bash
# ビルドディレクトリの作成
mkdir build && cd build

# CMakeで構成
cmake ..

# プロジェクトをビルド
cmake --build .

# テストの実行
./mygen_test
```

## その他の例

### 複数の値

```cpp
TEST_G(MyTest, MultipleValues) {
    int x = GENERATOR(1, 2, 3);       // 3つの値
    int y = GENERATOR(10, 20, 30);    // 3つの値
    USE_GENERATOR();
    
    // 9つのテスト組み合わせを生成
    EXPECT_LT(x, y);
}
```

### 異なる数の値

```cpp
TEST_G(MyTest, AsymmetricCombinations) {
    int size = GENERATOR(1, 2, 3);     // 3つの値
    int scale = GENERATOR(10, 100);    // 2つの値
    USE_GENERATOR();
    
    // 6つのテスト組み合わせを生成
    int result = size * scale;
    EXPECT_GT(result, 0);
}
```

## 高度な例

### クラスとオブジェクトの操作

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
    // 完全なオブジェクトを直接生成
    auto obj1 = GENERATOR(TestObject(1, "最初"), TestObject(2, "二番目"));
    auto obj2 = GENERATOR(TestObject(10, "十"), TestObject(20, "二十"));
    USE_GENERATOR();
    
    EXPECT_LT(obj1, obj2);
    printf("オブジェクト: obj1={%d, %s}, obj2={%d, %s}\n", 
           obj1.value, obj1.name.c_str(), 
           obj2.value, obj2.name.c_str());
}
```

### STLコンテナの操作

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
    printf("ベクター: サイズ=%d, 乗数=%d\n", size, multiplier);
}
```

## サンプリングモード

ライブラリはテスト組み合わせの生成に二つのサンプリングモードをサポートします：

### FULLモード（デフォルト - 直積積）
デフォルトモードはすべての可能な値の組み合わせ（直積積）を生成します。これは完全なテストカバレッジを保証する伝統的な動作です。

```cpp
TEST_G(MyTest, FullMode) {
    USE_GENERATOR();  // デフォルトはFULLモード
    // または明示的に: USE_GENERATOR(FULL);
    
    auto x = GENERATOR(1, 2);      // 2つの値
    auto y = GENERATOR(10, 20);    // 2つの値
    auto z = GENERATOR(100, 200);  // 2つの値
    
    // 8回のテスト実行を生成： 2 × 2 × 2 = 8組み合わせ
    // (1,10,100), (1,10,200), (1,20,100), (1,20,200),
    // (2,10,100), (2,10,200), (2,20,100), (2,20,200)
}
```

### ALIGNEDモード（並列反復）
ALIGNEDモードはジッパーのようにすべての列を並列で反復します。各列は各実行で次の値に進み、終端に達すると最初に戻ります。総実行数は最大の列のサイズと等しくなります。

```cpp
TEST_G(MyTest, AlignedMode) {
    USE_GENERATOR(ALIGNED);  // ALIGNEDモードを有効化
    
    auto x = GENERATOR(1, 2);           // 2つの値
    auto y = GENERATOR(10, 20, 30, 40); // 4つの値
    auto z = GENERATOR(100, 200, 300);  // 3つの値
    
    // 4回のテスト実行を生成（最大列サイズ）：
    // 実行 0: (1, 10, 100)  - すべてインデックス 0
    // 実行 1: (2, 20, 200)  - すべてインデックス 1
    // 実行 2: (1, 30, 300)  - xは0にラップ、他はインデックス 2
    // 実行 3: (2, 40, 100)  - xは1、yは3、zは0にラップ
}
```

#### ALIGNEDモードの主な特性：
- **決定的**: 値は順序に（0, 1, 2, ...）ラッピングで選択されます
- **宣言順序**: 列は宣言された順序で処理されます
- **実行数が少ない**: 総実行数 = 最大列サイズ（積ではない）
- **バランスの取れたカバレッジ**: 各列の各値がほぼ同じように使用されます

#### 実装の詳細：
ライブラリはGoogle Testの`GTEST_SKIP()`メカニズムを使用してALIGNEDモードを効率的に実装します：
1. テスト登録時に、ライブラリは各GENERATOR列のサイズをカウントします
2. 実行時に、ライブラリは最大列サイズを計算します
3. 最大サイズを超えるテスト実行は`GTEST_SKIP()`を使用してスキップされます
4. 各列は終端に達すると最初に戻ります（モジュロ演算子を使用）

このアプローチにより以下が保証されます：
- 必要なテスト実行のみが実行されます（すべての直積積ではありません）
- Google Testが報告するテスト数は実際に実行されたテストを反映します
- 不要な組み合わせ生成によるパフォーマンスペナルティがありません

#### 各モードをいつ使うか：
- **FULLモード**: すべての組み合わせの徹底的なテストが必要な場合
- **ALIGNEDモード**: より少ないテスト実行で代表的なサンプリングを望む場合

#### 例の比較：
```cpp
// FULLモード: 3 × 2 × 2 = 12回実行
TEST_G(MyTest, FullExample) {
    USE_GENERATOR(FULL);
    auto a = GENERATOR(1, 2, 3);
    auto b = GENERATOR(10, 20);
    auto c = GENERATOR(100, 200);
    // 12組み合わせすべてを生成
}

// ALIGNEDモード: max(3, 2, 2) = 3回実行
TEST_G(MyTest, AlignedExample) {
    USE_GENERATOR(ALIGNED);
    auto a = GENERATOR(1, 2, 3);
    auto b = GENERATOR(10, 20);
    auto c = GENERATOR(100, 200);
    // 3組み合わせのみ生成：
    // (1, 10, 100), (2, 20, 200), (3, 10, 100)
}
```

#### 検証テストの例：
プロジェクトには、両方のモードが正しく動作することを包括的に検証する`test_mode_counts.cpp`が含まれています：

```cpp
// 3x2x2構成でのテスト
TEST_G(ModeCountTest, Full_3x2x2) {
    int a = GENERATOR(1, 2, 3);        // 3つの値
    int b = GENERATOR(10, 20);         // 2つの値
    int c = GENERATOR(100, 200);       // 2つの値
    USE_GENERATOR(FULL);

    // 検証：正確に12のテストケースを生成 (3 × 2 × 2)
    // すべての組み合わせはユニーク
}

TEST_G(ModeCountTest, Aligned_3x2x2) {
    int a = GENERATOR(1, 2, 3);        // 3つの値
    int b = GENERATOR(10, 20);         // 2つの値
    int c = GENERATOR(100, 200);       // 2つの値
    USE_GENERATOR(ALIGNED);

    // 検証：正確に3のテストケースを生成（最大列サイズ）
    // 結果：(1,10,100), (2,20,200), (3,10,100)
}
```

詳細な出力を確認するために検証テストを実行します：
```bash
./build/test_mode_counts
```

期待される出力には以下が表示されます：
- FULLモード 3x2x2: 12回実行（すべてのユニークな組み合わせ）
- ALIGNEDモード 3x2x2: 3回実行（最大列サイズ）
- FULLモード 2x3x4: 24回実行（すべてのユニークな組み合わせ）
- ALIGNEDモード 2x3x4: 4回実行（最大列サイズ）
- FULLモード 5x1x3: 15回実行（すべてのユニークな組み合わせ）
- ALIGNEDモード 5x1x3: 5回実行（最大列サイズ）

## APIリファレンス

### マクロ

- **`TEST_G(TestClassName, TestName)`** - 自動テスト生成を含むパラメータ化テストを定義します。TEST_Pと個別のENABLE_GENERATOR呼び出しの必要性の両方を置き換えます。

- **`GENERATOR(...)`** - テストパラメータの値を定義します。各呼び出しはテストの組み合わせの新しい次元を作成します。
  ```cpp
  int value = GENERATOR(1, 2, 3);  // 3つのバリアントを作成
  ```
  **重要**: すべてのGENERATOR()呼び出しはUSE_GENERATOR()の前に来る必要があります

- **`USE_GENERATOR()`** - 各TEST_Gで一度呼び出す必要があり、すべてのGENERATOR()呼び出しの後に来る必要があります。デフォルトでFULLモードを使用します。

- **`USE_GENERATOR(mode)`** - 各TEST_Gで一度呼び出す必要があり、すべてのGENERATOR()呼び出しの後に来る必要があります。サンプリングモードを指定します：
  - `USE_GENERATOR(FULL)` - すべての値の直積積（デフォルトと同じ）
  - `USE_GENERATOR(ALIGNED)` - 列を通した並列反復

## 動作原理

ライブラリはテストインスタンス化間でジェネレータの状態を維持するためにスレッドローカル静的変数を使用します。`GENERATOR`が呼び出されると：

1. そのジェネレータの可能な値を登録します
2. 現在のテストパラメータインデックスに基づいて、返す値を計算します
3. すべての組み合わせが自動的に生成されます

## 重要な使用上の注意

### 正しい順序が重要です

✅ **正しい例**:
```cpp
TEST_G(MyTest, Example) {
    int a = GENERATOR(1, 2);     // 最初：ジェネレータを定義
    int b = GENERATOR(10, 20);   // 
    USE_GENERATOR();             // 次に：USE_GENERATOR()を呼び出す
    // ここにテストロジック
}
```

❌ **間違った例**:
```cpp
TEST_G(MyTest, Example) {
    USE_GENERATOR();             // 間違い！GENERATOR呼び出しの後に来る必要があります
    int a = GENERATOR(1, 2);     
    int b = GENERATOR(10, 20);   
    // ここにテストロジック
}
```

## 出力例

```
[==========] 1つのテストスイートから4つのテストを実行中。
[----------] Generator/MyTest__SimpleCaseから4つのテスト
[ RUN      ] Generator/MyTest__SimpleCase.__/0
テスト: a=1, b=10
[       OK ] Generator/MyTest__SimpleCase.__/0 (0 ms)
[ RUN      ] Generator/MyTest__SimpleCase.__/1
テスト: a=2, b=10
[       OK ] Generator/MyTest__SimpleCase.__/1 (0 ms)
[ RUN      ] Generator/MyTest__SimpleCase.__/2
テスト: a=1, b=20
[       OK ] Generator/MyTest__SimpleCase.__/2 (0 ms)
[ RUN      ] Generator/MyTest__SimpleCase.__/3
テスト: a=2, b=20
[       OK ] Generator/MyTest__SimpleCase.__/3 (0 ms)
[==========] 1つのテストスイートから4つのテストが実行されました。(合計0 ms)
[  合格  ] 4つのテスト。
```

## 制限事項

- テストフィクスチャは`::gtest_generator::TestWithGenerator`を継承する必要があります
- 標準の`TEST_P`の代わりに`TEST_G`マクロを使用する必要があります
- すべての`GENERATOR()`呼び出しは`USE_GENERATOR()`の前に来る必要があります
- 複雑な型（オブジェクト、ポインタ）はGENERATORで動作しますが、適切なテンプレートのインスタンス化が必要な場合があります
- `new`でポインタを生成する際のメモリ管理はユーザーの責任です

## テスト用のプライベートメンバーアクセス

ライブラリは、`#define private public`を使用したり本番コードを変更したりすることなく、テストでプライベートメンバーにアクセスする型安全な方法を提供します。

### クイック例

```cpp
// 本番クラス内
class MyClass {
private:
    int privateValue;
    std::string privateName;
public:
    MyClass(int v, const std::string& n) : privateValue(v), privateName(n) {}

    // テスト用のfriendアクセスを許可
    GTESTG_FRIEND_ACCESS_PRIVATE();
};

// テストファイル内


// アクセサを宣言 - フィールド名だけを渡す
GTESTG_PRIVATE_DECLARE_MEMBER(MyClass, privateValue);
GTESTG_PRIVATE_DECLARE_MEMBER(MyClass, privateName);

TEST_G(MyTest, AccessPrivate) {
    int value = GENERATOR(10, 20);
    USE_GENERATOR();

    MyClass obj(value, "test");

    // プライベートメンバーへのアクセスと変更
    int& privateRef = GTESTG_PRIVATE_MEMBER(MyClass, privateValue, &obj);
    EXPECT_EQ(privateRef, value);
    privateRef = 100;
    EXPECT_EQ(privateRef, 100);
}
```

### 主な機能

- **型安全**: テンプレート特殊化とfriend宣言を使用
- **ゼロオーバーヘッド**: 完全なコンパイル時メカニズム
- **本番環境で安全**: `GTESTG_FRIEND_ACCESS_PRIVATE()`は本番ビルドで空のマクロとして定義可能
- **共有可能**: 宣言ブロックを共通ヘッダーにコピー可能
- **名前空間化**: すべてのマクロと関数は名前の衝突を避けるために`GTESTG_`プレフィックスを使用
- **シンプルなAPI**: 最小限のパラメータ、クリーンな構文

### APIリファレンス

#### アクセスの宣言

| マクロ | 目的 | パラメータ | 例 |
|-------|---------|------------|---------|
| `GTESTG_PRIVATE_DECLARE_MEMBER` | インスタンスメンバーへのアクセス | Target, MemberName | `GTESTG_PRIVATE_DECLARE_MEMBER(MyClass, privateField)` |
| `GTESTG_PRIVATE_DECLARE_STATIC` | 静的メンバーへのアクセス | Target, MemberName | `GTESTG_PRIVATE_DECLARE_STATIC(MyClass, staticCounter)` |
| `GTESTG_PRIVATE_DECLARE_FUNCTION` | カスタムアクセサ関数 | ThisClass, Target, FuncName | `GTESTG_PRIVATE_DECLARE_FUNCTION(MyTest, MyClass, GetSum)` |

#### メンバーへのアクセス

| マクロ | 目的 | パラメータ | 例 |
|-------|---------|------------|---------|
| `GTESTG_PRIVATE_MEMBER` | インスタンスメンバーへのアクセス | Target, MemberName, &obj | `GTESTG_PRIVATE_MEMBER(MyClass, privateField, &obj)` |
| `GTESTG_PRIVATE_STATIC` | 静的メンバーへのアクセス | Target, MemberName | `GTESTG_PRIVATE_STATIC(MyClass, staticCounter)` |
| `GTESTG_PRIVATE_CALL` | 明示的なテストオブジェクトでカスタム関数を呼び出す | Target, FuncName, test_obj, &obj | `GTESTG_PRIVATE_CALL(MyClass, GetSum, *this, &obj)` |
| `GTESTG_PRIVATE_CALL_ON_TEST` | カスタム関数を呼び出す（暗黙的な'this'を使用） | ThisClass, Target, FuncName, &obj | `GTESTG_PRIVATE_CALL_ON_TEST(MyTest, MyClass, GetSum, &obj)` |

### 使用例

**インスタンスメンバー：**
```cpp
// 宣言
GTESTG_PRIVATE_DECLARE_MEMBER(MyClass, privateField);

// テスト内でアクセス
int& value = GTESTG_PRIVATE_MEMBER(MyClass, privateField, &obj);
value = 42;  // 変更可能
```

**静的メンバー：**
```cpp
// 宣言
GTESTG_PRIVATE_DECLARE_STATIC(MyClass, staticCounter);

// テスト内でアクセス
int& count = GTESTG_PRIVATE_STATIC(MyClass, staticCounter);
count++;  // 変更可能
```

**カスタム関数：**
```cpp
// カスタムロジックで宣言
// THISはテストコンテキストを提供し、TARGETはアクセスされるオブジェクトです
GTESTG_PRIVATE_DECLARE_FUNCTION(MyTest, MyClass, GetSum) {
    // 必要に応じてテストパラメータにアクセス：THIS->GetParam()
    // ターゲットオブジェクトにアクセス：TARGET->field1, TARGET->field2
    return TARGET->field1 + TARGET->field2;
}

// TEST_G(MyTest, ...)内から呼び出す
// オプション1：CALL_ON_TESTで暗黙的な'this'を使用
int sum1 = GTESTG_PRIVATE_CALL_ON_TEST(MyTest, MyClass, GetSum, &obj);

// オプション2：CALLでテストオブジェクトを明示的に渡す
int sum2 = GTESTG_PRIVATE_CALL(MyClass, GetSum, *this, &obj);
```

**カスタム関数のパラメータ名：**
- `THIS` - テストフィクスチャインスタンスへのポインタ（`GetParam()`などのテストコンテキストを提供）
- `TARGET` - プライベートメンバーにアクセスしているオブジェクトへのポインタ

**実装に関する注意：**
- ライブラリは、型安全なアクセスのためにfriend宣言を使用したテンプレート特殊化を使用します
- ALIGNEDモードでの列インデックス追跡は、テストパラメータ間で自動的にリセットされます（最新バージョンで修正）
- すべてのマクロは名前の衝突を避けるために`GTESTG_`プレフィックスを使用します

完全な例については、`test_private_access.cpp`と`example_common_header.h`を参照してください。

## 配列比較マクロ

このライブラリは、詳細なエラーメッセージとともに配列を要素ごとに比較するための便利なマクロを提供します。これらのマクロはGoogle Testのアサーションマクロの上に構築されています。

### クイック例

```cpp
TEST_G(ArrayTest, CompareArrays) {
    USE_GENERATOR();

    int expected[] = {1, 2, 3, 4, 5};
    int actual[] = {1, 2, 3, 4, 5};

    EXPECT_ARRAY_EQ(expected, actual, 5);  // Non-fatal assertion
}
```

### 利用可能なマクロ

#### 整数型と汎用型

- **`EXPECT_ARRAY_EQ(expected, actual, size)`** - 非致命的：2つの配列を要素ごとに比較
  ```cpp
  int expected[] = {1, 2, 3};
  int actual[] = {1, 2, 3};
  EXPECT_ARRAY_EQ(expected, actual, 3);
  ```

- **`ASSERT_ARRAY_EQ(expected, actual, size)`** - 致命的：2つの配列を要素ごとに比較
  ```cpp
  std::vector<int> expected = {10, 20, 30};
  std::vector<int> actual = {10, 20, 30};
  ASSERT_ARRAY_EQ(expected.data(), actual.data(), 3);  // Test stops if fails
  ```

#### 浮動小数点型

- **`EXPECT_ARRAY_NEAR(expected, actual, size, abs_error)`** - 非致命的：許容誤差を使用して浮動小数点配列を比較
  ```cpp
  double expected[] = {1.0, 2.0, 3.0};
  double actual[] = {1.001, 1.999, 3.002};
  EXPECT_ARRAY_NEAR(expected, actual, 3, 0.01);  // Tolerance: 0.01
  ```

- **`ASSERT_ARRAY_NEAR(expected, actual, size, abs_error)`** - 致命的：許容誤差を使用して浮動小数点配列を比較
  ```cpp
  float expected[] = {1.5f, 2.5f, 3.5f};
  float actual[] = {1.501f, 2.499f, 3.502f};
  ASSERT_ARRAY_NEAR(expected, actual, 3, 0.01f);
  ```

- **`EXPECT_ARRAY_DOUBLE_EQ(expected, actual, size)`** - 非致命的：デフォルトの許容誤差でdouble配列を比較
  ```cpp
  double expected[] = {1.5, 2.5, 3.5};
  double actual[] = {1.5, 2.5, 3.5};
  EXPECT_ARRAY_DOUBLE_EQ(expected, actual, 3);
  ```

- **`EXPECT_ARRAY_FLOAT_EQ(expected, actual, size)`** - 非致命的：デフォルトの許容誤差でfloat配列を比較
  ```cpp
  float expected[] = {1.25f, 2.25f, 3.25f};
  float actual[] = {1.25f, 2.25f, 3.25f};
  EXPECT_ARRAY_FLOAT_EQ(expected, actual, 3);
  ```

### エラーメッセージ

配列が異なる場合、マクロは詳細なエラーメッセージを提供します：

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

### 異なるコンテナ型の操作

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

### GENERATORとの組み合わせ

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

### 主な機能

- **要素ごとの比較**：各要素が個別に比較されます
- **詳細なエラーメッセージ**：どのインデックスが異なるかと値を表示します
- **任意の比較可能な型に対応**：int、float、double、string、operator==を持つカスタム型
- **成功メッセージ**：すべての要素が一致した場合に「Arrays are equal」を表示します
- **ベクターと配列に対応**：C言語スタイルの配列、std::vector、std::arrayで動作します

### 重要な注意事項

1. **サイズパラメータは必須**: 配列のサイズを明示的に提供する必要があります
2. **致命的 vs 非致命的**: 致命的なアサーションにはASSERT_*を、非致命的にはEXPECT_*を使用します
3. **浮動小数点比較**: 浮動小数点値にはNEAR、FLOAT_EQ、またはDOUBLE_EQを使用します
4. **カスタム型**: EXPECT_ARRAY_EQを使用するには、型にoperator==が定義されている必要があります
5. **サイズゼロの配列**: 空の配列（サイズ = 0）で正しく動作します

完全な例については、`test_array_compare.cpp`を参照してください。

## 今後の改善

- 総組み合わせ数の動的計算
- ジェネレータでの異なるデータ型のサポート
- 名前付きテストインスタンス化
- より複雑な値パターンのサポート

## ライセンス

このプロジェクトは教育および開発目的で提供されています。
