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
    // ⚠️ 重要: GENERATOR()呼び出しは必ず最上部に、USE_GENERATOR()の前に配置する必要があります
    // この順序は必須です - ライブラリはテストを実行する前にジェネレータをカウントします
    int a = GENERATOR(1, 2);      // 1または2になります
    int b = GENERATOR(10, 20);    // 10または20になります
    USE_GENERATOR();              // 必ずすべてのGENERATOR()呼び出しの後に配置する必要があります

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

### 式で生成された値の使用
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

### 複雑なテストロジック
```cpp
TEST_G(MyTest, ConditionalLogic) {
    int mode = GENERATOR(0, 1, 2);     // 3つのモード
    int value = GENERATOR(100, 200);   // 2つの値
    USE_GENERATOR();

    int result;
    switch (mode) {
        case 0: result = value + 10; break;
        case 1: result = value * 2; break;
        case 2: result = value - 50; break;
    }

    EXPECT_GT(result, 0);
    printf("モード %d に値 %d を適用した結果 %d\n", mode, value, result);
}
```

## 高度な例

### クラスとオブジェクトの操作

#### 直接オブジェクト生成
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

#### コンストラクタ引数でGENERATORを使用
```cpp
TEST_G(MyTest, ConstructorWithGenerators) {
    // コンストラクタ引数として使用されるGENERATOR値
    int val1 = GENERATOR(1, 2);
    int val2 = GENERATOR(10, 20);
    USE_GENERATOR();

    TestObject objects[] = {
        TestObject(val1, "test"),
        TestObject(val2, "demo")
    };

    EXPECT_LT(objects[0].value, objects[1].value);
    printf("配列オブジェクト: [0]={%d,%s}, [1]={%d,%s}\n",
           objects[0].value, objects[0].name.c_str(),
           objects[1].value, objects[1].name.c_str());
}
```

### ポインタと動的メモリの操作

#### オブジェクトへのポインタの生成
```cpp
TEST_G(MyTest, PointerGeneration) {
    // 異なるオブジェクトへのポインタを生成
    // 注: メモリ管理に注意してください
    auto* ptr1 = GENERATOR(new TestObject(1, "最初"),
                          new TestObject(2, "二番目"));
    auto* ptr2 = GENERATOR(new TestObject(10, "十"),
                          new TestObject(20, "二十"));
    USE_GENERATOR();

    EXPECT_LT(*ptr1, *ptr2);
    printf("ポインタ: ptr1={%d, %s}, ptr2={%d, %s}\n",
           ptr1->value, ptr1->name.c_str(),
           ptr2->value, ptr2->name.c_str());

    // クリーンアップ
    delete ptr1;
    delete ptr2;
}
```

#### ネストされたGENERATOR呼び出し（高度）
```cpp
TEST_G(MyTest, NestedGenerators) {
    // 複雑なネスト生成 - 各外部GENERATORに内部GENERATOR呼び出しが含まれる
    int inner1 = GENERATOR(1, 2);
    int inner2 = GENERATOR(3, 4);
    int inner3 = GENERATOR(10, 20);
    int inner4 = GENERATOR(30, 40);
    USE_GENERATOR();

    auto* obj1 = new TestObject(inner1, "最初");
    auto* obj2 = new TestObject(inner3, "二番目");

    EXPECT_LT(obj1->value, obj2->value);
    printf("ネスト: obj1={%d}, obj2={%d}\n", obj1->value, obj2->value);

    delete obj1;
    delete obj2;
}
```

### STLコンテナの操作

