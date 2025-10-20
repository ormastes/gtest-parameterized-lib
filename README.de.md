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

## Erweiterte Beispiele

### Arbeiten mit Klassen und Objekten

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

### Arbeiten mit STL-Containern

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
    printf("Vektor: Größe=%d, Multiplikator=%d\n", size, multiplier);
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
