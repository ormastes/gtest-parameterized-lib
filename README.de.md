# GTest Generator - Parametrisierte Testbibliothek

Eine einfache Header-Only-Bibliothek, die Catch2-ähnliche `GENERATE`-Syntax für parametrisierte Google Test Tests bereitstellt.

## Hauptfunktionen

- Einfache, intuitive Syntax ähnlich wie Catch2's `GENERATE`
- Header-Only-Implementierung
- Automatische Generierung aller Testkombinationen
- Kompatibel mit Standard Google Test `TEST_P` Makros

## Schnellstart

```cpp
#include "gtest_generator.h"

// Test-Fixture mit TestWithGenerator definieren
class MyTest : public ::gtest_generator::TestWithGenerator {
};

// Parametrisierten Test mit TEST_G Makro schreiben
TEST_G(MyTest, SimpleCase) {
    // ⚠️ KRITISCH: GENERATOR() Aufrufe MÜSSEN ganz OBEN stehen, VOR USE_GENERATOR()
    // Diese Reihenfolge ist zwingend erforderlich - die Bibliothek zählt Generatoren vor der Testausführung
    int a = GENERATOR(1, 2);      // Wird 1 oder 2 sein
    int b = GENERATOR(10, 20);    // Wird 10 oder 20 sein
    USE_GENERATOR();              // MUSS NACH allen GENERATOR() Aufrufen aufgerufen werden

    EXPECT_LT(a, b);
    printf("Test: a=%d, b=%d\n", a, b);
}
// TEST_G übernimmt automatisch die Testgenerierung - kein ENABLE_GENERATOR nötig!
```

## Installation

Dies ist eine Header-Only-Bibliothek. Kopieren Sie einfach `gtest_generator.h` in Ihr Projekt und binden Sie es ein.

## Projekt bauen

### Voraussetzungen

- CMake 3.14 oder höher
- Google Test Bibliothek
- C++14 kompatibler Compiler

### Build-Anweisungen

```bash
# Build-Verzeichnis erstellen
mkdir build && cd build

# Mit CMake konfigurieren
cmake ..

# Projekt bauen
cmake --build .

# Tests ausführen
./mygen_test
```

## Weitere Beispiele

### Mehrere Werte

```cpp
TEST_G(MyTest, MultipleValues) {
    int x = GENERATOR(1, 2, 3);       // 3 Werte
    int y = GENERATOR(10, 20, 30);    // 3 Werte
    USE_GENERATOR();
    
    // Generiert 9 Testkombinationen
    EXPECT_LT(x, y);
}
```

### Unterschiedliche Anzahl von Werten

```cpp
TEST_G(MyTest, AsymmetricCombinations) {
    int size = GENERATOR(1, 2, 3);     // 3 Werte
    int scale = GENERATOR(10, 100);    // 2 Werte
    USE_GENERATOR();

    // Generiert 6 Testkombinationen
    int result = size * scale;
    EXPECT_GT(result, 0);
}
```

### Verwendung generierter Werte in Ausdrücken

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

### Komplexe Testlogik

```cpp
TEST_G(MyTest, ConditionalLogic) {
    int mode = GENERATOR(0, 1, 2);     // 3 Modi
    int value = GENERATOR(100, 200);   // 2 Werte
    USE_GENERATOR();

    int result;
    switch (mode) {
        case 0: result = value + 10; break;
        case 1: result = value * 2; break;
        case 2: result = value - 50; break;
    }

    EXPECT_GT(result, 0);
    printf("Modus %d mit Wert %d ergibt Ergebnis %d\n", mode, value, result);
}
```

## Erweiterte Beispiele

### Arbeiten mit Klassen und Objekten

#### Direkte Objektgenerierung

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
    // Vollständige Objekte direkt generieren
    auto obj1 = GENERATOR(TestObject(1, "erstes"), TestObject(2, "zweites"));
    auto obj2 = GENERATOR(TestObject(10, "zehn"), TestObject(20, "zwanzig"));
    USE_GENERATOR();

    EXPECT_LT(obj1, obj2);
    printf("Objekte: obj1={%d, %s}, obj2={%d, %s}\n",
           obj1.value, obj1.name.c_str(),
           obj2.value, obj2.name.c_str());
}
```

#### Verwendung von GENERATOR in Konstruktorargumenten

```cpp
TEST_G(MyTest, ConstructorWithGenerators) {
    // GENERATOR-Werte als Konstruktorargumente verwendet
    int val1 = GENERATOR(1, 2);
    int val2 = GENERATOR(10, 20);
    USE_GENERATOR();

    TestObject objects[] = {
        TestObject(val1, "test"),
        TestObject(val2, "demo")
    };

    EXPECT_LT(objects[0].value, objects[1].value);
    printf("Array-Objekte: [0]={%d,%s}, [1]={%d,%s}\n",
           objects[0].value, objects[0].name.c_str(),
           objects[1].value, objects[1].name.c_str());
}
```

### Arbeiten mit Zeigern und dynamischem Speicher

#### Generierung von Zeigern auf Objekte

```cpp
TEST_G(MyTest, PointerGeneration) {
    // Zeiger auf verschiedene Objekte generieren
    // Hinweis: Achten Sie auf die Speicherverwaltung
    auto* ptr1 = GENERATOR(new TestObject(1, "erstes"),
                          new TestObject(2, "zweites"));
    auto* ptr2 = GENERATOR(new TestObject(10, "zehn"),
                          new TestObject(20, "zwanzig"));
    USE_GENERATOR();

    EXPECT_LT(*ptr1, *ptr2);
    printf("Zeiger: ptr1={%d, %s}, ptr2={%d, %s}\n",
           ptr1->value, ptr1->name.c_str(),
           ptr2->value, ptr2->name.c_str());

    // Aufräumen
    delete ptr1;
    delete ptr2;
}
```

#### Verschachtelte GENERATOR-Aufrufe (Fortgeschritten)

```cpp
TEST_G(MyTest, NestedGenerators) {
    // Komplexe verschachtelte Generierung - jeder äußere GENERATOR enthält innere GENERATOR-Aufrufe
    int inner1 = GENERATOR(1, 2);
    int inner2 = GENERATOR(3, 4);
    int inner3 = GENERATOR(10, 20);
    int inner4 = GENERATOR(30, 40);
    USE_GENERATOR();

    auto* obj1 = new TestObject(inner1, "erstes");
    auto* obj2 = new TestObject(inner3, "zweites");

    EXPECT_LT(obj1->value, obj2->value);
    printf("Verschachtelt: obj1={%d}, obj2={%d}\n", obj1->value, obj2->value);

    delete obj1;
    delete obj2;
}
```

### Arbeiten mit STL-Containern

#### Generierung von Containergrößen und -inhalten

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

    printf("Vektor: Größe=%d, Multiplikator=%d, Elemente=[", size, multiplier);
    for (int v : vec) printf("%d ", v);
    printf("]\n");
}
```