#### コンテナのサイズと内容の生成
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

    printf("ベクター: サイズ=%d, 乗数=%d, 要素=[", size, multiplier);
    for (int v : vec) printf("%d ", v);
    printf("]\n");
}
```

#### 文字列の組み合わせ生成
```cpp
TEST_G(MyTest, StringCombinations) {
    auto prefix_choice = GENERATOR(0, 1);
    auto suffix_choice = GENERATOR(0, 1);
    auto repeat = GENERATOR(1, 2);
    USE_GENERATOR();

    std::string prefix = prefix_choice ? "こんにちは" : "やあ";
    std::string suffix = suffix_choice ? "世界" : "みんな";

    std::string result;
    for (int i = 0; i < repeat; ++i) {
        if (i > 0) result += " ";
        result += prefix + " " + suffix;
    }

    EXPECT_FALSE(result.empty());
    printf("文字列: prefix='%s', suffix='%s', repeat=%d => '%s'\n",
           prefix.c_str(), suffix.c_str(), repeat, result.c_str());
}
```

### スマートポインタの操作

#### unique_ptrとGENERATORの使用
```cpp
TEST_G(MyTest, SmartPointers) {
    auto value1 = GENERATOR(1, 2);
    auto value2 = GENERATOR(10, 20);
    USE_GENERATOR();

    auto ptr1 = std::make_unique<TestObject>(value1, "最初");
    auto ptr2 = std::make_unique<TestObject>(value2, "二番目");

    EXPECT_LT(*ptr1, *ptr2);
    printf("スマートポインタ: ptr1={%d, %s}, ptr2={%d, %s}\n",
           ptr1->value, ptr1->name.c_str(),
           ptr2->value, ptr2->name.c_str());
}
```

### 複雑な構造体の例

#### 複数のフィールドを持つ構造体の生成
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
    printf("点: p1=(%d,%d), p2=(%d,%d), 距離=%d\n",
           p1.x, p1.y, p2.x, p2.y, distance);
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

### ⚠️ 重要: GENERATORとUSE_GENERATORの順序

**非常に重要:** すべての`GENERATOR()`呼び出しは、テストケースの**最上部**で`USE_GENERATOR()`の**前**に配置する**必要があります**。この順序はライブラリが正しく動作するために必須です。

✅ **正しい例**:
```cpp
TEST_G(MyTest, Example) {
    // ステップ1: すべてのGENERATOR()呼び出しを最上部に最初に配置
    int a = GENERATOR(1, 2);
    int b = GENERATOR(10, 20);

    // ステップ2: すべてのジェネレータの後にUSE_GENERATOR()
    USE_GENERATOR();

    // ステップ3: ここにテストロジック
    EXPECT_LT(a, b);
}
```

❌ **間違った例 - 動作しません**:
```cpp
TEST_G(MyTest, Example) {
    USE_GENERATOR();             // ❌ 間違い！ジェネレータの後に来る必要があります
    int a = GENERATOR(1, 2);     // ❌ 遅すぎます！
    int b = GENERATOR(10, 20);
}
```

❌ **間違った例 - 動作しません**:
```cpp
TEST_G(MyTest, Example) {
    int a = GENERATOR(1, 2);
    USE_GENERATOR();             // ❌ 間違い！すべてのジェネレータの後に来る必要があります
    int b = GENERATOR(10, 20);   // ❌ このジェネレータはUSE_GENERATOR()の後です
}
```

**これが重要な理由:**
- ライブラリは事前実行フェーズでジェネレータをカウントします
- `USE_GENERATOR()`はジェネレータ宣言フェーズの終わりをマークします
- `USE_GENERATOR()`の後に宣言されたジェネレータは無視されるか、エラーを引き起こします

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

TEST_F_FRIEND(MyClassTest, AccessPrivateMembers) {
    // プライベートメンバーへの直接アクセス！
    EXPECT_EQ(obj.privateValue, 42);
    EXPECT_EQ(obj.privateName, "secret");
}

TEST_F_FRIEND(MyClassTest, ModifyPrivateMembers) {
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
| `TEST_F_FRIEND(Suite, TestName)` | friendアクセス付きのテストを定義 | TEST_Fと同じ |
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

TEST_F_FRIEND(WidgetTest, CheckSecret) {
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

TEST_F_FRIEND(DerivedTest, AccessBoth) {
    EXPECT_EQ(d.base_secret_, 10);     // 基底クラスのプライベートにアクセス
    EXPECT_EQ(d.derived_secret_, 20);  // 派生クラスのプライベートにアクセス
}
```

