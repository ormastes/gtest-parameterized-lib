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

## Licenza

Questo progetto è fornito così com'è per scopi educativi e di sviluppo.