#### Generierung von Stringkombinationen

```cpp
TEST_G(MyTest, StringCombinations) {
    auto prefix_choice = GENERATOR(0, 1);
    auto suffix_choice = GENERATOR(0, 1);
    auto repeat = GENERATOR(1, 2);
    USE_GENERATOR();

    std::string prefix = prefix_choice ? "Hallo" : "Hi";
    std::string suffix = suffix_choice ? "Welt" : "Dort";

    std::string result;
    for (int i = 0; i < repeat; ++i) {
        if (i > 0) result += " ";
        result += prefix + " " + suffix;
    }

    EXPECT_FALSE(result.empty());
    printf("String: prefix='%s', suffix='%s', repeat=%d => '%s'\n",
           prefix.c_str(), suffix.c_str(), repeat, result.c_str());
}
```

### Arbeiten mit Smart Pointern

#### Verwendung von unique_ptr mit GENERATOR

```cpp
TEST_G(MyTest, SmartPointers) {
    auto value1 = GENERATOR(1, 2);
    auto value2 = GENERATOR(10, 20);
    USE_GENERATOR();

    auto ptr1 = std::make_unique<TestObject>(value1, "erstes");
    auto ptr2 = std::make_unique<TestObject>(value2, "zweites");

    EXPECT_LT(*ptr1, *ptr2);
    printf("Smart Pointer: ptr1={%d, %s}, ptr2={%d, %s}\n",
           ptr1->value, ptr1->name.c_str(),
           ptr2->value, ptr2->name.c_str());
}
```

### Beispiele für komplexe Strukturen

#### Generierung von Strukturen mit mehreren Feldern

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
    printf("Punkte: p1=(%d,%d), p2=(%d,%d), Abstand=%d\n",
           p1.x, p1.y, p2.x, p2.y, distance);
}
```

## Sampling-Modi

Die Bibliothek unterstützt zwei Sampling-Modi für die Generierung von Testkombinationen:

### FULL-Modus (Standard - Kartesisches Produkt)
Der Standardmodus generiert alle möglichen Kombinationen von Werten (Kartesisches Produkt). Dies ist das traditionelle Verhalten, das vollständige Testabdeckung gewährleistet.

```cpp
TEST_G(MyTest, FullMode) {
    USE_GENERATOR();  // Standard ist FULL-Modus
    // oder explizit: USE_GENERATOR(FULL);
    
    auto x = GENERATOR(1, 2);      // 2 Werte
    auto y = GENERATOR(10, 20);    // 2 Werte
    auto z = GENERATOR(100, 200);  // 2 Werte
    
    // Generiert 8 Testläufe: 2 × 2 × 2 = 8 Kombinationen
    // (1,10,100), (1,10,200), (1,20,100), (1,20,200),
    // (2,10,100), (2,10,200), (2,20,100), (2,20,200)
}
```

### ALIGNED-Modus (Parallele Iteration)
Der ALIGNED-Modus iteriert durch alle Spalten parallel, wie ein Reißverschluss. Jede Spalte rückt bei jedem Lauf zum nächsten Wert vor und springt an den Anfang, wenn sie das Ende erreicht. Die Gesamtanzahl der Läufe entspricht der Größe der größten Spalte.

```cpp
TEST_G(MyTest, AlignedMode) {
    USE_GENERATOR(ALIGNED);  // ALIGNED-Modus aktivieren
    
    auto x = GENERATOR(1, 2);           // 2 Werte
    auto y = GENERATOR(10, 20, 30, 40); // 4 Werte  
    auto z = GENERATOR(100, 200, 300);  // 3 Werte
    
    // Generiert 4 Testläufe (maximale Spaltengröße):
    // Lauf 0: (1, 10, 100)  - alle bei Index 0
    // Lauf 1: (2, 20, 200)  - alle bei Index 1
    // Lauf 2: (1, 30, 300)  - x springt zu 0, andere bei Index 2
    // Lauf 3: (2, 40, 100)  - x bei 1, y bei 3, z springt zu 0
}
```

#### Wichtige Eigenschaften des ALIGNED-Modus:
- **Deterministisch**: Werte werden in Reihenfolge (0, 1, 2, ...) mit Umbruch ausgewählt
- **Deklarationsreihenfolge**: Spalten werden in der Reihenfolge verarbeitet, in der sie deklariert wurden
- **Weniger Läufe**: Gesamtläufe = maximale Spaltengröße (nicht das Produkt)
- **Ausgewogene Abdeckung**: Jeder Wert in jeder Spalte wird ungefähr gleich häufig verwendet

#### Implementierungsdetails:
Die Bibliothek implementiert den ALIGNED-Modus effizient unter Verwendung des `GTEST_SKIP()`-Mechanismus von Google Test:
1. Während der Testregistrierung zählt die Bibliothek die Größe jeder GENERATOR-Spalte
2. Zur Laufzeit berechnet die Bibliothek die maximale Spaltengröße
3. Testläufe über die maximale Größe hinaus werden mit `GTEST_SKIP()` übersprungen
4. Jede Spalte wird zurückgesetzt, wenn sie ihr Ende erreicht (unter Verwendung des Modulo-Operators)

Dieser Ansatz stellt sicher, dass:
- Nur die notwendigen Testläufe ausgeführt werden (nicht alle kartesischen Produkte)
- Die von Google Test gemeldete Testanzahl die tatsächlich ausgeführten Tests widerspiegelt
- Keine Leistungseinbuße durch Generierung unnötiger Kombinationen entsteht

#### Wann welcher Modus zu verwenden ist:
- **FULL-Modus**: Wenn Sie erschöpfende Tests aller Kombinationen benötigen
- **ALIGNED-Modus**: Wenn Sie repräsentatives Sampling mit weniger Testläufen wünschen

#### Vergleichsbeispiel:
```cpp
// FULL-Modus: 3 × 2 × 2 = 12 Läufe
TEST_G(MyTest, FullExample) {
    USE_GENERATOR(FULL);
    auto a = GENERATOR(1, 2, 3);
    auto b = GENERATOR(10, 20);
    auto c = GENERATOR(100, 200);
    // Generiert alle 12 Kombinationen
}

