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
    // ⚠️ CRITICO: Le chiamate GENERATOR() DEVONO essere IN CIMA, PRIMA di USE_GENERATOR()
    // Questo ordine è obbligatorio - la libreria conta i generatori prima di eseguire il test
    int a = GENERATOR(1, 2);      // Sarà 1 o 2
    int b = GENERATOR(10, 20);    // Sarà 10 o 20
    USE_GENERATOR();              // DEVE essere chiamato DOPO tutte le chiamate GENERATOR()

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

### Utilizzo di Valori Generati nelle Espressioni
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

### Logica di Test Complessa
```cpp
TEST_G(MyTest, ConditionalLogic) {
    int mode = GENERATOR(0, 1, 2);     // 3 modalità
    int value = GENERATOR(100, 200);   // 2 valori
    USE_GENERATOR();

    int result;
    switch (mode) {
        case 0: result = value + 10; break;
        case 1: result = value * 2; break;
        case 2: result = value - 50; break;
    }

    EXPECT_GT(result, 0);
    printf("Modalità %d con valore %d produce risultato %d\n", mode, value, result);
}
```

## Esempi Avanzati

### Lavorare con Classi e Oggetti

#### Generazione Diretta di Oggetti
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

#### Uso di GENERATOR negli Argomenti del Costruttore
```cpp
TEST_G(MyTest, ConstructorWithGenerators) {
    // Valori GENERATOR usati come argomenti del costruttore
    int val1 = GENERATOR(1, 2);
    int val2 = GENERATOR(10, 20);
    USE_GENERATOR();

    TestObject objects[] = {
        TestObject(val1, "test"),
        TestObject(val2, "demo")
    };

    EXPECT_LT(objects[0].value, objects[1].value);
    printf("Oggetti array: [0]={%d,%s}, [1]={%d,%s}\n",
           objects[0].value, objects[0].name.c_str(),
           objects[1].value, objects[1].name.c_str());
}
```

### Lavorare con Puntatori e Memoria Dinamica

#### Generazione di Puntatori a Oggetti
```cpp
TEST_G(MyTest, PointerGeneration) {
    // Generare puntatori a oggetti diversi
    // Nota: Prestare attenzione alla gestione della memoria
    auto* ptr1 = GENERATOR(new TestObject(1, "primo"),
                          new TestObject(2, "secondo"));
    auto* ptr2 = GENERATOR(new TestObject(10, "dieci"),
                          new TestObject(20, "venti"));
    USE_GENERATOR();

    EXPECT_LT(*ptr1, *ptr2);
    printf("Puntatori: ptr1={%d, %s}, ptr2={%d, %s}\n",
           ptr1->value, ptr1->name.c_str(),
           ptr2->value, ptr2->name.c_str());

    // Pulizia
    delete ptr1;
    delete ptr2;
}
```

#### Chiamate GENERATOR Annidate (Avanzato)
```cpp
TEST_G(MyTest, NestedGenerators) {
    // Generazione annidata complessa - ogni GENERATOR esterno contiene chiamate GENERATOR interne
    int inner1 = GENERATOR(1, 2);
    int inner2 = GENERATOR(3, 4);
    int inner3 = GENERATOR(10, 20);
    int inner4 = GENERATOR(30, 40);
    USE_GENERATOR();

    auto* obj1 = new TestObject(inner1, "primo");
    auto* obj2 = new TestObject(inner3, "secondo");

    EXPECT_LT(obj1->value, obj2->value);
    printf("Annidato: obj1={%d}, obj2={%d}\n", obj1->value, obj2->value);

    delete obj1;
    delete obj2;
}
```

### Lavorare con Contenitori STL

