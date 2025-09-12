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
    // WICHTIG: Alle GENERATOR() Aufrufe müssen VOR USE_GENERATOR() kommen
    int a = GENERATOR(1, 2);      // Wird 1 oder 2 sein
    int b = GENERATOR(10, 20);    // Wird 10 oder 20 sein
    USE_GENERATOR();              // Muss NACH allen GENERATOR() Aufrufen aufgerufen werden

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

### Die richtige Reihenfolge ist entscheidend

✅ **RICHTIG**:
```cpp
TEST_G(MyTest, Example) {
    int a = GENERATOR(1, 2);     // Zuerst: Generatoren definieren
    int b = GENERATOR(10, 20);   // 
    USE_GENERATOR();             // Dann: USE_GENERATOR() aufrufen
    // Test-Logik hier
}
```

❌ **FALSCH**:
```cpp
TEST_G(MyTest, Example) {
    USE_GENERATOR();             // Falsch! Dies muss nach GENERATOR Aufrufen kommen
    int a = GENERATOR(1, 2);     
    int b = GENERATOR(10, 20);   
    // Test-Logik hier
}
```

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

## Lizenz

Dieses Projekt wird für Bildungs- und Entwicklungszwecke so wie es ist bereitgestellt.