// ALIGNED-Modus: max(3, 2, 2) = 3 Läufe
TEST_G(MyTest, AlignedExample) {
    USE_GENERATOR(ALIGNED);
    auto a = GENERATOR(1, 2, 3);
    auto b = GENERATOR(10, 20);
    auto c = GENERATOR(100, 200);
    // Generiert nur 3 Kombinationen:
    // (1, 10, 100), (2, 20, 200), (3, 10, 100)
}
```

#### Beispiel für Verifikationstest:
Das Projekt enthält `test_mode_counts.cpp`, das umfassend verifiziert, dass beide Modi korrekt funktionieren:

```cpp
// Test mit 3x2x2 Konfiguration
TEST_G(ModeCountTest, Full_3x2x2) {
    int a = GENERATOR(1, 2, 3);        // 3 Werte
    int b = GENERATOR(10, 20);         // 2 Werte
    int c = GENERATOR(100, 200);       // 2 Werte
    USE_GENERATOR(FULL);

    // Verifiziert: Generiert genau 12 Testfälle (3 × 2 × 2)
    // Alle Kombinationen sind eindeutig
}

TEST_G(ModeCountTest, Aligned_3x2x2) {
    int a = GENERATOR(1, 2, 3);        // 3 Werte
    int b = GENERATOR(10, 20);         // 2 Werte
    int c = GENERATOR(100, 200);       // 2 Werte
    USE_GENERATOR(ALIGNED);

    // Verifiziert: Generiert genau 3 Testfälle (maximale Spaltengröße)
    // Ergebnisse: (1,10,100), (2,20,200), (3,10,100)
}
```

Führen Sie den Verifikationstest aus, um die detaillierte Ausgabe zu sehen:
```bash
./build/test_mode_counts
```

Die erwartete Ausgabe zeigt:
- FULL-Modus 3x2x2: 12 Läufe mit allen eindeutigen Kombinationen
- ALIGNED-Modus 3x2x2: 3 Läufe (maximale Spaltengröße)
- FULL-Modus 2x3x4: 24 Läufe mit allen eindeutigen Kombinationen
- ALIGNED-Modus 2x3x4: 4 Läufe (maximale Spaltengröße)
- FULL-Modus 5x1x3: 15 Läufe mit allen eindeutigen Kombinationen
- ALIGNED-Modus 5x1x3: 5 Läufe (maximale Spaltengröße)

## API-Referenz

### Makros

- **`TEST_G(TestClassName, TestName)`** - Definiert einen parametrisierten Test mit automatischer Testgenerierung. Ersetzt sowohl TEST_P als auch die Notwendigkeit separater ENABLE_GENERATOR-Aufrufe.

- **`GENERATOR(...)`** - Definiert Werte für einen Testparameter. Jeder Aufruf erstellt eine neue Dimension von Testkombinationen.
  ```cpp
  int value = GENERATOR(1, 2, 3);  // Erstellt 3 Varianten
  ```
  **WICHTIG**: Alle GENERATOR() Aufrufe müssen VOR USE_GENERATOR() kommen

- **`USE_GENERATOR()`** - Muss einmal in jedem TEST_G aufgerufen werden, NACH allen GENERATOR() Aufrufen. Verwendet standardmäßig den FULL-Modus.

- **`USE_GENERATOR(mode)`** - Muss einmal in jedem TEST_G aufgerufen werden, NACH allen GENERATOR() Aufrufen. Spezifiziert den Sampling-Modus:
  - `USE_GENERATOR(FULL)` - Kartesisches Produkt aller Werte (gleich wie Standard)
  - `USE_GENERATOR(ALIGNED)` - Parallele Iteration durch Spalten

## Wie es funktioniert

Die Bibliothek verwendet thread-lokale statische Variablen, um den Generator-Zustand zwischen Test-Instanziierungen zu erhalten. Wenn `GENERATOR` aufgerufen wird:

1. Registriert es die möglichen Werte für diesen Generator
2. Basierend auf dem aktuellen Testparameter-Index berechnet es, welcher Wert zurückgegeben werden soll
3. Alle Kombinationen werden automatisch generiert

## Wichtige Nutzungshinweise

### ⚠️ KRITISCH: Reihenfolge von GENERATOR und USE_GENERATOR

**SEHR WICHTIG:** Alle `GENERATOR()` Aufrufe **MÜSSEN** ganz **OBEN** im Testfall stehen, **VOR** `USE_GENERATOR()`. Diese Reihenfolge ist zwingend erforderlich, damit die Bibliothek korrekt funktioniert.

✅ **RICHTIG**:
```cpp
TEST_G(MyTest, Example) {
    // Schritt 1: ALLE GENERATOR() Aufrufe zuerst ganz OBEN
    int a = GENERATOR(1, 2);
    int b = GENERATOR(10, 20);

    // Schritt 2: USE_GENERATOR() NACH allen Generatoren
    USE_GENERATOR();

    // Schritt 3: Ihre Test-Logik hier
    EXPECT_LT(a, b);
}
```

❌ **FALSCH - wird NICHT funktionieren**:
```cpp
TEST_G(MyTest, Example) {
    USE_GENERATOR();             // ❌ FALSCH! Muss nach Generatoren kommen
    int a = GENERATOR(1, 2);     // ❌ Zu spät!
    int b = GENERATOR(10, 20);
}
```

❌ **FALSCH - wird NICHT funktionieren**:
```cpp
TEST_G(MyTest, Example) {
    int a = GENERATOR(1, 2);
    USE_GENERATOR();             // ❌ FALSCH! Muss nach ALLEN Generatoren kommen
    int b = GENERATOR(10, 20);   // ❌ Dieser Generator ist nach USE_GENERATOR()
}
```

**Warum das wichtig ist:**
- Die Bibliothek zählt Generatoren während einer Vorabausführungsphase
- `USE_GENERATOR()` markiert das Ende der Generator-Deklarationsphase
- Nach `USE_GENERATOR()` deklarierte Generatoren werden ignoriert oder verursachen Fehler

## Beispielausgabe

```
[==========] Führe 4 Tests aus 1 Test-Suite aus.
[----------] 4 Tests von Generator/MyTest__SimpleCase
[ RUN      ] Generator/MyTest__SimpleCase.__/0
Test: a=1, b=10
[       OK ] Generator/MyTest__SimpleCase.__/0 (0 ms)
[ RUN      ] Generator/MyTest__SimpleCase.__/1
Test: a=2, b=10
[       OK ] Generator/MyTest__SimpleCase.__/1 (0 ms)
[ RUN      ] Generator/MyTest__SimpleCase.__/2
Test: a=1, b=20
[       OK ] Generator/MyTest__SimpleCase.__/2 (0 ms)
[ RUN      ] Generator/MyTest__SimpleCase.__/3
Test: a=2, b=20
[       OK ] Generator/MyTest__SimpleCase.__/3 (0 ms)
[==========] 4 Tests aus 1 Test-Suite liefen. (0 ms insgesamt)
[  BESTANDEN  ] 4 Tests.
```

## Einschränkungen

- Test-Fixture muss von `::gtest_generator::TestWithGenerator` erben
- Muss `TEST_G` Makro anstelle des Standard `TEST_P` verwenden
- Alle `GENERATOR()` Aufrufe müssen vor `USE_GENERATOR()` kommen
- Komplexe Typen (Objekte, Zeiger) funktionieren mit GENERATOR, benötigen aber möglicherweise eine angemessene Template-Instanziierung
- Speicherverwaltung ist Verantwortung des Benutzers beim Generieren von Zeigern mit `new`

## Zugriff auf Private Member zum Testen

Die Bibliothek bietet eine einfache und saubere Möglichkeit, auf private Member in Tests zuzugreifen, indem explizite Friend-Deklarationen verwendet werden.

### Schnellbeispiel

```cpp
// In Ihrer Produktionsklasse
class MyClass {
private:
    int privateValue;
    std::string privateName;