#### Generazione di Dimensioni e Contenuti dei Contenitori
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

    printf("Vettore: dimensione=%d, moltiplicatore=%d, elementi=[", size, multiplier);
    for (int v : vec) printf("%d ", v);
    printf("]\n");
}
```

#### Generazione di Combinazioni di Stringhe
```cpp
TEST_G(MyTest, StringCombinations) {
    auto prefix_choice = GENERATOR(0, 1);
    auto suffix_choice = GENERATOR(0, 1);
    auto repeat = GENERATOR(1, 2);
    USE_GENERATOR();

    std::string prefix = prefix_choice ? "Ciao" : "Salve";
    std::string suffix = suffix_choice ? "Mondo" : "Tutti";

    std::string result;
    for (int i = 0; i < repeat; ++i) {
        if (i > 0) result += " ";
        result += prefix + " " + suffix;
    }

    EXPECT_FALSE(result.empty());
    printf("Stringa: prefix='%s', suffix='%s', repeat=%d => '%s'\n",
           prefix.c_str(), suffix.c_str(), repeat, result.c_str());
}
```

### Lavorare con Puntatori Intelligenti

#### Uso di unique_ptr con GENERATOR
```cpp
TEST_G(MyTest, SmartPointers) {
    auto value1 = GENERATOR(1, 2);
    auto value2 = GENERATOR(10, 20);
    USE_GENERATOR();

    auto ptr1 = std::make_unique<TestObject>(value1, "primo");
    auto ptr2 = std::make_unique<TestObject>(value2, "secondo");

    EXPECT_LT(*ptr1, *ptr2);
    printf("Puntatori intelligenti: ptr1={%d, %s}, ptr2={%d, %s}\n",
           ptr1->value, ptr1->name.c_str(),
           ptr2->value, ptr2->name.c_str());
}
```

### Esempi di Strutture Complesse

#### Generazione di Strutture con Campi Multipli
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
    printf("Punti: p1=(%d,%d), p2=(%d,%d), distanza=%d\n",
           p1.x, p1.y, p2.x, p2.y, distance);
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

### ⚠️ CRITICO: Ordine di GENERATOR e USE_GENERATOR

**MOLTO IMPORTANTE:** Tutte le chiamate `GENERATOR()` **DEVONO** essere **IN CIMA** al caso di test, **PRIMA** di `USE_GENERATOR()`. Questo ordine è obbligatorio per il corretto funzionamento della libreria.

✅ **CORRETTO**:
```cpp
TEST_G(MyTest, Example) {
    // Passo 1: TUTTE le chiamate GENERATOR() per prime IN CIMA
    int a = GENERATOR(1, 2);
    int b = GENERATOR(10, 20);

    // Passo 2: USE_GENERATOR() DOPO tutti i generatori
    USE_GENERATOR();

    // Passo 3: La tua logica di test qui
    EXPECT_LT(a, b);
}
```

❌ **SBAGLIATO - NON funzionerà**:
```cpp
TEST_G(MyTest, Example) {
    USE_GENERATOR();             // ❌ SBAGLIATO! Deve venire dopo i generatori
    int a = GENERATOR(1, 2);     // ❌ Troppo tardi!
    int b = GENERATOR(10, 20);
}
```

❌ **SBAGLIATO - NON funzionerà**:
```cpp
TEST_G(MyTest, Example) {
    int a = GENERATOR(1, 2);
    USE_GENERATOR();             // ❌ SBAGLIATO! Deve venire dopo TUTTI i generatori
    int b = GENERATOR(10, 20);   // ❌ Questo generatore è dopo USE_GENERATOR()
}
```

**Perché questo è importante:**
- La libreria conta i generatori durante una fase di pre-esecuzione
- `USE_GENERATOR()` segna la fine della fase di dichiarazione dei generatori
- I generatori dichiarati dopo `USE_GENERATOR()` saranno ignorati o causeranno errori

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

La libreria fornisce un modo semplice e pulito per accedere ai membri privati nei test usando dichiarazioni friend esplicite.

### Esempio Rapido

```cpp
// Nella tua classe di produzione
class MyClass {
private:
    int privateValue;
    std::string privateName;

    int computeSecret(int x) const { return privateValue * x; }

public:
    MyClass(int v, const std::string& n) : privateValue(v), privateName(n) {}

