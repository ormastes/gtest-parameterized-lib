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

ライブラリは、明示的なfriend宣言を使用してテストでプライベートメンバーにアクセスするシンプルでクリーンな方法を提供します。

### クイック例

```cpp
// 本番クラス内
class MyClass {
private:
    int privateValue;
    std::string privateName;

    int computeSecret(int x) const { return privateValue * x; }

public:
    MyClass(int v, const std::string& n) : privateValue(v), privateName(n) {}

    // 特定のテストにfriendアクセスを許可
    GTESTG_FRIEND_TEST(MyClassTest, AccessPrivateMembers);
    GTESTG_FRIEND_TEST(MyClassTest, ModifyPrivateMembers);
};

// テストファイル内
struct MyClassTest : ::testing::Test {
    MyClass obj{42, "secret"};
};

TEST_FRIEND(MyClassTest, AccessPrivateMembers) {
    // プライベートメンバーへの直接アクセス！
    EXPECT_EQ(obj.privateValue, 42);
    EXPECT_EQ(obj.privateName, "secret");
}

TEST_FRIEND(MyClassTest, ModifyPrivateMembers) {
    // プライベートメンバーを変更可能
    obj.privateValue = 100;
    EXPECT_EQ(obj.privateValue, 100);

    // プライベートメソッドを呼び出し可能
    int result = obj.computeSecret(2);
    EXPECT_EQ(result, 200);
}
```

### 主な機能

- **シンプルでクリーン**: 標準C++のfriend宣言を使用
- **選択的アクセス**: 必要な特定のテストにのみアクセスを許可
- **ゼロオーバーヘッド**: 純粋なコンパイル時メカニズム、実行時コストなし
- **型安全**: コンパイラによる型安全性の保証
- **本番環境で安全**: friend宣言は実行時に影響を与えません

### APIリファレンス

#### 対象クラス用マクロ

| マクロ | 目的 | 使用方法 |
|-------|---------|-------|
| `GTESTG_FRIEND_TEST(Suite, TestName)` | 特定のTEST_Fテストにfriendアクセスを許可 | クラス定義内に配置 |
| `GTESTG_FRIEND_TEST_G(TestClassName, TestName)` | TEST_Gテストにfriendアクセスを許可 | クラス定義内に配置 |
| `GTESTG_FRIEND_TEST_SUITE(Suite)` | スイート内のすべてのテストにfriendアクセスを許可 | クラス定義内に配置 |

#### テストファイル用マクロ

| マクロ | 目的 | 使用方法 |
|-------|---------|-------|
| `TEST_FRIEND(Suite, TestName)` | friendアクセス付きのテストを定義 | TEST_Fと同じ |
| `TEST_G_FRIEND(TestClassName, TestName)` | friendアクセス付きのジェネレーターテストを定義 | TEST_Gと同じ |

### 使用例

#### 基本的なプライベートアクセス
```cpp
// 対象クラス
class Widget {
private:
    int secret_ = 42;
public:
    // 特定のテストへのアクセスを許可
    GTESTG_FRIEND_TEST(WidgetTest, CheckSecret);
};

// テストファイル
struct WidgetTest : ::testing::Test {
    Widget w;
};

TEST_FRIEND(WidgetTest, CheckSecret) {
    EXPECT_EQ(w.secret_, 42);  // プライベートメンバーへの直接アクセス
}
```

#### プライベートアクセス付きジェネレーターテスト
```cpp
// 対象クラス
class Calculator {
private:
    int factor_ = 10;
    int multiply(int x) const { return x * factor_; }
public:
    // ジェネレーターテストへのアクセスを許可
    GTESTG_FRIEND_TEST_G(CalcTest, TestMultiply);
};

// テストファイル
struct CalcTest : ::gtest_generator::TestWithGenerator {
    Calculator calc;
};

TEST_G_FRIEND(CalcTest, TestMultiply) {
    int input = GENERATOR(1, 2, 3);
    USE_GENERATOR();

    // プライベートメソッドとメンバーへのアクセス
    int result = calc.multiply(input);
    EXPECT_EQ(result, input * calc.factor_);
}
```

#### スイート内のすべてのテストへのアクセスを許可
```cpp
// 対象クラス - MyTestSuite内のすべてのテストにアクセスを許可
class MyClass {
private:
    int value_ = 100;
public:
    // テストスイート全体へのアクセスを許可
    GTESTG_FRIEND_TEST_SUITE(MyTestSuite);
};

// テストファイル - このスイート内のすべてのテストがアクセス可能
struct MyTestSuite : ::testing::Test {
    MyClass obj;
};

TEST_F(MyTestSuite, Test1) {
    EXPECT_EQ(obj.value_, 100);  // アクセス可能
}

TEST_F(MyTestSuite, Test2) {
    obj.value_ = 200;  // こちらもアクセス可能
    EXPECT_EQ(obj.value_, 200);
}
```

#### 継承での使用
```cpp
// 基底クラス
class Base {
private:
    int base_secret_ = 10;
public:
    GTESTG_FRIEND_TEST(DerivedTest, AccessBoth);
};

// 派生クラス
class Derived : public Base {
private:
    int derived_secret_ = 20;
public:
    GTESTG_FRIEND_TEST(DerivedTest, AccessBoth);
};

// テスト
struct DerivedTest : ::testing::Test {
    Derived d;
};

TEST_FRIEND(DerivedTest, AccessBoth) {
    EXPECT_EQ(d.base_secret_, 10);     // 基底クラスのプライベートにアクセス
    EXPECT_EQ(d.derived_secret_, 20);  // 派生クラスのプライベートにアクセス
}
```

### 重要な注意事項

1. **明示的な許可が必要**: プライベートアクセスが必要な各テストは、対象クラスで明示的にリストする必要があります
2. **マジックなし**: 標準C++のfriend宣言を使用 - シンプルで予測可能
3. **TEST_FRIENDはオプション**: `TEST_FRIEND`は`TEST_F`にマッピングされる便利なマクロです。クラスに適切な`GTESTG_FRIEND_TEST`宣言がある場合は、通常の`TEST_F`を使用できます
4. **コンパイル時の安全性**: friendアクセスが許可されていない状態でテストがプライベートメンバーにアクセスしようとすると、コンパイルエラーが発生します
5. **メンテナンス**: プライベートアクセスが必要な新しいテストを追加する場合は、対象クラスに対応する`GTESTG_FRIEND_TEST`宣言を追加することを忘れないでください

### この機能を使用するタイミング

次の場合にプライベートメンバーアクセスを使用します：
- 公開インターフェースを通じて公開されていない内部状態をテストする
- 複雑なプライベートロジックを検証する
- テスト用に特定の内部状態を設定する
- 簡単にリファクタリングできないレガシーコードをテストする

次の場合は使用を避けてください：
- プライベートアクセスの必要性が設計の問題を示している場合
- 公開インターフェースのテストで十分な場合
- テストと実装の間に密結合を作成する場合

完全な例については`test_friend_access.cpp`を参照してください。
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