    int computeSecret(int x) const { return privateValue * x; }

public:
    MyClass(int v, const std::string& n) : privateValue(v), privateName(n) {}

    // Friend-Zugriff für spezifische Tests gewähren
    GTESTG_FRIEND_TEST(MyClassTest, AccessPrivateMembers);
    GTESTG_FRIEND_TEST(MyClassTest, ModifyPrivateMembers);
};

// In Ihrer Testdatei
struct MyClassTest : ::testing::Test {
    MyClass obj{42, "secret"};
};

TEST_FRIEND(MyClassTest, AccessPrivateMembers) {
    // Direkter Zugriff auf private Member!
    EXPECT_EQ(obj.privateValue, 42);
    EXPECT_EQ(obj.privateName, "secret");
}

TEST_FRIEND(MyClassTest, ModifyPrivateMembers) {
    // Kann private Member ändern
    obj.privateValue = 100;
    EXPECT_EQ(obj.privateValue, 100);

    // Kann private Methoden aufrufen
    int result = obj.computeSecret(2);
    EXPECT_EQ(result, 200);
}
```

### Hauptmerkmale

- **Einfach und sauber**: Verwendet Standard-C++-Friend-Deklarationen
- **Selektiver Zugriff**: Gewähren Sie Zugriff nur für spezifische Tests, die ihn benötigen
- **Null Overhead**: Reiner Compile-Zeit-Mechanismus, keine Laufzeitkosten
- **Typsicher**: Vom Compiler erzwungene Typsicherheit
- **Produktionssicher**: Friend-Deklarationen haben keine Auswirkungen zur Laufzeit

### API-Referenz

#### Makros für Zielklassen

| Makro | Zweck | Verwendung |
|-------|---------|-------|
| `GTESTG_FRIEND_TEST(Suite, TestName)` | Friend-Zugriff für einen spezifischen TEST_F-Test gewähren | In Klassendefinition platzieren |
| `GTESTG_FRIEND_TEST_G(TestClassName, TestName)` | Friend-Zugriff für einen TEST_G-Test gewähren | In Klassendefinition platzieren |
| `GTESTG_FRIEND_TEST_SUITE(Suite)` | Friend-Zugriff für alle Tests in einer Suite gewähren | In Klassendefinition platzieren |

#### Makros für Testdateien

| Makro | Zweck | Verwendung |
|-------|---------|-------|
| `TEST_FRIEND(Suite, TestName)` | Einen Test mit Friend-Zugriff definieren | Wie TEST_F |
| `TEST_G_FRIEND(TestClassName, TestName)` | Einen Generator-Test mit Friend-Zugriff definieren | Wie TEST_G |

### Verwendungsbeispiele

#### Grundlegender privater Zugriff
```cpp
// Zielklasse
class Widget {
private:
    int secret_ = 42;
public:
    // Zugriff für spezifischen Test gewähren
    GTESTG_FRIEND_TEST(WidgetTest, CheckSecret);
};