    // Concedi accesso friend a test specifici
    GTESTG_FRIEND_TEST(MyClassTest, AccessPrivateMembers);
    GTESTG_FRIEND_TEST(MyClassTest, ModifyPrivateMembers);
};

// Nel tuo file di test
struct MyClassTest : ::testing::Test {
    MyClass obj{42, "secret"};
};

TEST_FRIEND(MyClassTest, AccessPrivateMembers) {
    // Accesso diretto ai membri privati!
    EXPECT_EQ(obj.privateValue, 42);
    EXPECT_EQ(obj.privateName, "secret");
}

TEST_FRIEND(MyClassTest, ModifyPrivateMembers) {
    // Può modificare i membri privati
    obj.privateValue = 100;
    EXPECT_EQ(obj.privateValue, 100);

    // Può chiamare metodi privati
    int result = obj.computeSecret(2);
    EXPECT_EQ(result, 200);
}
```

### Caratteristiche Principali

- **Semplice e Pulito**: Usa dichiarazioni friend standard di C++
- **Accesso Selettivo**: Concedi l'accesso solo ai test specifici che ne hanno bisogno
- **Zero Overhead**: Meccanismo puro compile-time, nessun costo runtime
- **Type-Safe**: Sicurezza dei tipi garantita dal compilatore
- **Sicuro per la Produzione**: Le dichiarazioni friend non hanno impatto runtime

### Riferimento API

#### Macro per le Classi Target

| Macro | Scopo | Utilizzo |
|-------|---------|-------|
| `GTESTG_FRIEND_TEST(Suite, TestName)` | Concedi accesso friend a un test TEST_F specifico | Posiziona nella definizione della classe |
| `GTESTG_FRIEND_TEST_G(TestClassName, TestName)` | Concedi accesso friend a un test TEST_G | Posiziona nella definizione della classe |
| `GTESTG_FRIEND_TEST_SUITE(Suite)` | Concedi accesso friend a tutti i test in una suite | Posiziona nella definizione della classe |

#### Macro per i File di Test

| Macro | Scopo | Utilizzo |
|-------|---------|-------|
| `TEST_FRIEND(Suite, TestName)` | Definisci un test con accesso friend | Come TEST_F |
| `TEST_G_FRIEND(TestClassName, TestName)` | Definisci un test generatore con accesso friend | Come TEST_G |

### Esempi d'Uso

#### Accesso Privato Base
```cpp
// Classe target
class Widget {
private:
    int secret_ = 42;
public:
    // Concedi accesso a un test specifico
    GTESTG_FRIEND_TEST(WidgetTest, CheckSecret);
};

// File di test
struct WidgetTest : ::testing::Test {
    Widget w;
};

TEST_FRIEND(WidgetTest, CheckSecret) {
    EXPECT_EQ(w.secret_, 42);  // Accesso diretto al membro privato
}
```

#### Test Generatori con Accesso Privato
```cpp
// Classe target
class Calculator {
private:
    int factor_ = 10;
    int multiply(int x) const { return x * factor_; }
public:
    // Concedi accesso al test generatore
    GTESTG_FRIEND_TEST_G(CalcTest, TestMultiply);
};

// File di test
struct CalcTest : ::gtest_generator::TestWithGenerator {
    Calculator calc;
};

TEST_G_FRIEND(CalcTest, TestMultiply) {
    int input = GENERATOR(1, 2, 3);
    USE_GENERATOR();

    // Accesso al metodo e membro privati
    int result = calc.multiply(input);
    EXPECT_EQ(result, input * calc.factor_);
}
```

#### Concedi Accesso a Tutti i Test in una Suite
```cpp
// Classe target - concede accesso a TUTTI i test in MyTestSuite
class MyClass {
private:
    int value_ = 100;
public:
    // Concedi accesso all'intera suite di test
    GTESTG_FRIEND_TEST_SUITE(MyTestSuite);
};

// File di test - tutti i test in questa suite hanno accesso
struct MyTestSuite : ::testing::Test {
    MyClass obj;
};

