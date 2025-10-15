# GTest Generator - Libreria per Test Parametrizzati

Una semplice libreria header-only che fornisce una sintassi in stile Catch2 `GENERATE` per i test parametrizzati di Google Test.

## Caratteristiche Principali

- Sintassi semplice e intuitiva simile a `GENERATE` di Catch2
- Implementazione header-only
- Generazione automatica di tutte le combinazioni di test
- Compatibile con le macro standard `TEST_P` di Google Test

## Avvio Rapido

```cpp
#include "gtest_generator.h"

// Definire la fixture di test usando TestWithGenerator
class MyTest : public ::gtest_generator::TestWithGenerator {
};

// Scrivere test parametrizzato usando la macro TEST_G
TEST_G(MyTest, SimpleCase) {
    // IMPORTANTE: Tutte le chiamate GENERATOR() devono venire PRIMA di USE_GENERATOR()
    int a = GENERATOR(1, 2);      // Sarà 1 o 2
    int b = GENERATOR(10, 20);    // Sarà 10 o 20
    USE_GENERATOR();              // Deve essere chiamato DOPO tutte le chiamate GENERATOR()

    EXPECT_LT(a, b);
    printf("Test: a=%d, b=%d\n", a, b);
}
// TEST_G gestisce automaticamente la generazione dei test - non serve ENABLE_GENERATOR!
```

## Installazione

Questa è una libreria header-only. Semplicemente copia `gtest_generator.h` nel tuo progetto e includilo.

## Compilare il Progetto

### Prerequisiti

- CMake 3.14 o superiore
- Libreria Google Test
- Compilatore compatibile con C++14

### Istruzioni di Compilazione

```bash
# Creare la directory di build
mkdir build && cd build

# Configurare con CMake
cmake ..

# Compilare il progetto
cmake --build .

# Eseguire i test
./mygen_test
```

## Altri Esempi

### Valori Multipli

```cpp
TEST_G(MyTest, MultipleValues) {
    int x = GENERATOR(1, 2, 3);       // 3 valori
    int y = GENERATOR(10, 20, 30);    // 3 valori
    USE_GENERATOR();
    
    // Genera 9 combinazioni di test
    EXPECT_LT(x, y);
}
```

### Numero Diverso di Valori

```cpp
TEST_G(MyTest, AsymmetricCombinations) {
    int size = GENERATOR(1, 2, 3);     // 3 valori
    int scale = GENERATOR(10, 100);    // 2 valori
    USE_GENERATOR();
    
    // Genera 6 combinazioni di test
    int result = size * scale;
    EXPECT_GT(result, 0);
}
```

## Esempi Avanzati

### Lavorare con Classi e Oggetti

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
    // Generare oggetti completi direttamente
    auto obj1 = GENERATOR(TestObject(1, "primo"), TestObject(2, "secondo"));
    auto obj2 = GENERATOR(TestObject(10, "dieci"), TestObject(20, "venti"));
    USE_GENERATOR();
    
    EXPECT_LT(obj1, obj2);
    printf("Oggetti: obj1={%d, %s}, obj2={%d, %s}\n", 
           obj1.value, obj1.name.c_str(), 
           obj2.value, obj2.name.c_str());
}
```

### Lavorare con Container STL

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
    printf("Vettore: dimensione=%d, moltiplicatore=%d\n", size, multiplier);
}
```

## Modalità di Campionamento

La libreria supporta due modalità di campionamento per generare combinazioni di test:

### Modalità FULL (Predefinita - Prodotto Cartesiano)
La modalità predefinita genera tutte le possibili combinazioni di valori (prodotto cartesiano). Questo è il comportamento tradizionale che garantisce una copertura completa dei test.

```cpp
TEST_G(MyTest, FullMode) {
    USE_GENERATOR();  // Il predefinito è modalità FULL
    // o esplicitamente: USE_GENERATOR(FULL);
    
    auto x = GENERATOR(1, 2);      // 2 valori
    auto y = GENERATOR(10, 20);    // 2 valori
    auto z = GENERATOR(100, 200);  // 2 valori
    
    // Genera 8 esecuzioni di test: 2 × 2 × 2 = 8 combinazioni
    // (1,10,100), (1,10,200), (1,20,100), (1,20,200),
    // (2,10,100), (2,10,200), (2,20,100), (2,20,200)
}
```

### Modalità ALIGNED (Iterazione Parallela)
La modalità ALIGNED itera attraverso tutte le colonne in parallelo, come una cerniera. Ogni colonna avanza al valore successivo ad ogni esecuzione, avvolgendo quando raggiunge la fine. Il numero totale di esecuzioni è uguale alla dimensione della colonna più grande.