// Testdatei
struct WidgetTest : ::testing::Test {
    Widget w;
};

TEST_FRIEND(WidgetTest, CheckSecret) {
    EXPECT_EQ(w.secret_, 42);  // Direkter Zugriff auf private Member
}
```

#### Generator-Tests mit privatem Zugriff
```cpp
// Zielklasse
class Calculator {
private:
    int factor_ = 10;
    int multiply(int x) const { return x * factor_; }
public:
    // Zugriff für Generator-Test gewähren
    GTESTG_FRIEND_TEST_G(CalcTest, TestMultiply);
};

// Testdatei
struct CalcTest : ::gtest_generator::TestWithGenerator {
    Calculator calc;
};

TEST_G_FRIEND(CalcTest, TestMultiply) {
    int input = GENERATOR(1, 2, 3);
    USE_GENERATOR();

    // Zugriff auf private Methode und Member
    int result = calc.multiply(input);
    EXPECT_EQ(result, input * calc.factor_);
}
```

#### Zugriff für alle Tests in einer Suite gewähren
```cpp
// Zielklasse - gewährt Zugriff für ALLE Tests in MyTestSuite
class MyClass {
private:
    int value_ = 100;
public:
    // Zugriff für gesamte Test-Suite gewähren
    GTESTG_FRIEND_TEST_SUITE(MyTestSuite);
};

// Testdatei - alle Tests in dieser Suite haben Zugriff
struct MyTestSuite : ::testing::Test {
    MyClass obj;
};

TEST_F(MyTestSuite, Test1) {
    EXPECT_EQ(obj.value_, 100);  // Hat Zugriff
}

TEST_F(MyTestSuite, Test2) {
    obj.value_ = 200;  // Hat ebenfalls Zugriff
    EXPECT_EQ(obj.value_, 200);
}
```

#### Arbeiten mit Vererbung
```cpp
// Basisklasse
class Base {
private:
    int base_secret_ = 10;
public:
    GTESTG_FRIEND_TEST(DerivedTest, AccessBoth);
};

// Abgeleitete Klasse
class Derived : public Base {
private:
    int derived_secret_ = 20;
public:
    GTESTG_FRIEND_TEST(DerivedTest, AccessBoth);
};

// Test
struct DerivedTest : ::testing::Test {
    Derived d;
};

TEST_FRIEND(DerivedTest, AccessBoth) {
    EXPECT_EQ(d.base_secret_, 10);     // Zugriff auf Basis-private
    EXPECT_EQ(d.derived_secret_, 20);  // Zugriff auf abgeleitete private
}
```

### Wichtige Hinweise

1. **Explizite Gewährung erforderlich**: Jeder Test, der privaten Zugriff benötigt, muss explizit in der Zielklasse aufgeführt werden
2. **Keine Magie**: Verwendet Standard-C++-Friend-Deklarationen - einfach und vorhersagbar
3. **TEST_FRIEND ist optional**: `TEST_FRIEND` ist nur ein Convenience-Makro, das auf `TEST_F` abbildet. Sie können reguläres `TEST_F` verwenden, wenn die Klasse die entsprechende `GTESTG_FRIEND_TEST`-Deklaration hat
4. **Compile-Zeit-Sicherheit**: Wenn ein Test versucht, auf private Member ohne Friend-Zugriff zuzugreifen, erhalten Sie einen Compile-Fehler
5. **Wartung**: Wenn Sie einen neuen Test hinzufügen, der privaten Zugriff benötigt, denken Sie daran, die entsprechende `GTESTG_FRIEND_TEST`-Deklaration zur Zielklasse hinzuzufügen

### Wann diese Funktion verwendet werden sollte

Verwenden Sie privaten Member-Zugriff, wenn:
- Interne Zustände getestet werden, die nicht über die öffentliche Schnittstelle verfügbar sind
- Komplexe private Logik überprüft wird
- Spezifische interne Zustände für Tests eingerichtet werden
- Legacy-Code getestet wird, der nicht einfach refaktoriert werden kann

Vermeiden Sie die Verwendung, wenn:
- Die Notwendigkeit für privaten Zugriff auf schlechtes Design hinweist
- Das Testen der öffentlichen Schnittstelle ausreichend wäre
- Dies eine enge Kopplung zwischen Tests und Implementierung schaffen würde

Siehe `test_friend_access.cpp` für vollständige Beispiele.

### TEST_FRIEND und TEST_G_FRIEND Makros

Die Bibliothek bietet `TEST_FRIEND` und `TEST_G_FRIEND` Makros, die Testinfrastruktur mit eingebauter Unterstützung für das VirtualAccessor-Muster erstellen. Diese Makros arbeiten nahtlos mit der `GTESTG_FRIEND_ACCESS_PRIVATE()` Deklaration zusammen.

**Wichtige Punkte:**
- `GTESTG_FRIEND_ACCESS_PRIVATE()` gewährt Friend-Zugriff für **beide** klassenbasierte (VirtualAccessor) und funktionsbasierte (gtestg_private_accessMember) Ansätze
- Verwenden Sie `TEST_FRIEND` für reguläre TEST_F-Stil Tests
- Verwenden Sie `TEST_G_FRIEND` für generatorbasierte parametrisierte Tests
- Verwenden Sie weiterhin `GTESTG_PRIVATE_MEMBER` Makros für den Zugriff auf private Member

**TEST_FRIEND Beispiel:**
```cpp
class Widget {
private:
    int secret_ = 42;
public:
    Widget() = default;

    // Ein einziges Makro gewährt beide Arten von Friend-Zugriff
    GTESTG_FRIEND_ACCESS_PRIVATE();
};

// Accessor deklarieren
GTESTG_PRIVATE_DECLARE_MEMBER(Widget, secret_);