TEST_F(MyTestSuite, Test1) {
    EXPECT_EQ(obj.value_, 100);  // Ha accesso
}

TEST_F(MyTestSuite, Test2) {
    obj.value_ = 200;  // Ha anche accesso
    EXPECT_EQ(obj.value_, 200);
}
```

#### Lavorare con l'Ereditarietà
```cpp
// Classe base
class Base {
private:
    int base_secret_ = 10;
public:
    GTESTG_FRIEND_TEST(DerivedTest, AccessBoth);
};

// Classe derivata
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
    EXPECT_EQ(d.base_secret_, 10);     // Accesso al privato della base
    EXPECT_EQ(d.derived_secret_, 20);  // Accesso al privato della derivata
}
```

### Note Importanti

1. **Richiesta Concessione Esplicita**: Ogni test che necessita di accesso privato deve essere esplicitamente elencato nella classe target
2. **Nessuna Magia**: Usa dichiarazioni friend standard di C++ - semplice e prevedibile
3. **TEST_FRIEND è Opzionale**: `TEST_FRIEND` è solo una macro di convenienza che mappa a `TEST_F`. Puoi usare `TEST_F` normale se la classe ha la dichiarazione `GTESTG_FRIEND_TEST` appropriata
4. **Sicurezza Compile-Time**: Se un test tenta di accedere a membri privati senza avere accesso friend concesso, otterrai un errore di compilazione
5. **Manutenzione**: Quando aggiungi un nuovo test che necessita di accesso privato, ricorda di aggiungere la dichiarazione `GTESTG_FRIEND_TEST` corrispondente alla classe target

### Quando Usare Questa Funzionalità

Usa l'accesso ai membri privati quando:
- Testi stato interno non esposto tramite interfaccia pubblica
- Verifichi logica privata complessa
- Imposti stati interni specifici per i test
- Testi codice legacy che non può essere facilmente refactorizzato

Evita di usare quando:
- La necessità di accesso privato indica un design scadente
- Il test dell'interfaccia pubblica sarebbe sufficiente
- Creerebbe un accoppiamento stretto tra test e implementazione

Vedere `test_friend_access.cpp` per esempi completi.

### Macro TEST_FRIEND e TEST_G_FRIEND

La libreria fornisce macro `TEST_FRIEND` e `TEST_G_FRIEND` che creano infrastruttura di test con supporto integrato per il pattern VirtualAccessor. Queste macro funzionano perfettamente con la dichiarazione `GTESTG_FRIEND_ACCESS_PRIVATE()`.

**Punti Chiave:**
- `GTESTG_FRIEND_ACCESS_PRIVATE()` concede accesso friend per **entrambi** gli approcci basati su classi (VirtualAccessor) e basati su funzioni (gtestg_private_accessMember)
- Usa `TEST_FRIEND` per test regolari in stile TEST_F
- Usa `TEST_G_FRIEND` per test parametrizzati basati su generatori
- Continua a usare macro `GTESTG_PRIVATE_MEMBER` per accedere ai membri privati

**Esempio con TEST_FRIEND:**
```cpp
class Widget {
private:
    int secret_ = 42;
public:
    Widget() = default;

    // Una singola macro concede entrambi i tipi di accesso friend
    GTESTG_FRIEND_ACCESS_PRIVATE();
};

// Dichiarare l'accessor
GTESTG_PRIVATE_DECLARE_MEMBER(Widget, secret_);

struct WidgetTest : ::testing::Test {
    Widget w;
};