### 重要な注意事項

1. **明示的な許可が必要**: プライベートアクセスが必要な各テストは、対象クラスで明示的にリストする必要があります
2. **マジックなし**: 標準C++のfriend宣言を使用 - シンプルで予測可能
3. **TEST_F_FRIENDはオプション**: `TEST_F_FRIEND`は`TEST_F`にマッピングされる便利なマクロです。クラスに適切な`GTESTG_FRIEND_TEST`宣言がある場合は、通常の`TEST_F`を使用できます
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

### 統合プライベートメンバーアクセスシステム

ライブラリは、テストでprivateおよびprotectedメンバーにアクセスするための統合システムを提供します。クラスに単一のマクロ`GTESTG_FRIEND_ACCESS_PRIVATE()`を追加することで、プライベートメンバーアクセスのための**2つの相補的なアプローチ**が有効になります:

1. **TEST_F_FRIEND/TEST_G_FRIENDによる直接アクセス** - ほとんどの場合に推奨
2. **GTESTG_PRIVATE_MEMBERマクロによる関数ベースアクセス** - より明示的な制御のため

両方のアプローチはシームレスに連携し、同じテストで使用できます。

#### コア: GTESTG_FRIEND_ACCESS_PRIVATE()

この単一のマクロをクラスに追加して、プライベートメンバーアクセスを有効にします:

```cpp
class MyClass {
private:
    int privateValue = 42;
    std::string privateName = "secret";
public:
    // 1つのマクロで2つのアクセスアプローチを有効化
    GTESTG_FRIEND_ACCESS_PRIVATE();
};
```

このマクロは以下へのfriendアクセスを許可します:
- **VirtualAccessorテンプレート** - TEST_F_FRIENDおよびTEST_G_FRIENDで使用
- **gtestg_private_accessMember関数** - GTESTG_PRIVATE_MEMBERマクロで使用

#### アプローチ1: TEST_F_FRIENDとTEST_G_FRIENDの使用（推奨）

シンプルな場合は、`TEST_F_FRIEND`または`TEST_G_FRIEND`を使用してプライベートメンバーに直接アクセスできるテストを作成します:

**TEST_F_FRIENDの例:**
```cpp
struct WidgetTest : ::testing::Test {
    Widget w;
};

TEST_F_FRIEND(WidgetTest, AccessPrivate) {
    // プライベートメンバーへの直接アクセス（VirtualAccessor継承を介して）
    EXPECT_EQ(w.secret_, 42);
    w.secret_ = 100;
    EXPECT_EQ(w.secret_, 100);
}
```

**TEST_G_FRIENDの例:**
```cpp
struct WidgetGenTest : ::gtest_generator::TestWithGenerator {
    Widget w{999};
};

TEST_G_FRIEND(WidgetGenTest, GeneratorTest) {
    int factor = GENERATOR(1, 2, 5);
    USE_GENERATOR();

    // パラメータ化テストでも直接アクセス可能
    EXPECT_EQ(w.secret_, 999);
    printf("factor=%d, secret=%d\n", factor, w.secret_);
}
```

**マルチファイルサポート:**
`TEST_F_FRIEND`および`TEST_G_FRIEND`は、同じ実行可能ファイルにリンクされた複数の.cppファイルでテストが定義されている場合に正しく動作します。例については`test_friend_multi_file1.cpp`および`test_friend_multi_file2.cpp`を参照してください。

#### アプローチ2: GTESTG_PRIVATE_MEMBERマクロの使用（明示的制御）

より多くの制御が必要な場合、または通常の`TEST_F`/`TEST_G`マクロを使用する場合は、関数ベースのアクセサマクロを使用します。このアプローチでは、アクセスしたい各メンバーの宣言が必要です。