```cpp
TEST_G(MyTest, AlignedMode) {
    USE_GENERATOR(ALIGNED);  // Abilita modalità ALIGNED
    
    auto x = GENERATOR(1, 2);           // 2 valori
    auto y = GENERATOR(10, 20, 30, 40); // 4 valori  
    auto z = GENERATOR(100, 200, 300);  // 3 valori
    
    // Genera 4 esecuzioni di test (dimensione massima colonna):
    // Esecuzione 0: (1, 10, 100)  - tutti all'indice 0
    // Esecuzione 1: (2, 20, 200)  - tutti all'indice 1
    // Esecuzione 2: (1, 30, 300)  - x si avvolge a 0, altri all'indice 2
    // Esecuzione 3: (2, 40, 100)  - x a 1, y a 3, z si avvolge a 0
}
```

#### Caratteristiche Chiave della Modalità ALIGNED:
- **Deterministica**: I valori sono selezionati in ordine (0, 1, 2, ...) con avvolgimento
- **Ordine di Dichiarazione**: Le colonne sono elaborate nell'ordine in cui sono dichiarate
- **Meno Esecuzioni**: Esecuzioni totali = dimensione massima colonna (non il prodotto)
- **Copertura Bilanciata**: Ogni valore in ogni colonna è utilizzato approssimativamente in modo uguale