TEST_FRIEND(WidgetTest, AccessPrivate) {
    // Accedere al membro privato usando l'accessor basato su funzioni
    int& secret = GTESTG_PRIVATE_MEMBER(Widget, secret_, &w);
    EXPECT_EQ(secret, 42);
    secret = 100;
    EXPECT_EQ(secret, 100);
}
```

**Esempio con TEST_G_FRIEND:**
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

**Supporto Multi-File:**
`TEST_FRIEND` e `TEST_G_FRIEND` funzionano correttamente quando i test sono definiti in più file .cpp collegati allo stesso eseguibile, proprio come `TEST_G` regolare. Vedere `test_friend_multi_file1.cpp` e `test_friend_multi_file2.cpp` per esempi.

### Sistema Unificato di Accesso ai Membri Privati

La libreria fornisce un sistema unificato per accedere ai membri privati e protetti nei tuoi test. Aggiungendo una singola macro `GTESTG_FRIEND_ACCESS_PRIVATE()` alla tua classe, abiliti **due approcci complementari** per l'accesso ai membri privati:

1. **Accesso Diretto tramite TEST_FRIEND/TEST_G_FRIEND** - Raccomandato per la maggior parte dei casi
2. **Accesso Basato su Funzioni tramite macro GTESTG_PRIVATE_MEMBER** - Per un controllo più esplicito

Entrambi gli approcci funzionano perfettamente insieme e possono essere utilizzati nello stesso test.

#### Il Nucleo: GTESTG_FRIEND_ACCESS_PRIVATE()

Aggiungi questa singola macro alla tua classe per abilitare l'accesso ai membri privati:

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

Questa macro concede l'accesso friend a:
- **Template VirtualAccessor** - Usato da TEST_FRIEND e TEST_G_FRIEND
- **Funzione gtestg_private_accessMember** - Usata dalle macro GTESTG_PRIVATE_MEMBER

#### Approccio 1: Usare TEST_FRIEND e TEST_G_FRIEND (Raccomandato)

Per casi semplici, usa `TEST_FRIEND` o `TEST_G_FRIEND` per creare test che possono accedere direttamente ai membri privati:

**Esempio con TEST_FRIEND:**
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

**Esempio con TEST_G_FRIEND:**
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

**Supporto Multi-File:**
`TEST_FRIEND` e `TEST_G_FRIEND` funzionano correttamente quando i test sono definiti in più file .cpp collegati allo stesso eseguibile. Vedere `test_friend_multi_file1.cpp` e `test_friend_multi_file2.cpp` per esempi.

**Come Funziona:**
- `TEST_FRIEND` e `TEST_G_FRIEND` creano una specializzazione template esplicita di `VirtualAccessor<Suite, TestName>` all'interno del namespace `gtestg_detail`
- Questa specializzazione ottiene l'accesso friend tramite `GTESTG_FRIEND_ACCESS_PRIVATE()`
- Poiché `VirtualAccessor` è un friend e deriva dalla tua fixture di test, può accedere ai membri privati della classe target
- Il corpo del test viene eseguito all'interno del contesto di questa classe friend, abilitando l'accesso diretto ai membri privati
- Ogni test ottiene un tipo tag unico per creare una specializzazione separata, evitando conflitti di nomi

#### Approccio 2: Usare le Macro GTESTG_PRIVATE_MEMBER (Controllo Esplicito)

Per maggiore controllo o quando lavori con le macro regolari `TEST_F`/`TEST_G`, usa le macro accessor basate su funzioni. Questo approccio richiede di dichiarare l'accesso per ogni membro che vuoi accedere.

**Passo 1: Dichiarare l'accesso fuori dalla tua classe (nel file di test):**
```cpp
// Declare which members you want to access
GTESTG_PRIVATE_DECLARE_MEMBER(Widget, secret_);
GTESTG_PRIVATE_DECLARE_MEMBER(Widget, privateName);
```

**Passo 2: Accedere ai membri nei tuoi test:**
```cpp
TEST_FRIEND(WidgetTest, AccessPrivate) {
    // Access using the macro
    int& secret = GTESTG_PRIVATE_MEMBER(Widget, secret_, &w);
    EXPECT_EQ(secret, 42);
    secret = 100;
    EXPECT_EQ(secret, 100);
}
```

Questo approccio è utile quando:
- Vuoi documentazione esplicita di quali membri vengono acceduti
- Devi accedere a membri statici
- Vuoi funzioni accessor personalizzate con logica aggiuntiva

#### Combinare Entrambi gli Approcci

Puoi usare entrambi gli approcci nello stesso test:

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

### Riferimento Completo dell'API per le Macro GTESTG_PRIVATE_MEMBER

Questa sezione fornisce un riferimento dettagliato per le macro di accesso ai membri privati basate su funzioni (Approccio 2).

#### Dichiarare l'Accesso ai Membri

Posiziona queste dichiarazioni **fuori** dalla tua classe, tipicamente nel tuo file di test. Queste dichiarazioni informano il sistema quali membri privati vuoi accedere:

| Macro | Scopo | Parametri | Esempio |
|-------|-------|-----------|---------|
| `GTESTG_PRIVATE_DECLARE_MEMBER` | Dichiarare l'accesso ai membri di istanza | Target, MemberName | `GTESTG_PRIVATE_DECLARE_MEMBER(MyClass, privateField)` |
| `GTESTG_PRIVATE_DECLARE_STATIC` | Dichiarare l'accesso ai membri statici | Target, MemberName | `GTESTG_PRIVATE_DECLARE_STATIC(MyClass, staticCounter)` |
| `GTESTG_PRIVATE_DECLARE_FUNCTION` | Dichiarare una funzione accessor personalizzata | ThisClass, Target, FuncName | `GTESTG_PRIVATE_DECLARE_FUNCTION(MyTest, MyClass, GetSum)` |

#### Macro per Accedere ai Membri

Usa queste macro **all'interno** delle tue funzioni di test per accedere ai membri privati:

| Macro | Scopo | Parametri | Esempio |
|-------|-------|-----------|---------|
| `GTESTG_PRIVATE_MEMBER` | Accedere a un membro di istanza | Target, MemberName, &obj | `GTESTG_PRIVATE_MEMBER(MyClass, privateField, &obj)` |
| `GTESTG_PRIVATE_STATIC` | Accedere a un membro statico | Target, MemberName | `GTESTG_PRIVATE_STATIC(MyClass, staticCounter)` |
| `GTESTG_PRIVATE_CALL` | Chiamare una funzione personalizzata con oggetto test esplicito | Target, FuncName, test_obj, &obj | `GTESTG_PRIVATE_CALL(MyClass, GetSum, this, &obj)` |
| `GTESTG_PRIVATE_CALL_ON_TEST` | Chiamare una funzione personalizzata (usa 'this' implicito) | ThisClass, Target, FuncName, &obj | `GTESTG_PRIVATE_CALL_ON_TEST(MyTest, MyClass, GetSum, &obj)` |

#### Esempi di Utilizzo Dettagliati

I seguenti esempi dimostrano modelli di utilizzo completi per le macro GTESTG_PRIVATE_*.

**Esempio 1: Accedere ai Membri di Istanza**
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

**Esempio 2: Accedere ai Membri Statici**
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

**Esempio 3: Funzioni Accessor Personalizzate**
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

Per esempi completi, vedere `test_private_access.cpp` e `test_define_macros.cpp`.

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

### Note Importanti

1. **Parametro dimensione richiesto**: Devi fornire esplicitamente la dimensione dell'array
2. **Fatale vs Non-fatale**: Usa ASSERT_* per asserzioni fatali, EXPECT_* per non-fatali
3. **Confronti a virgola mobile**: Usa NEAR, FLOAT_EQ o DOUBLE_EQ per valori a virgola mobile
4. **Tipi personalizzati**: Il tuo tipo deve avere operator== definito per EXPECT_ARRAY_EQ
5. **Array di dimensione zero**: Funziona correttamente con array vuoti (dimensione = 0)

Vedere `test_array_compare.cpp` per esempi completi.

## Miglioramenti Futuri

- Calcolo dinamico del numero totale di combinazioni
- Supporto per diversi tipi di dati nei generatori
- Istanziazioni di test con nome
- Supporto per pattern di valori più complessi

## Licenza

Questo progetto è fornito così com'è per scopi educativi e di sviluppo.