struct WidgetTest : ::testing::Test {
    Widget w;
};

TEST_FRIEND(WidgetTest, AccessPrivate) {
    // Zugriff auf private Member über funktionsbasierten Accessor
    int& secret = GTESTG_PRIVATE_MEMBER(Widget, secret_, &w);
    EXPECT_EQ(secret, 42);
    secret = 100;
    EXPECT_EQ(secret, 100);
}
```

**TEST_G_FRIEND Beispiel:**
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

**Multi-Datei-Unterstützung:**
`TEST_FRIEND` und `TEST_G_FRIEND` funktionieren korrekt, wenn Tests in mehreren .cpp-Dateien definiert sind, die zur selben ausführbaren Datei gelinkt werden, genau wie reguläres `TEST_G`. Siehe `test_friend_multi_file1.cpp` und `test_friend_multi_file2.cpp` für Beispiele.

### Einheitliches System für privaten Memberzugriff

Die Bibliothek bietet ein einheitliches System für den Zugriff auf private und protected Member in Ihren Tests. Durch Hinzufügen eines einzigen Makros `GTESTG_FRIEND_ACCESS_PRIVATE()` zu Ihrer Klasse ermöglichen Sie **zwei komplementäre Ansätze** für den Zugriff auf private Member:

1. **Direkter Zugriff über TEST_FRIEND/TEST_G_FRIEND** - Empfohlen für die meisten Fälle
2. **Funktionsbasierter Zugriff über GTESTG_PRIVATE_MEMBER Makros** - Für explizitere Kontrolle

Beide Ansätze arbeiten nahtlos zusammen und können im selben Test verwendet werden.

#### Der Kern: GTESTG_FRIEND_ACCESS_PRIVATE()

Fügen Sie dieses einzelne Makro zu Ihrer Klasse hinzu, um den Zugriff auf private Member zu ermöglichen:

```cpp
class MyClass {
private:
    int privateValue = 42;
    std::string privateName = "secret";
public:
    // One macro enables both access approaches
    GTESTG_FRIEND_ACCESS_PRIVATE();
};
```

Dieses Makro gewährt Friend-Zugriff auf:
- **VirtualAccessor Template** - Wird von TEST_FRIEND und TEST_G_FRIEND verwendet
- **gtestg_private_accessMember Funktion** - Wird von GTESTG_PRIVATE_MEMBER Makros verwendet

#### Ansatz 1: Verwendung von TEST_FRIEND und TEST_G_FRIEND (Empfohlen)

Für einfache Fälle verwenden Sie `TEST_FRIEND` oder `TEST_G_FRIEND`, um Tests zu erstellen, die direkt auf private Member zugreifen können:

**Beispiel mit TEST_FRIEND:**
```cpp
struct WidgetTest : ::testing::Test {
    Widget w;
};

TEST_FRIEND(WidgetTest, AccessPrivate) {
    // Direct access to private members (via VirtualAccessor specialization)
    EXPECT_EQ(w.secret_, 42);
    w.secret_ = 100;
    EXPECT_EQ(w.secret_, 100);
}
```

**Beispiel mit TEST_G_FRIEND:**
```cpp
struct WidgetGenTest : ::gtest_generator::TestWithGenerator {
    Widget w{999};
};

TEST_G_FRIEND(WidgetGenTest, GeneratorTest) {
    int factor = GENERATOR(1, 2, 5);
    USE_GENERATOR();

    // Direct access works in parameterized tests too
    EXPECT_EQ(w.secret_, 999);
    printf("factor=%d, secret=%d\n", factor, w.secret_);
}
```

**Multi-Datei-Unterstützung:**
`TEST_FRIEND` und `TEST_G_FRIEND` funktionieren korrekt, wenn Tests in mehreren .cpp-Dateien definiert sind, die zur selben ausführbaren Datei gelinkt werden. Siehe `test_friend_multi_file1.cpp` und `test_friend_multi_file2.cpp` für Beispiele.

**Wie es funktioniert:**
- `TEST_FRIEND` und `TEST_G_FRIEND` erstellen eine explizite Template-Spezialisierung von `VirtualAccessor<Suite, TestName>` innerhalb des `gtestg_detail` Namensraums
- Diese Spezialisierung erhält Friend-Zugriff über `GTESTG_FRIEND_ACCESS_PRIVATE()`
- Da `VirtualAccessor` ein Friend ist und von Ihrer Test-Fixture erbt, kann er auf private Member der Zielklasse zugreifen
- Der Testkörper wird im Kontext dieser Friend-Klasse ausgeführt, was direkten Zugriff auf private Member ermöglicht
- Jeder Test erhält einen eindeutigen Tag-Typ, um eine separate Spezialisierung zu erstellen und Namenskonflikte zu vermeiden

#### Ansatz 2: Verwendung von GTESTG_PRIVATE_MEMBER Makros (Explizite Kontrolle)

Für mehr Kontrolle oder bei der Arbeit mit regulären `TEST_F`/`TEST_G` Makros verwenden Sie die funktionsbasierten Accessor-Makros. Dieser Ansatz erfordert die Deklaration des Zugriffs für jedes Member, auf das Sie zugreifen möchten.

**Schritt 1: Deklarieren Sie den Zugriff außerhalb Ihrer Klasse (in der Testdatei):**
```cpp
// Declare which members you want to access
GTESTG_PRIVATE_DECLARE_MEMBER(Widget, secret_);
GTESTG_PRIVATE_DECLARE_MEMBER(Widget, privateName);
```

**Schritt 2: Greifen Sie auf Member in Ihren Tests zu:**
```cpp
TEST_FRIEND(WidgetTest, AccessPrivate) {
    // Access using the macro
    int& secret = GTESTG_PRIVATE_MEMBER(Widget, secret_, &w);
    EXPECT_EQ(secret, 42);
    secret = 100;
    EXPECT_EQ(secret, 100);
}
```

Dieser Ansatz ist nützlich, wenn:
- Sie explizite Dokumentation darüber wünschen, auf welche Member zugegriffen wird
- Sie auf statische Member zugreifen müssen
- Sie benutzerdefinierte Accessor-Funktionen mit zusätzlicher Logik wünschen

#### Kombination beider Ansätze

Sie können beide Ansätze im selben Test verwenden:

```cpp
class Widget {
private:
    int secret_ = 42;
    static int counter_;
public:
    GTESTG_FRIEND_ACCESS_PRIVATE();  // Enables both approaches
};