**ステップ1: クラス外部でアクセスを宣言（テストファイル内）:**
```cpp
// アクセスしたいメンバーを宣言
GTESTG_PRIVATE_DECLARE_MEMBER(Widget, secret_);
GTESTG_PRIVATE_DECLARE_MEMBER(Widget, privateName);
```

**ステップ2: テストでメンバーにアクセス:**
```cpp
TEST_F_FRIEND(WidgetTest, AccessPrivate) {
    // マクロを使用してアクセス
    int& secret = GTESTG_PRIVATE_MEMBER(Widget, secret_, &w);
    EXPECT_EQ(secret, 42);
    secret = 100;
    EXPECT_EQ(secret, 100);
}
```

このアプローチが有用な場合:
- アクセスされるメンバーの明示的なドキュメント化が必要な場合
- 静的メンバーにアクセスする必要がある場合
- 追加ロジックを持つカスタムアクセサ関数が必要な場合

#### 両方のアプローチの組み合わせ

同じテストで両方のアプローチを使用できます:

```cpp
class Widget {
private:
    int secret_ = 42;
    static int counter_;
public:
    GTESTG_FRIEND_ACCESS_PRIVATE();  // 両方のアプローチを有効化
};

int Widget::counter_ = 0;

// 静的メンバーへのアクセスを宣言
GTESTG_PRIVATE_DECLARE_STATIC(Widget, counter_);

TEST_G_FRIEND(WidgetTest, CombinedAccess) {
    int value = GENERATOR(10, 20);
    USE_GENERATOR();

    Widget w;

    // アプローチ1: インスタンスメンバーへの直接アクセス
    w.secret_ = value;
    EXPECT_EQ(w.secret_, value);

    // アプローチ2: 静的メンバーにマクロを使用
    int& count = GTESTG_PRIVATE_STATIC(Widget, counter_);
    count++;
}
```

### GTESTG_PRIVATE_MEMBERマクロの完全なAPIリファレンス

このセクションは、関数ベースのプライベートメンバーアクセスマクロ（アプローチ2）の詳細なリファレンスを提供します。

#### メンバーへのアクセス宣言

これらの宣言をクラスの**外部**、通常はテストファイルに配置します。これらの宣言は、アクセスしたいプライベートメンバーをシステムに伝えます:

| マクロ | 用途 | パラメータ | 例 |
|-------|------|----------|-----|
| `GTESTG_PRIVATE_DECLARE_MEMBER` | インスタンスメンバーへのアクセスを宣言 | Target, MemberName | `GTESTG_PRIVATE_DECLARE_MEMBER(MyClass, privateField)` |
| `GTESTG_PRIVATE_DECLARE_STATIC` | 静的メンバーへのアクセスを宣言 | Target, MemberName | `GTESTG_PRIVATE_DECLARE_STATIC(MyClass, staticCounter)` |
| `GTESTG_PRIVATE_DECLARE_FUNCTION` | カスタムアクセサ関数を宣言 | ThisClass, Target, FuncName | `GTESTG_PRIVATE_DECLARE_FUNCTION(MyTest, MyClass, GetSum)` |

#### メンバーアクセスマクロ

テスト関数の**内部**でこれらのマクロを使用してプライベートメンバーにアクセスします:

| マクロ | 用途 | パラメータ | 例 |
|-------|------|----------|-----|
| `GTESTG_PRIVATE_MEMBER` | インスタンスメンバーへアクセス | Target, MemberName, &obj | `GTESTG_PRIVATE_MEMBER(MyClass, privateField, &obj)` |
| `GTESTG_PRIVATE_STATIC` | 静的メンバーへアクセス | Target, MemberName | `GTESTG_PRIVATE_STATIC(MyClass, staticCounter)` |
| `GTESTG_PRIVATE_CALL` | 明示的なテストオブジェクトでカスタム関数を呼び出し | Target, FuncName, test_obj, &obj | `GTESTG_PRIVATE_CALL(MyClass, GetSum, this, &obj)` |
| `GTESTG_PRIVATE_CALL_ON_TEST` | カスタム関数を呼び出し（暗黙の'this'を使用） | ThisClass, Target, FuncName, &obj | `GTESTG_PRIVATE_CALL_ON_TEST(MyTest, MyClass, GetSum, &obj)` |