#### Dettagli di Implementazione:
La libreria implementa la modalità ALIGNED in modo efficiente utilizzando il meccanismo `GTEST_SKIP()` di Google Test:
1. Durante la registrazione dei test, la libreria conta la dimensione di ogni colonna GENERATOR
2. Al momento dell'esecuzione, la libreria calcola la dimensione massima della colonna
3. Le esecuzioni di test oltre la dimensione massima vengono saltate utilizzando `GTEST_SKIP()`
4. Ogni colonna si avvolge quando raggiunge la fine (utilizzando l'operatore modulo)

Questo approccio garantisce che:
- Vengano eseguite solo le esecuzioni di test necessarie (non tutti i prodotti cartesiani)
- Il conteggio dei test riportato da Google Test riflette i test effettivamente eseguiti
- Non ci siano penalità di prestazioni dalla generazione di combinazioni non necessarie

#### Quando Usare Ogni Modalità:
- **Modalità FULL**: Quando serve un test esaustivo di tutte le combinazioni
- **Modalità ALIGNED**: Quando si desidera un campionamento rappresentativo con meno esecuzioni di test

#### Esempio di Confronto:
```cpp
// Modalità FULL: 3 × 2 × 2 = 12 esecuzioni
TEST_G(MyTest, FullExample) {
    USE_GENERATOR(FULL);
    auto a = GENERATOR(1, 2, 3);
    auto b = GENERATOR(10, 20);
    auto c = GENERATOR(100, 200);
    // Genera tutte e 12 le combinazioni
}

// Modalità ALIGNED: max(3, 2, 2) = 3 esecuzioni
TEST_G(MyTest, AlignedExample) {
    USE_GENERATOR(ALIGNED);
    auto a = GENERATOR(1, 2, 3);
    auto b = GENERATOR(10, 20);
    auto c = GENERATOR(100, 200);
    // Genera solo 3 combinazioni:
    // (1, 10, 100), (2, 20, 200), (3, 10, 100)
}
```

#### Esempio di Test di Verifica:
Il progetto include `test_mode_counts.cpp` che verifica in modo completo che entrambe le modalità funzionino correttamente:

```cpp
// Test con configurazione 3x2x2
TEST_G(ModeCountTest, Full_3x2x2) {
    int a = GENERATOR(1, 2, 3);        // 3 valori
    int b = GENERATOR(10, 20);         // 2 valori
    int c = GENERATOR(100, 200);       // 2 valori
    USE_GENERATOR(FULL);

    // Verifica: genera esattamente 12 casi di test (3 × 2 × 2)
    // Tutte le combinazioni sono uniche
}

TEST_G(ModeCountTest, Aligned_3x2x2) {
    int a = GENERATOR(1, 2, 3);        // 3 valori
    int b = GENERATOR(10, 20);         // 2 valori
    int c = GENERATOR(100, 200);       // 2 valori
    USE_GENERATOR(ALIGNED);

    // Verifica: genera esattamente 3 casi di test (dimensione massima colonna)
    // Risultati: (1,10,100), (2,20,200), (3,10,100)
}
```

Eseguire il test di verifica per vedere l'output dettagliato:
```bash
./build/test_mode_counts
```

L'output atteso mostra:
- Modalità FULL 3x2x2: 12 esecuzioni con tutte le combinazioni uniche
- Modalità ALIGNED 3x2x2: 3 esecuzioni (dimensione massima colonna)
- Modalità FULL 2x3x4: 24 esecuzioni con tutte le combinazioni uniche
- Modalità ALIGNED 2x3x4: 4 esecuzioni (dimensione massima colonna)
- Modalità FULL 5x1x3: 15 esecuzioni con tutte le combinazioni uniche
- Modalità ALIGNED 5x1x3: 5 esecuzioni (dimensione massima colonna)

## Riferimento API

### Macro

- **`TEST_G(TestClassName, TestName)`** - Definisce un test parametrizzato con generazione automatica di test. Sostituisce sia TEST_P che la necessità di chiamate ENABLE_GENERATOR separate.

- **`GENERATOR(...)`** - Definisce i valori per un parametro di test. Ogni chiamata crea una nuova dimensione di combinazioni di test.
  ```cpp
  int value = GENERATOR(1, 2, 3);  // Crea 3 varianti
  ```
  **IMPORTANTE**: Tutte le chiamate GENERATOR() devono venire PRIMA di USE_GENERATOR()

- **`USE_GENERATOR()`** - Deve essere chiamato una volta in ogni TEST_G, DOPO tutte le chiamate GENERATOR(). Usa la modalità FULL per impostazione predefinita.

- **`USE_GENERATOR(mode)`** - Deve essere chiamato una volta in ogni TEST_G, DOPO tutte le chiamate GENERATOR(). Specifica la modalità di campionamento:
  - `USE_GENERATOR(FULL)` - Prodotto cartesiano di tutti i valori (uguale al predefinito)
  - `USE_GENERATOR(ALIGNED)` - Iterazione parallela attraverso le colonne

## Come Funziona

La libreria usa variabili statiche thread-local per mantenere lo stato del generatore tra le istanziazioni dei test. Quando `GENERATOR` viene chiamato:

1. Registra i valori possibili per quel generatore
2. Basandosi sull'indice del parametro di test corrente, calcola quale valore restituire
3. Tutte le combinazioni vengono generate automaticamente

## Note Importanti sull'Uso

### L'Ordine Corretto è Critico

✅ **CORRETTO**:
```cpp
TEST_G(MyTest, Example) {
    int a = GENERATOR(1, 2);     // Prima: Definire i generatori
    int b = GENERATOR(10, 20);   // 
    USE_GENERATOR();             // Poi: Chiamare USE_GENERATOR()
    // Logica del test qui
}
```

❌ **SBAGLIATO**:
```cpp
TEST_G(MyTest, Example) {
    USE_GENERATOR();             // Sbagliato! Questo deve venire dopo le chiamate GENERATOR
    int a = GENERATOR(1, 2);     
    int b = GENERATOR(10, 20);   
    // Logica del test qui
}
```

## Output di Esempio

```
[==========] Esecuzione di 4 test da 1 suite di test.
[----------] 4 test da Generator/MyTest__SimpleCase
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
[==========] 4 test da 1 suite di test eseguiti. (0 ms totali)
[  PASSATI  ] 4 test.
```

## Limitazioni

- La fixture di test deve ereditare da `::gtest_generator::TestWithGenerator`
- Deve usare la macro `TEST_G` invece del `TEST_P` standard
- Tutte le chiamate `GENERATOR()` devono venire prima di `USE_GENERATOR()`
- I tipi complessi (oggetti, puntatori) funzionano con GENERATOR ma potrebbero richiedere un'istanziazione template appropriata
- La gestione della memoria è responsabilità dell'utente quando si generano puntatori con `new`

## Accesso ai Membri Privati per il Testing

La libreria fornisce un modo type-safe per accedere ai membri privati nei test senza usare `#define private public` o modificare il codice di produzione.

### Esempio Rapido

```cpp
// Nella tua classe di produzione
class MyClass {
private:
    int privateValue;
    std::string privateName;
public:
    MyClass(int v, const std::string& n) : privateValue(v), privateName(n) {}

    // Concedi accesso friend per il testing
    GTESTG_FRIEND_ACCESS_PRIVATE();
};

// Nel tuo file di test


// Dichiarare gli accessor - passare solo il nome del campo
GTESTG_PRIVATE_DECLARE_MEMBER(MyClass, privateValue);
GTESTG_PRIVATE_DECLARE_MEMBER(MyClass, privateName);

TEST_G(MyTest, AccessPrivate) {
    int value = GENERATOR(10, 20);
    USE_GENERATOR();

    MyClass obj(value, "test");

    // Accedere e modificare i membri privati
    int& privateRef = GTESTG_PRIVATE_MEMBER(MyClass, privateValue, &obj);
    EXPECT_EQ(privateRef, value);
    privateRef = 100;
    EXPECT_EQ(privateRef, 100);
}
```

### Caratteristiche Principali

- **Type-safe**: Usa specializzazione template e dichiarazioni friend
- **Zero overhead**: Meccanismo completamente compile-time
- **Sicuro per la produzione**: `GTESTG_FRIEND_ACCESS_PRIVATE()` può essere definito come macro vuota nelle build di produzione
- **Condivisibile**: Il blocco di dichiarazione (linee 260-274 in `gtest_generator.h`) può essere copiato negli header comuni


**Membri Statici:**
```cpp
GTESTG_PRIVATE_DECLARE_STATIC(MyClass, staticCounter)
```

**사용자 정의 함수:**
```cpp
GTESTG_PRIVATE_DECLARE_FUNCTION(MyTest, MyClass, CustomAccess) {
    return target->privateField1 + target->privateField2;
};
```

**Funzioni Accessor Personalizzate:**
```cpp
GTESTG_PRIVATE_DECLARE_FUNCTION(MyTest, MyClass, CustomAccess) {
    return target->privateField1 + target->privateField2;
}
```

Vedere `test_private_access.cpp` e `example_common_header.h` per esempi completi.

## Macro di Confronto Array

La libreria fornisce macro convenienti per confrontare array elemento per elemento con messaggi di errore dettagliati. Queste macro sono costruite sulle macro di asserzione di Google Test.

### Esempio Rapido

```cpp
TEST_G(ArrayTest, CompareArrays) {
    USE_GENERATOR();

    int expected[] = {1, 2, 3, 4, 5};
    int actual[] = {1, 2, 3, 4, 5};

    EXPECT_ARRAY_EQ(expected, actual, 5);  // Non-fatal assertion
}
```

### Macro Disponibili

#### Tipi Interi e Generici

- **`EXPECT_ARRAY_EQ(expected, actual, size)`** - Non-fatale: Confronta due array elemento per elemento
  ```cpp
  int expected[] = {1, 2, 3};
  int actual[] = {1, 2, 3};
  EXPECT_ARRAY_EQ(expected, actual, 3);
  ```

- **`ASSERT_ARRAY_EQ(expected, actual, size)`** - Fatale: Confronta due array elemento per elemento
  ```cpp
  std::vector<int> expected = {10, 20, 30};
  std::vector<int> actual = {10, 20, 30};
  ASSERT_ARRAY_EQ(expected.data(), actual.data(), 3);  // Test stops if fails
  ```

#### Tipi a Virgola Mobile

- **`EXPECT_ARRAY_NEAR(expected, actual, size, abs_error)`** - Non-fatale: Confronta array a virgola mobile con tolleranza
  ```cpp
  double expected[] = {1.0, 2.0, 3.0};
  double actual[] = {1.001, 1.999, 3.002};
  EXPECT_ARRAY_NEAR(expected, actual, 3, 0.01);  // Tolerance: 0.01
  ```

- **`ASSERT_ARRAY_NEAR(expected, actual, size, abs_error)`** - Fatale: Confronta array a virgola mobile con tolleranza
  ```cpp
  float expected[] = {1.5f, 2.5f, 3.5f};
  float actual[] = {1.501f, 2.499f, 3.502f};
  ASSERT_ARRAY_NEAR(expected, actual, 3, 0.01f);
  ```

- **`EXPECT_ARRAY_DOUBLE_EQ(expected, actual, size)`** - Non-fatale: Confronta array di double con tolleranza predefinita
  ```cpp
  double expected[] = {1.5, 2.5, 3.5};
  double actual[] = {1.5, 2.5, 3.5};
  EXPECT_ARRAY_DOUBLE_EQ(expected, actual, 3);
  ```

- **`EXPECT_ARRAY_FLOAT_EQ(expected, actual, size)`** - Non-fatale: Confronta array di float con tolleranza predefinita
  ```cpp
  float expected[] = {1.25f, 2.25f, 3.25f};
  float actual[] = {1.25f, 2.25f, 3.25f};
  EXPECT_ARRAY_FLOAT_EQ(expected, actual, 3);
  ```

### Messaggi di Errore

Quando gli array differiscono, le macro forniscono messaggi di errore dettagliati:

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

### Lavorare con Diversi Tipi di Container

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

### Combinare con GENERATOR

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

### Caratteristiche Chiave

- **Confronto elemento per elemento**: Ogni elemento è confrontato individualmente
- **Messaggi di errore dettagliati**: Mostra quale indice differisce e i valori
- **Funziona con qualsiasi tipo confrontabile**: int, float, double, string, tipi personalizzati con operator==
- **Messaggi di successo**: Mostra "Arrays are equal" quando tutti gli elementi corrispondono
- **Compatibile con vettori e array**: Funziona con array C-style, std::vector, std::array