int Widget::counter_ = 0;

// Declare access for static member
GTESTG_PRIVATE_DECLARE_STATIC(Widget, counter_);

TEST_G_FRIEND(WidgetTest, CombinedAccess) {
    int value = GENERATOR(10, 20);
    USE_GENERATOR();

    Widget w;

    // Approach 1: Direct access to instance member
    w.secret_ = value;
    EXPECT_EQ(w.secret_, value);

    // Approach 2: Use macro for static member
    int& count = GTESTG_PRIVATE_STATIC(Widget, counter_);
    count++;
}
```

### Vollständige API-Referenz für GTESTG_PRIVATE_MEMBER Makros

Dieser Abschnitt bietet eine detaillierte Referenz für die funktionsbasierten Makros für den Zugriff auf private Member (Ansatz 2).

#### Deklaration des Zugriffs auf Member

Platzieren Sie diese Deklarationen **außerhalb** Ihrer Klasse, typischerweise in Ihrer Testdatei. Diese Deklarationen teilen dem System mit, auf welche privaten Member Sie zugreifen möchten:

| Makro | Zweck | Parameter | Beispiel |
|-------|-------|-----------|----------|
| `GTESTG_PRIVATE_DECLARE_MEMBER` | Deklariert Zugriff auf Instanz-Member | Target, MemberName | `GTESTG_PRIVATE_DECLARE_MEMBER(MyClass, privateField)` |
| `GTESTG_PRIVATE_DECLARE_STATIC` | Deklariert Zugriff auf statische Member | Target, MemberName | `GTESTG_PRIVATE_DECLARE_STATIC(MyClass, staticCounter)` |
| `GTESTG_PRIVATE_DECLARE_FUNCTION` | Deklariert benutzerdefinierte Accessor-Funktion | ThisClass, Target, FuncName | `GTESTG_PRIVATE_DECLARE_FUNCTION(MyTest, MyClass, GetSum)` |

#### Makros für den Zugriff auf Member

Verwenden Sie diese Makros **innerhalb** Ihrer Testfunktionen, um auf private Member zuzugreifen:

| Makro | Zweck | Parameter | Beispiel |
|-------|-------|-----------|----------|
| `GTESTG_PRIVATE_MEMBER` | Zugriff auf Instanz-Member | Target, MemberName, &obj | `GTESTG_PRIVATE_MEMBER(MyClass, privateField, &obj)` |
| `GTESTG_PRIVATE_STATIC` | Zugriff auf statische Member | Target, MemberName | `GTESTG_PRIVATE_STATIC(MyClass, staticCounter)` |
| `GTESTG_PRIVATE_CALL` | Ruft benutzerdefinierte Funktion mit explizitem Test-Objekt auf | Target, FuncName, test_obj, &obj | `GTESTG_PRIVATE_CALL(MyClass, GetSum, this, &obj)` |
| `GTESTG_PRIVATE_CALL_ON_TEST` | Ruft benutzerdefinierte Funktion auf (verwendet implizites 'this') | ThisClass, Target, FuncName, &obj | `GTESTG_PRIVATE_CALL_ON_TEST(MyTest, MyClass, GetSum, &obj)` |

#### Detaillierte Verwendungsbeispiele

Die folgenden Beispiele demonstrieren umfassende Verwendungsmuster für die GTESTG_PRIVATE_* Makros.

**Beispiel 1: Zugriff auf Instanz-Member**
```cpp
class MyClass {
private:
    int privateValue = 42;
    std::string privateName = "secret";
public:
    GTESTG_FRIEND_ACCESS_PRIVATE();
};

// Declare access (in test file)
GTESTG_PRIVATE_DECLARE_MEMBER(MyClass, privateValue);
GTESTG_PRIVATE_DECLARE_MEMBER(MyClass, privateName);

TEST_G(MyTest, AccessPrivate) {
    int value = GENERATOR(10, 20, 30);
    USE_GENERATOR();

    MyClass obj;

    // Access and modify private members
    int& val = GTESTG_PRIVATE_MEMBER(MyClass, privateValue, &obj);
    EXPECT_EQ(val, 42);
    val = value;  // Can modify
    EXPECT_EQ(val, value);

    std::string& name = GTESTG_PRIVATE_MEMBER(MyClass, privateName, &obj);
    EXPECT_EQ(name, "secret");
    name = "modified";
}
```

**Beispiel 2: Zugriff auf statische Member**
```cpp
class MyClass {
private:
    static int staticCounter;
public:
    GTESTG_FRIEND_ACCESS_PRIVATE();
};

int MyClass::staticCounter = 100;

// Declare access to static member
GTESTG_PRIVATE_DECLARE_STATIC(MyClass, staticCounter);

TEST_G(MyTest, AccessStatic) {
    USE_GENERATOR();

    // Access static member (no object needed)
    int& count = GTESTG_PRIVATE_STATIC(MyClass, staticCounter);
    EXPECT_EQ(count, 100);
    count = 200;  // Can modify
    EXPECT_EQ(count, 200);
}
```

**Beispiel 3: Benutzerdefinierte Accessor-Funktionen**
```cpp
class MyClass {
private:
    int field1 = 10;
    int field2 = 20;
public:
    GTESTG_FRIEND_ACCESS_PRIVATE();
};

class MyTest : public ::gtest_generator::TestWithGenerator {};