#### 詳細な使用例

以下の例は、GTESTG_PRIVATE_*マクロの包括的な使用パターンを示します。

**例1: インスタンスメンバーへのアクセス**
```cpp
class MyClass {
private:
    int privateValue = 42;
    std::string privateName = "secret";
public:
    GTESTG_FRIEND_ACCESS_PRIVATE();
};

// アクセスを宣言（テストファイル内）
GTESTG_PRIVATE_DECLARE_MEMBER(MyClass, privateValue);
GTESTG_PRIVATE_DECLARE_MEMBER(MyClass, privateName);

TEST_G(MyTest, AccessPrivate) {
    int value = GENERATOR(10, 20, 30);
    USE_GENERATOR();

    MyClass obj;

    // プライベートメンバーへアクセスして変更
    int& val = GTESTG_PRIVATE_MEMBER(MyClass, privateValue, &obj);
    EXPECT_EQ(val, 42);
    val = value;  // 変更可能
    EXPECT_EQ(val, value);

    std::string& name = GTESTG_PRIVATE_MEMBER(MyClass, privateName, &obj);
    EXPECT_EQ(name, "secret");
    name = "modified";
}
```

**例2: 静的メンバーへのアクセス**
```cpp
class MyClass {
private:
    static int staticCounter;
public:
    GTESTG_FRIEND_ACCESS_PRIVATE();
};

int MyClass::staticCounter = 100;

// 静的メンバーへのアクセスを宣言
GTESTG_PRIVATE_DECLARE_STATIC(MyClass, staticCounter);

TEST_G(MyTest, AccessStatic) {
    USE_GENERATOR();

    // 静的メンバーへアクセス（オブジェクト不要）
    int& count = GTESTG_PRIVATE_STATIC(MyClass, staticCounter);
    EXPECT_EQ(count, 100);
    count = 200;  // 変更可能
    EXPECT_EQ(count, 200);
}
```

**例3: カスタムアクセサ関数**
```cpp
class MyClass {
private:
    int field1 = 10;
    int field2 = 20;
public:
    GTESTG_FRIEND_ACCESS_PRIVATE();
};

class MyTest : public ::gtest_generator::TestWithGenerator {};

// テストコンテキストとプライベートメンバーの両方にアクセスするカスタム関数を宣言
// THIS = テストオブジェクト、TARGET = アクセスされるオブジェクト
GTESTG_PRIVATE_DECLARE_FUNCTION(MyTest, MyClass, GetSum) {
    // テストコンテキストへアクセス: THIS->GetParam()
    // プライベートメンバーへアクセス: TARGET->field1, TARGET->field2
    return TARGET->field1 + TARGET->field2;
}

TEST_G(MyTest, CustomFunction) {
    int multiplier = GENERATOR(1, 2, 3);
    USE_GENERATOR();

    MyClass obj;

    // CALL_ON_TESTを使用してカスタム関数を呼び出し（暗黙の'this'を使用）
    int sum = GTESTG_PRIVATE_CALL_ON_TEST(MyTest, MyClass, GetSum, &obj);
    EXPECT_EQ(sum, 30);  // 10 + 20

    // 代替: テストオブジェクトを明示的に渡す
    int sum2 = GTESTG_PRIVATE_CALL(MyClass, GetSum, static_cast<MyTest*>(this), &obj);
    EXPECT_EQ(sum2, 30);
}
```

完全な例については、`test_private_access.cpp`および`test_define_macros.cpp`を参照してください。

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
