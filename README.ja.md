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

## ライセンス

このプロジェクトは教育および開発目的でそのまま提供されています。