// Declare custom function with access to both test context and private members
// THIS = test object, TARGET = object being accessed
GTESTG_PRIVATE_DECLARE_FUNCTION(MyTest, MyClass, GetSum) {
    // Can access test context: THIS->GetParam()
    // Can access private members: TARGET->field1, TARGET->field2
    return TARGET->field1 + TARGET->field2;
}

TEST_G(MyTest, CustomFunction) {
    int multiplier = GENERATOR(1, 2, 3);
    USE_GENERATOR();

    MyClass obj;

    // Call custom function using CALL_ON_TEST (uses implicit 'this')
    int sum = GTESTG_PRIVATE_CALL_ON_TEST(MyTest, MyClass, GetSum, &obj);
    EXPECT_EQ(sum, 30);  // 10 + 20

    // Alternative: Pass test object explicitly
    int sum2 = GTESTG_PRIVATE_CALL(MyClass, GetSum, static_cast<MyTest*>(this), &obj);
    EXPECT_EQ(sum2, 30);
}
```

Für vollständige Beispiele siehe `test_private_access.cpp` und `test_define_macros.cpp`.

## Array-Vergleichsmakros

Die Bibliothek bietet praktische Makros zum elementweisen Vergleich von Arrays mit detaillierten Fehlermeldungen. Diese Makros basieren auf den Assertion-Makros von Google Test.

### Schnellbeispiel

```cpp
TEST_G(ArrayTest, CompareArrays) {
    USE_GENERATOR();

    int expected[] = {1, 2, 3, 4, 5};
    int actual[] = {1, 2, 3, 4, 5};

    EXPECT_ARRAY_EQ(expected, actual, 5);  // Non-fatal assertion
}
```

### Verfügbare Makros

#### Integer- und generische Typen

- **`EXPECT_ARRAY_EQ(expected, actual, size)`** - Non-fatal: Vergleicht zwei Arrays elementweise
  ```cpp
  int expected[] = {1, 2, 3};
  int actual[] = {1, 2, 3};
  EXPECT_ARRAY_EQ(expected, actual, 3);
  ```

- **`ASSERT_ARRAY_EQ(expected, actual, size)`** - Fatal: Vergleicht zwei Arrays elementweise
  ```cpp
  std::vector<int> expected = {10, 20, 30};
  std::vector<int> actual = {10, 20, 30};
  ASSERT_ARRAY_EQ(expected.data(), actual.data(), 3);  // Test stops if fails
  ```

#### Gleitkomma-Typen

- **`EXPECT_ARRAY_NEAR(expected, actual, size, abs_error)`** - Non-fatal: Vergleicht Gleitkomma-Arrays mit Toleranz
  ```cpp
  double expected[] = {1.0, 2.0, 3.0};
  double actual[] = {1.001, 1.999, 3.002};
  EXPECT_ARRAY_NEAR(expected, actual, 3, 0.01);  // Tolerance: 0.01
  ```

- **`ASSERT_ARRAY_NEAR(expected, actual, size, abs_error)`** - Fatal: Vergleicht Gleitkomma-Arrays mit Toleranz
  ```cpp
  float expected[] = {1.5f, 2.5f, 3.5f};
  float actual[] = {1.501f, 2.499f, 3.502f};
  ASSERT_ARRAY_NEAR(expected, actual, 3, 0.01f);
  ```

- **`EXPECT_ARRAY_DOUBLE_EQ(expected, actual, size)`** - Non-fatal: Vergleicht Double-Arrays mit Standard-Toleranz
  ```cpp
  double expected[] = {1.5, 2.5, 3.5};
  double actual[] = {1.5, 2.5, 3.5};
  EXPECT_ARRAY_DOUBLE_EQ(expected, actual, 3);
  ```

- **`EXPECT_ARRAY_FLOAT_EQ(expected, actual, size)`** - Non-fatal: Vergleicht Float-Arrays mit Standard-Toleranz
  ```cpp
  float expected[] = {1.25f, 2.25f, 3.25f};
  float actual[] = {1.25f, 2.25f, 3.25f};
  EXPECT_ARRAY_FLOAT_EQ(expected, actual, 3);
  ```

### Fehlermeldungen

Wenn sich Arrays unterscheiden, liefern die Makros detaillierte Fehlermeldungen:

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

### Arbeiten mit verschiedenen Container-Typen

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

### Kombination mit GENERATOR

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

### Hauptmerkmale

- **Elementweiser Vergleich**: Jedes Element wird einzeln verglichen
- **Detaillierte Fehlermeldungen**: Zeigt an, welcher Index sich unterscheidet und welche Werte vorliegen
- **Funktioniert mit jedem vergleichbaren Typ**: int, float, double, string, benutzerdefinierte Typen mit operator==
- **Erfolgsmeldungen**: Zeigt "Arrays are equal" wenn alle Elemente übereinstimmen
- **Kompatibel mit Vektoren und Arrays**: Funktioniert mit C-Style-Arrays, std::vector, std::array

### Wichtige Hinweise

1. **Größenparameter ist erforderlich**: Sie müssen die Array-Größe explizit angeben
2. **Fatal vs Non-fatal**: Verwenden Sie ASSERT_* für fatale Assertions, EXPECT_* für non-fatale
3. **Gleitkomma-Vergleiche**: Verwenden Sie NEAR, FLOAT_EQ oder DOUBLE_EQ für Gleitkommawerte
4. **Benutzerdefinierte Typen**: Ihr Typ muss operator== für EXPECT_ARRAY_EQ definiert haben
5. **Null-Größe-Arrays**: Funktioniert korrekt mit leeren Arrays (Größe = 0)

Siehe `test_array_compare.cpp` für vollständige Beispiele.

## Zukünftige Verbesserungen

- Dynamische Berechnung der Gesamtanzahl von Kombinationen
- Unterstützung für verschiedene Datentypen in Generatoren
- Benannte Testinstanziierungen
- Unterstützung für komplexere Wertmuster

## Lizenz

Dieses Projekt wird zu Bildungs- und Entwicklungszwecken bereitgestellt.
