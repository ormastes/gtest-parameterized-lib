# GTest Generator - Bibliothèque de Tests Paramétrés

Une bibliothèque simple en-tête uniquement qui fournit une syntaxe de style Catch2 `GENERATE` pour les tests paramétrés Google Test.

## Fonctionnalités Principales

- Syntaxe simple et intuitive similaire à `GENERATE` de Catch2
- Implémentation en-tête uniquement
- Génération automatique de toutes les combinaisons de tests
- Compatible avec les macros standard `TEST_P` de Google Test

## Démarrage Rapide

```cpp
#include "gtest_generator.h"

// Définir la fixture de test en utilisant TestWithGenerator
class MyTest : public ::gtest_generator::TestWithGenerator {
};

// Écrire un test paramétré en utilisant la macro TEST_G
TEST_G(MyTest, SimpleCase) {
    // ⚠️ CRITIQUE : Les appels GENERATOR() DOIVENT être EN HAUT, AVANT USE_GENERATOR()
    // Cet ordre est obligatoire - la bibliothèque compte les générateurs avant d'exécuter le test
    int a = GENERATOR(1, 2);      // Sera 1 ou 2
    int b = GENERATOR(10, 20);    // Sera 10 ou 20
    USE_GENERATOR();              // DOIT être appelé APRÈS tous les appels GENERATOR()

    EXPECT_LT(a, b);
    printf("Test: a=%d, b=%d\n", a, b);
}
// TEST_G gère automatiquement la génération de tests - pas besoin d'ENABLE_GENERATOR !
```

## Installation

Il s'agit d'une bibliothèque en-tête uniquement. Copiez simplement `gtest_generator.h` dans votre projet et incluez-le.

## Construction du Projet

### Prérequis

- CMake 3.14 ou supérieur
- Bibliothèque Google Test
- Compilateur compatible C++14

### Instructions de Construction

```bash
# Créer le répertoire de construction
mkdir build && cd build

# Configurer avec CMake
cmake ..

# Construire le projet
cmake --build .

# Exécuter les tests
./mygen_test
```

## Plus d'Exemples

### Valeurs Multiples

```cpp
TEST_G(MyTest, MultipleValues) {
    int x = GENERATOR(1, 2, 3);       // 3 valeurs
    int y = GENERATOR(10, 20, 30);    // 3 valeurs
    USE_GENERATOR();
    
    // Génère 9 combinaisons de tests
    EXPECT_LT(x, y);
}
```

### Nombre Différent de Valeurs

```cpp
TEST_G(MyTest, AsymmetricCombinations) {
    int size = GENERATOR(1, 2, 3);     // 3 valeurs
    int scale = GENERATOR(10, 100);    // 2 valeurs
    USE_GENERATOR();

    // Génère 6 combinaisons de tests
    int result = size * scale;
    EXPECT_GT(result, 0);
}
```

### Utilisation de Valeurs Générées dans les Expressions
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

### Logique de Test Complexe
```cpp
TEST_G(MyTest, ConditionalLogic) {
    int mode = GENERATOR(0, 1, 2);     // 3 modes
    int value = GENERATOR(100, 200);   // 2 valeurs
    USE_GENERATOR();

    int result;
    switch (mode) {
        case 0: result = value + 10; break;
        case 1: result = value * 2; break;
        case 2: result = value - 50; break;
    }

    EXPECT_GT(result, 0);
    printf("Mode %d avec valeur %d donne résultat %d\n", mode, value, result);
}
```

## Exemples Avancés

### Travailler avec des Classes et Objets

#### Génération Directe d'Objets
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
    // Générer des objets complets directement
    auto obj1 = GENERATOR(TestObject(1, "premier"), TestObject(2, "deuxième"));
    auto obj2 = GENERATOR(TestObject(10, "dix"), TestObject(20, "vingt"));
    USE_GENERATOR();

    EXPECT_LT(obj1, obj2);
    printf("Objets: obj1={%d, %s}, obj2={%d, %s}\n",
           obj1.value, obj1.name.c_str(),
           obj2.value, obj2.name.c_str());
}
```

#### Utilisation de GENERATOR dans les Arguments du Constructeur
```cpp
TEST_G(MyTest, ConstructorWithGenerators) {
    // Valeurs GENERATOR utilisées comme arguments du constructeur
    int val1 = GENERATOR(1, 2);
    int val2 = GENERATOR(10, 20);
    USE_GENERATOR();

    TestObject objects[] = {
        TestObject(val1, "test"),
        TestObject(val2, "demo")
    };

    EXPECT_LT(objects[0].value, objects[1].value);
    printf("Objets de tableau: [0]={%d,%s}, [1]={%d,%s}\n",
           objects[0].value, objects[0].name.c_str(),
           objects[1].value, objects[1].name.c_str());
}
```

### Travailler avec des Pointeurs et Mémoire Dynamique

#### Génération de Pointeurs vers des Objets
```cpp
TEST_G(MyTest, PointerGeneration) {
    // Générer des pointeurs vers différents objets
    // Note: Faites attention à la gestion de la mémoire
    auto* ptr1 = GENERATOR(new TestObject(1, "premier"),
                          new TestObject(2, "deuxième"));
    auto* ptr2 = GENERATOR(new TestObject(10, "dix"),
                          new TestObject(20, "vingt"));
    USE_GENERATOR();

    EXPECT_LT(*ptr1, *ptr2);
    printf("Pointeurs: ptr1={%d, %s}, ptr2={%d, %s}\n",
           ptr1->value, ptr1->name.c_str(),
           ptr2->value, ptr2->name.c_str());

    // Nettoyage
    delete ptr1;
    delete ptr2;
}
```

#### Appels GENERATOR Imbriqués (Avancé)
```cpp
TEST_G(MyTest, NestedGenerators) {
    // Génération imbriquée complexe - chaque GENERATOR externe contient des appels GENERATOR internes
    int inner1 = GENERATOR(1, 2);
    int inner2 = GENERATOR(3, 4);
    int inner3 = GENERATOR(10, 20);
    int inner4 = GENERATOR(30, 40);
    USE_GENERATOR();

    auto* obj1 = new TestObject(inner1, "premier");
    auto* obj2 = new TestObject(inner3, "deuxième");

    EXPECT_LT(obj1->value, obj2->value);
    printf("Imbriqué: obj1={%d}, obj2={%d}\n", obj1->value, obj2->value);

    delete obj1;
    delete obj2;
}
```

### Travailler avec des Conteneurs STL

#### Génération de Tailles et Contenus de Conteneurs
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

    printf("Vecteur: taille=%d, multiplicateur=%d, éléments=[", size, multiplier);
    for (int v : vec) printf("%d ", v);
    printf("]\n");
}
```

#### Génération de Combinaisons de Chaînes
```cpp
TEST_G(MyTest, StringCombinations) {
    auto prefix_choice = GENERATOR(0, 1);
    auto suffix_choice = GENERATOR(0, 1);
    auto repeat = GENERATOR(1, 2);
    USE_GENERATOR();

    std::string prefix = prefix_choice ? "Bonjour" : "Salut";
    std::string suffix = suffix_choice ? "Monde" : "Tous";

    std::string result;
    for (int i = 0; i < repeat; ++i) {
        if (i > 0) result += " ";
        result += prefix + " " + suffix;
    }

    EXPECT_FALSE(result.empty());
    printf("Chaîne: prefix='%s', suffix='%s', repeat=%d => '%s'\n",
           prefix.c_str(), suffix.c_str(), repeat, result.c_str());
}
```

### Travailler avec des Pointeurs Intelligents

#### Utilisation de unique_ptr avec GENERATOR
```cpp
TEST_G(MyTest, SmartPointers) {
    auto value1 = GENERATOR(1, 2);
    auto value2 = GENERATOR(10, 20);
    USE_GENERATOR();

    auto ptr1 = std::make_unique<TestObject>(value1, "premier");
    auto ptr2 = std::make_unique<TestObject>(value2, "deuxième");

    EXPECT_LT(*ptr1, *ptr2);
    printf("Pointeurs intelligents: ptr1={%d, %s}, ptr2={%d, %s}\n",
           ptr1->value, ptr1->name.c_str(),
           ptr2->value, ptr2->name.c_str());
}
```

### Exemples de Structures Complexes

#### Génération de Structures avec Plusieurs Champs
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
    printf("Points: p1=(%d,%d), p2=(%d,%d), distance=%d\n",
           p1.x, p1.y, p2.x, p2.y, distance);
}
```

## Modes d'Échantillonnage

La bibliothèque prend en charge deux modes d'échantillonnage pour générer des combinaisons de tests :

### Mode FULL (Par Défaut - Produit Cartésien)
Le mode par défaut génère toutes les combinaisons possibles de valeurs (produit cartésien). Il s'agit du comportement traditionnel qui garantit une couverture de test complète.

```cpp
TEST_G(MyTest, FullMode) {
    USE_GENERATOR();  // Par défaut, c'est le mode FULL
    // ou explicitement : USE_GENERATOR(FULL);
    
    auto x = GENERATOR(1, 2);      // 2 valeurs
    auto y = GENERATOR(10, 20);    // 2 valeurs
    auto z = GENERATOR(100, 200);  // 2 valeurs
    
    // Génère 8 exécutions de test : 2 × 2 × 2 = 8 combinaisons
    // (1,10,100), (1,10,200), (1,20,100), (1,20,200),
    // (2,10,100), (2,10,200), (2,20,100), (2,20,200)
}
```

### Mode ALIGNED (Itération Parallèle)
Le mode ALIGNED itère à travers toutes les colonnes en parallèle, comme une fermeture éclair. Chaque colonne avance à sa valeur suivante à chaque exécution, en se répétant lorsqu'elle atteint la fin. Le nombre total d'exécutions est égal à la taille de la plus grande colonne.

```cpp
TEST_G(MyTest, AlignedMode) {
    USE_GENERATOR(ALIGNED);  // Activer le mode ALIGNED
    
    auto x = GENERATOR(1, 2);           // 2 valeurs
    auto y = GENERATOR(10, 20, 30, 40); // 4 valeurs  
    auto z = GENERATOR(100, 200, 300);  // 3 valeurs
    
    // Génère 4 exécutions de test (taille maximale de colonne) :
    // Exécution 0 : (1, 10, 100)  - tous à l'index 0
    // Exécution 1 : (2, 20, 200)  - tous à l'index 1
    // Exécution 2 : (1, 30, 300)  - x revient à 0, autres à l'index 2
    // Exécution 3 : (2, 40, 100)  - x à 1, y à 3, z revient à 0
}
```

#### Caractéristiques Clés du Mode ALIGNED :
- **Déterministe** : Les valeurs sont sélectionnées dans l'ordre (0, 1, 2, ...) avec répétition
- **Ordre de Déclaration** : Les colonnes sont traitées dans l'ordre où elles sont déclarées
- **Moins d'Exécutions** : Total d'exécutions = taille maximale de colonne (pas le produit)
- **Couverture Équilibrée** : Chaque valeur dans chaque colonne est utilisée approximativement de manière égale

#### Détails d'Implémentation :
La bibliothèque implémente le mode ALIGNED efficacement en utilisant le mécanisme `GTEST_SKIP()` de Google Test :
1. Pendant l'enregistrement des tests, la bibliothèque compte la taille de chaque colonne GENERATOR
2. Au moment de l'exécution, la bibliothèque calcule la taille maximale de colonne
3. Les exécutions de test au-delà de la taille maximale sont sautées en utilisant `GTEST_SKIP()`
4. Chaque colonne se répète lorsqu'elle atteint la fin (en utilisant l'opérateur modulo)

Cette approche garantit que :
- Seules les exécutions de test nécessaires sont effectuées (pas tous les produits cartésiens)
- Le nombre de tests rapporté par Google Test reflète les tests réellement exécutés
- Aucune pénalité de performance due à la génération de combinaisons inutiles

#### Quand Utiliser Chaque Mode :
- **Mode FULL** : Quand vous avez besoin de tests exhaustifs de toutes les combinaisons
- **Mode ALIGNED** : Quand vous voulez un échantillonnage représentatif avec moins d'exécutions de test

#### Exemple de Comparaison :
```cpp
// Mode FULL : 3 × 2 × 2 = 12 exécutions
TEST_G(MyTest, FullExample) {
    USE_GENERATOR(FULL);
    auto a = GENERATOR(1, 2, 3);
    auto b = GENERATOR(10, 20);
    auto c = GENERATOR(100, 200);
    // Génère toutes les 12 combinaisons
}

// Mode ALIGNED : max(3, 2, 2) = 3 exécutions
TEST_G(MyTest, AlignedExample) {
    USE_GENERATOR(ALIGNED);
    auto a = GENERATOR(1, 2, 3);
    auto b = GENERATOR(10, 20);
    auto c = GENERATOR(100, 200);
    // Génère seulement 3 combinaisons :
    // (1, 10, 100), (2, 20, 200), (3, 10, 100)
}
```

#### Exemple de Test de Vérification :
Le projet inclut `test_mode_counts.cpp` qui vérifie de manière complète que les deux modes fonctionnent correctement :

```cpp
// Test avec configuration 3x2x2
TEST_G(ModeCountTest, Full_3x2x2) {
    int a = GENERATOR(1, 2, 3);        // 3 valeurs
    int b = GENERATOR(10, 20);         // 2 valeurs
    int c = GENERATOR(100, 200);       // 2 valeurs
    USE_GENERATOR(FULL);

    // Vérifie : génère exactement 12 cas de test (3 × 2 × 2)
    // Toutes les combinaisons sont uniques
}

TEST_G(ModeCountTest, Aligned_3x2x2) {
    int a = GENERATOR(1, 2, 3);        // 3 valeurs
    int b = GENERATOR(10, 20);         // 2 valeurs
    int c = GENERATOR(100, 200);       // 2 valeurs
    USE_GENERATOR(ALIGNED);

    // Vérifie : génère exactement 3 cas de test (taille maximale de colonne)
    // Résultats : (1,10,100), (2,20,200), (3,10,100)
}
```

Exécutez le test de vérification pour voir la sortie détaillée :
```bash
./build/test_mode_counts
```

La sortie attendue montre :
- Mode FULL 3x2x2 : 12 exécutions avec toutes les combinaisons uniques
- Mode ALIGNED 3x2x2 : 3 exécutions (taille maximale de colonne)
- Mode FULL 2x3x4 : 24 exécutions avec toutes les combinaisons uniques
- Mode ALIGNED 2x3x4 : 4 exécutions (taille maximale de colonne)
- Mode FULL 5x1x3 : 15 exécutions avec toutes les combinaisons uniques
- Mode ALIGNED 5x1x3 : 5 exécutions (taille maximale de colonne)

## Référence API

### Macros

- **`TEST_G(TestClassName, TestName)`** - Définit un test paramétré avec génération automatique de tests. Remplace à la fois TEST_P et le besoin d'appels ENABLE_GENERATOR séparés.

- **`GENERATOR(...)`** - Définit les valeurs pour un paramètre de test. Chaque appel crée une nouvelle dimension de combinaisons de tests.
  ```cpp
  int value = GENERATOR(1, 2, 3);  // Crée 3 variantes
  ```
  **IMPORTANT** : Tous les appels GENERATOR() doivent venir AVANT USE_GENERATOR()

- **`USE_GENERATOR()`** - Doit être appelé une fois dans chaque TEST_G, APRÈS tous les appels GENERATOR(). Utilise le mode FULL par défaut.

- **`USE_GENERATOR(mode)`** - Doit être appelé une fois dans chaque TEST_G, APRÈS tous les appels GENERATOR(). Spécifie le mode d'échantillonnage :
  - `USE_GENERATOR(FULL)` - Produit cartésien de toutes les valeurs (identique au défaut)
  - `USE_GENERATOR(ALIGNED)` - Itération parallèle à travers les colonnes

## Comment Ça Fonctionne

La bibliothèque utilise des variables statiques thread-local pour maintenir l'état du générateur entre les instanciations de tests. Quand `GENERATOR` est appelé :

1. Il enregistre les valeurs possibles pour ce générateur
2. Basé sur l'index du paramètre de test actuel, il calcule quelle valeur retourner
3. Toutes les combinaisons sont générées automatiquement

## Notes d'Utilisation Importantes

### ⚠️ CRITIQUE : Ordre de GENERATOR et USE_GENERATOR

**TRÈS IMPORTANT :** Tous les appels `GENERATOR()` **DOIVENT** être **EN HAUT** du cas de test, **AVANT** `USE_GENERATOR()`. Cet ordre est obligatoire pour que la bibliothèque fonctionne correctement.

✅ **CORRECT** :
```cpp
TEST_G(MyTest, Example) {
    // Étape 1 : TOUS les appels GENERATOR() d'abord EN HAUT
    int a = GENERATOR(1, 2);
    int b = GENERATOR(10, 20);

    // Étape 2 : USE_GENERATOR() APRÈS tous les générateurs
    USE_GENERATOR();

    // Étape 3 : Votre logique de test ici
    EXPECT_LT(a, b);
}
```

❌ **INCORRECT - NE fonctionnera PAS** :
```cpp
TEST_G(MyTest, Example) {
    USE_GENERATOR();             // ❌ FAUX ! Doit venir après les générateurs
    int a = GENERATOR(1, 2);     // ❌ Trop tard !
    int b = GENERATOR(10, 20);
}
```

❌ **INCORRECT - NE fonctionnera PAS** :
```cpp
TEST_G(MyTest, Example) {
    int a = GENERATOR(1, 2);
    USE_GENERATOR();             // ❌ FAUX ! Doit venir après TOUS les générateurs
    int b = GENERATOR(10, 20);   // ❌ Ce générateur est après USE_GENERATOR()
}
```

**Pourquoi c'est important :**
- La bibliothèque compte les générateurs pendant une phase de pré-exécution
- `USE_GENERATOR()` marque la fin de la phase de déclaration des générateurs
- Les générateurs déclarés après `USE_GENERATOR()` seront ignorés ou causeront des erreurs

## Exemple de Sortie

```
[==========] Exécution de 4 tests depuis 1 suite de tests.
[----------] 4 tests depuis Generator/MyTest__SimpleCase
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
[==========] 4 tests depuis 1 suite de tests exécutés. (0 ms au total)
[  RÉUSSI  ] 4 tests.
```

## Limitations

- La fixture de test doit hériter de `::gtest_generator::TestWithGenerator`
- Doit utiliser la macro `TEST_G` au lieu du `TEST_P` standard
- Tous les appels `GENERATOR()` doivent venir avant `USE_GENERATOR()`
- Les types complexes (objets, pointeurs) fonctionnent avec GENERATOR mais peuvent nécessiter une instanciation de template appropriée
- La gestion de la mémoire est la responsabilité de l'utilisateur lors de la génération de pointeurs avec `new`

## Accès aux Membres Privés pour les Tests

La bibliothèque fournit un moyen simple et propre d'accéder aux membres privés dans les tests en utilisant des déclarations friend explicites.

### Exemple Rapide

```cpp
// Dans votre classe de production
class MyClass {
private:
    int privateValue;
    std::string privateName;

    int computeSecret(int x) const { return privateValue * x; }

public:
    MyClass(int v, const std::string& n) : privateValue(v), privateName(n) {}

    // Accorder un accès ami à des tests spécifiques
    GTESTG_FRIEND_TEST(MyClassTest, AccessPrivateMembers);
    GTESTG_FRIEND_TEST(MyClassTest, ModifyPrivateMembers);
};

// Dans votre fichier de test
struct MyClassTest : ::testing::Test {
    MyClass obj{42, "secret"};
};

TEST_FRIEND(MyClassTest, AccessPrivateMembers) {
    // Accès direct aux membres privés !
    EXPECT_EQ(obj.privateValue, 42);
    EXPECT_EQ(obj.privateName, "secret");
}

TEST_FRIEND(MyClassTest, ModifyPrivateMembers) {
    // Peut modifier les membres privés
    obj.privateValue = 100;
    EXPECT_EQ(obj.privateValue, 100);

    // Peut appeler les méthodes privées
    int result = obj.computeSecret(2);
    EXPECT_EQ(result, 200);
}
```

### Fonctionnalités Clés

- **Simple et Propre** : Utilise les déclarations friend standard de C++
- **Accès Sélectif** : Accordez l'accès uniquement aux tests spécifiques qui en ont besoin
- **Zéro Surcharge** : Mécanisme pur au moment de la compilation, aucun coût d'exécution
- **Sûr au Niveau des Types** : Sécurité des types garantie par le compilateur
- **Sûr pour la Production** : Les déclarations friend n'ont aucun impact à l'exécution

### Référence API

#### Macros pour les Classes Cibles

| Macro | Objectif | Utilisation |
|-------|---------|-------|
| `GTESTG_FRIEND_TEST(Suite, TestName)` | Accorder un accès ami à un test TEST_F spécifique | Placer dans la définition de classe |
| `GTESTG_FRIEND_TEST_G(TestClassName, TestName)` | Accorder un accès ami à un test TEST_G | Placer dans la définition de classe |
| `GTESTG_FRIEND_TEST_SUITE(Suite)` | Accorder un accès ami à tous les tests d'une suite | Placer dans la définition de classe |

#### Macros pour les Fichiers de Test

| Macro | Objectif | Utilisation |
|-------|---------|-------|
| `TEST_FRIEND(Suite, TestName)` | Définir un test avec accès ami | Comme TEST_F |
| `TEST_G_FRIEND(TestClassName, TestName)` | Définir un test générateur avec accès ami | Comme TEST_G |

### Exemples d'Utilisation

#### Accès Privé de Base
```cpp
// Classe cible
class Widget {
private:
    int secret_ = 42;
public:
    // Accorder l'accès à un test spécifique
    GTESTG_FRIEND_TEST(WidgetTest, CheckSecret);
};

// Fichier de test
struct WidgetTest : ::testing::Test {
    Widget w;
};

TEST_FRIEND(WidgetTest, CheckSecret) {
    EXPECT_EQ(w.secret_, 42);  // Accès direct au membre privé
}
```

#### Tests Générateurs avec Accès Privé
```cpp
// Classe cible
class Calculator {
private:
    int factor_ = 10;
    int multiply(int x) const { return x * factor_; }
public:
    // Accorder l'accès au test générateur
    GTESTG_FRIEND_TEST_G(CalcTest, TestMultiply);
};

// Fichier de test
struct CalcTest : ::gtest_generator::TestWithGenerator {
    Calculator calc;
};

TEST_G_FRIEND(CalcTest, TestMultiply) {
    int input = GENERATOR(1, 2, 3);
    USE_GENERATOR();

    // Accès à la méthode et au membre privés
    int result = calc.multiply(input);
    EXPECT_EQ(result, input * calc.factor_);
}
```

#### Accorder l'Accès à Tous les Tests d'une Suite
```cpp
// Classe cible - accorde l'accès à TOUS les tests dans MyTestSuite
class MyClass {
private:
    int value_ = 100;
public:
    // Accorder l'accès à toute la suite de tests
    GTESTG_FRIEND_TEST_SUITE(MyTestSuite);
};

// Fichier de test - tous les tests de cette suite ont accès
struct MyTestSuite : ::testing::Test {
    MyClass obj;
};

TEST_F(MyTestSuite, Test1) {
    EXPECT_EQ(obj.value_, 100);  // A l'accès
}

TEST_F(MyTestSuite, Test2) {
    obj.value_ = 200;  // A également l'accès
    EXPECT_EQ(obj.value_, 200);
}
```

#### Travailler avec l'Héritage
```cpp
// Classe de base
class Base {
private:
    int base_secret_ = 10;
public:
    GTESTG_FRIEND_TEST(DerivedTest, AccessBoth);
};

// Classe dérivée
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
    EXPECT_EQ(d.base_secret_, 10);     // Accès au privé de base
    EXPECT_EQ(d.derived_secret_, 20);  // Accès au privé dérivé
}
```

### Notes Importantes

1. **Accord Explicite Requis** : Chaque test nécessitant un accès privé doit être explicitement listé dans la classe cible
2. **Pas de Magie** : Utilise des déclarations friend standard de C++ - simple et prévisible
3. **TEST_FRIEND est Optionnel** : `TEST_FRIEND` est juste une macro de commodité qui correspond à `TEST_F`. Vous pouvez utiliser `TEST_F` régulier si la classe a la déclaration `GTESTG_FRIEND_TEST` appropriée
4. **Sécurité au Moment de la Compilation** : Si un test tente d'accéder à des membres privés sans avoir reçu l'accès ami, vous obtiendrez une erreur de compilation
5. **Maintenance** : Lorsque vous ajoutez un nouveau test nécessitant un accès privé, n'oubliez pas d'ajouter la déclaration `GTESTG_FRIEND_TEST` correspondante à la classe cible

### Quand Utiliser Cette Fonctionnalité

Utilisez l'accès aux membres privés lorsque :
- Tester l'état interne qui n'est pas exposé via l'interface publique
- Vérifier une logique privée complexe
- Configurer des états internes spécifiques pour les tests
- Tester du code hérité qui ne peut pas être facilement refactorisé

Évitez d'utiliser lorsque :
- Le besoin d'accès privé indique une mauvaise conception
- Le test de l'interface publique serait suffisant
- Cela créerait un couplage étroit entre les tests et l'implémentation

Voir `test_friend_access.cpp` pour des exemples complets.

### Macros TEST_FRIEND et TEST_G_FRIEND

La bibliothèque fournit des macros `TEST_FRIEND` et `TEST_G_FRIEND` qui créent une infrastructure de test avec un support intégré pour le modèle VirtualAccessor. Ces macros fonctionnent de manière transparente avec la déclaration `GTESTG_FRIEND_ACCESS_PRIVATE()`.

**Points Clés :**
- `GTESTG_FRIEND_ACCESS_PRIVATE()` accorde un accès friend pour **les deux** approches basées sur les classes (VirtualAccessor) et basées sur les fonctions (gtestg_private_accessMember)
- Utilisez `TEST_FRIEND` pour les tests réguliers de style TEST_F
- Utilisez `TEST_G_FRIEND` pour les tests paramétrés basés sur les générateurs
- Continuez à utiliser les macros `GTESTG_PRIVATE_MEMBER` pour accéder aux membres privés

**Exemple avec TEST_FRIEND :**
```cpp
class Widget {
private:
    int secret_ = 42;
public:
    Widget() = default;

    // Une seule macro accorde les deux types d'accès friend
    GTESTG_FRIEND_ACCESS_PRIVATE();
};

// Déclarer l'accesseur
GTESTG_PRIVATE_DECLARE_MEMBER(Widget, secret_);

struct WidgetTest : ::testing::Test {
    Widget w;
};

TEST_FRIEND(WidgetTest, AccessPrivate) {
    // Accéder au membre privé en utilisant l'accesseur basé sur les fonctions
    int& secret = GTESTG_PRIVATE_MEMBER(Widget, secret_, &w);
    EXPECT_EQ(secret, 42);
    secret = 100;
    EXPECT_EQ(secret, 100);
}
```

**Exemple avec TEST_G_FRIEND :**
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

**Support Multi-Fichiers :**
`TEST_FRIEND` et `TEST_G_FRIEND` fonctionnent correctement lorsque les tests sont définis dans plusieurs fichiers .cpp liés au même exécutable, tout comme `TEST_G` régulier. Voir `test_friend_multi_file1.cpp` et `test_friend_multi_file2.cpp` pour des exemples.

### Système Unifié d'Accès aux Membres Privés

La bibliothèque fournit un système unifié pour accéder aux membres privés et protégés dans vos tests. En ajoutant une seule macro `GTESTG_FRIEND_ACCESS_PRIVATE()` à votre classe, vous activez **deux approches complémentaires** pour l'accès aux membres privés :

1. **Accès Direct via TEST_FRIEND/TEST_G_FRIEND** - Recommandé pour la plupart des cas
2. **Accès Basé sur les Fonctions via les macros GTESTG_PRIVATE_MEMBER** - Pour plus de contrôle explicite

Les deux approches fonctionnent de manière transparente ensemble et peuvent être utilisées dans le même test.

#### Le Cœur : GTESTG_FRIEND_ACCESS_PRIVATE()

Ajoutez cette seule macro à votre classe pour activer l'accès aux membres privés :

```cpp
class MyClass {
private:
    int privateValue = 42;
    std::string privateName = "secret";
public:
    // Une seule macro active les deux approches d'accès
    GTESTG_FRIEND_ACCESS_PRIVATE();
};
```

Cette macro accorde un accès friend à :
- **Template VirtualAccessor** - Utilisé par TEST_FRIEND et TEST_G_FRIEND
- **Fonction gtestg_private_accessMember** - Utilisée par les macros GTESTG_PRIVATE_MEMBER

#### Approche 1 : Utilisation de TEST_FRIEND et TEST_G_FRIEND (Recommandé)

Pour les cas simples, utilisez `TEST_FRIEND` ou `TEST_G_FRIEND` pour créer des tests qui peuvent accéder directement aux membres privés :

**Exemple avec TEST_FRIEND :**
```cpp
struct WidgetTest : ::testing::Test {
    Widget w;
};

TEST_FRIEND(WidgetTest, AccessPrivate) {
    // Accès direct aux membres privés (via la spécialisation VirtualAccessor)
    EXPECT_EQ(w.secret_, 42);
    w.secret_ = 100;
    EXPECT_EQ(w.secret_, 100);
}
```

**Exemple avec TEST_G_FRIEND :**
```cpp
struct WidgetGenTest : ::gtest_generator::TestWithGenerator {
    Widget w{999};
};

TEST_G_FRIEND(WidgetGenTest, GeneratorTest) {
    int factor = GENERATOR(1, 2, 5);
    USE_GENERATOR();

    // L'accès direct fonctionne également dans les tests paramétrés
    EXPECT_EQ(w.secret_, 999);
    printf("factor=%d, secret=%d\n", factor, w.secret_);
}
```

**Support Multi-Fichiers :**
`TEST_FRIEND` et `TEST_G_FRIEND` fonctionnent correctement lorsque les tests sont définis dans plusieurs fichiers .cpp liés au même exécutable. Voir `test_friend_multi_file1.cpp` et `test_friend_multi_file2.cpp` pour des exemples.

**Comment Ça Fonctionne :**
- `TEST_FRIEND` et `TEST_G_FRIEND` créent une spécialisation de template explicite de `VirtualAccessor<Suite, TestName>` dans l'espace de noms `gtestg_detail`
- Cette spécialisation reçoit un accès friend via `GTESTG_FRIEND_ACCESS_PRIVATE()`
- Comme `VirtualAccessor` est un ami et dérive de votre fixture de test, il peut accéder aux membres privés de la classe cible
- Le corps du test s'exécute dans le contexte de cette classe amie, permettant un accès direct aux membres privés
- Chaque test obtient un type de tag unique pour créer une spécialisation séparée, évitant les conflits de noms

#### Approche 2 : Utilisation des Macros GTESTG_PRIVATE_MEMBER (Contrôle Explicite)

Pour plus de contrôle ou lorsque vous travaillez avec les macros régulières `TEST_F`/`TEST_G`, utilisez les macros d'accesseur basées sur les fonctions. Cette approche nécessite de déclarer l'accès pour chaque membre auquel vous souhaitez accéder.

**Étape 1 : Déclarer l'accès en dehors de votre classe (dans le fichier de test) :**
```cpp
// Déclarer quels membres vous souhaitez accéder
GTESTG_PRIVATE_DECLARE_MEMBER(Widget, secret_);
GTESTG_PRIVATE_DECLARE_MEMBER(Widget, privateName);
```

**Étape 2 : Accéder aux membres dans vos tests :**
```cpp
TEST_FRIEND(WidgetTest, AccessPrivate) {
    // Accès en utilisant la macro
    int& secret = GTESTG_PRIVATE_MEMBER(Widget, secret_, &w);
    EXPECT_EQ(secret, 42);
    secret = 100;
    EXPECT_EQ(secret, 100);
}
```

Cette approche est utile lorsque :
- Vous souhaitez une documentation explicite des membres qui sont accédés
- Vous devez accéder aux membres statiques
- Vous souhaitez des fonctions d'accesseur personnalisées avec une logique supplémentaire

#### Combinaison des Deux Approches

Vous pouvez utiliser les deux approches dans le même test :

```cpp
class Widget {
private:
    int secret_ = 42;
    static int counter_;
public:
    GTESTG_FRIEND_ACCESS_PRIVATE();  // Active les deux approches
};

int Widget::counter_ = 0;

// Déclarer l'accès pour le membre statique
GTESTG_PRIVATE_DECLARE_STATIC(Widget, counter_);

TEST_G_FRIEND(WidgetTest, CombinedAccess) {
    int value = GENERATOR(10, 20);
    USE_GENERATOR();

    Widget w;

    // Approche 1 : Accès direct au membre d'instance
    w.secret_ = value;
    EXPECT_EQ(w.secret_, value);

    // Approche 2 : Utiliser la macro pour le membre statique
    int& count = GTESTG_PRIVATE_STATIC(Widget, counter_);
    count++;
}
```

### Référence Complète de l'API pour les Macros GTESTG_PRIVATE_MEMBER

Cette section fournit une référence détaillée pour les macros d'accès aux membres privés basées sur les fonctions (Approche 2).

#### Déclaration de l'Accès aux Membres

Placez ces déclarations **en dehors** de votre classe, généralement dans votre fichier de test. Ces déclarations indiquent au système quels membres privés vous souhaitez accéder :

| Macro | Objectif | Paramètres | Exemple |
|-------|---------|------------|---------|
| `GTESTG_PRIVATE_DECLARE_MEMBER` | Déclarer l'accès aux membres d'instance | Target, MemberName | `GTESTG_PRIVATE_DECLARE_MEMBER(MyClass, privateField)` |
| `GTESTG_PRIVATE_DECLARE_STATIC` | Déclarer l'accès aux membres statiques | Target, MemberName | `GTESTG_PRIVATE_DECLARE_STATIC(MyClass, staticCounter)` |
| `GTESTG_PRIVATE_DECLARE_FUNCTION` | Déclarer une fonction d'accesseur personnalisée | ThisClass, Target, FuncName | `GTESTG_PRIVATE_DECLARE_FUNCTION(MyTest, MyClass, GetSum)` |

#### Macros pour Accéder aux Membres

Utilisez ces macros **à l'intérieur** de vos fonctions de test pour accéder aux membres privés :

| Macro | Objectif | Paramètres | Exemple |
|-------|---------|------------|---------|
| `GTESTG_PRIVATE_MEMBER` | Accéder au membre d'instance | Target, MemberName, &obj | `GTESTG_PRIVATE_MEMBER(MyClass, privateField, &obj)` |
| `GTESTG_PRIVATE_STATIC` | Accéder au membre statique | Target, MemberName | `GTESTG_PRIVATE_STATIC(MyClass, staticCounter)` |
| `GTESTG_PRIVATE_CALL` | Appeler une fonction personnalisée avec un objet de test explicite | Target, FuncName, test_obj, &obj | `GTESTG_PRIVATE_CALL(MyClass, GetSum, this, &obj)` |
| `GTESTG_PRIVATE_CALL_ON_TEST` | Appeler une fonction personnalisée (utilise 'this' implicite) | ThisClass, Target, FuncName, &obj | `GTESTG_PRIVATE_CALL_ON_TEST(MyTest, MyClass, GetSum, &obj)` |

#### Exemples d'Utilisation Détaillés

Les exemples suivants démontrent des modèles d'utilisation complets pour les macros GTESTG_PRIVATE_*.

**Exemple 1 : Accès aux Membres d'Instance**
```cpp
class MyClass {
private:
    int privateValue = 42;
    std::string privateName = "secret";
public:
    GTESTG_FRIEND_ACCESS_PRIVATE();
};

// Déclarer l'accès (dans le fichier de test)
GTESTG_PRIVATE_DECLARE_MEMBER(MyClass, privateValue);
GTESTG_PRIVATE_DECLARE_MEMBER(MyClass, privateName);

TEST_G(MyTest, AccessPrivate) {
    int value = GENERATOR(10, 20, 30);
    USE_GENERATOR();

    MyClass obj;

    // Accéder et modifier les membres privés
    int& val = GTESTG_PRIVATE_MEMBER(MyClass, privateValue, &obj);
    EXPECT_EQ(val, 42);
    val = value;  // Peut modifier
    EXPECT_EQ(val, value);

    std::string& name = GTESTG_PRIVATE_MEMBER(MyClass, privateName, &obj);
    EXPECT_EQ(name, "secret");
    name = "modified";
}
```

**Exemple 2 : Accès aux Membres Statiques**
```cpp
class MyClass {
private:
    static int staticCounter;
public:
    GTESTG_FRIEND_ACCESS_PRIVATE();
};

int MyClass::staticCounter = 100;

// Déclarer l'accès au membre statique
GTESTG_PRIVATE_DECLARE_STATIC(MyClass, staticCounter);

TEST_G(MyTest, AccessStatic) {
    USE_GENERATOR();

    // Accéder au membre statique (aucun objet nécessaire)
    int& count = GTESTG_PRIVATE_STATIC(MyClass, staticCounter);
    EXPECT_EQ(count, 100);
    count = 200;  // Peut modifier
    EXPECT_EQ(count, 200);
}
```

**Exemple 3 : Fonctions d'Accesseur Personnalisées**
```cpp
class MyClass {
private:
    int field1 = 10;
    int field2 = 20;
public:
    GTESTG_FRIEND_ACCESS_PRIVATE();
};

class MyTest : public ::gtest_generator::TestWithGenerator {};

// Déclarer une fonction personnalisée avec accès au contexte de test et aux membres privés
// THIS = objet de test, TARGET = objet accédé
GTESTG_PRIVATE_DECLARE_FUNCTION(MyTest, MyClass, GetSum) {
    // Peut accéder au contexte de test : THIS->GetParam()
    // Peut accéder aux membres privés : TARGET->field1, TARGET->field2
    return TARGET->field1 + TARGET->field2;
}

TEST_G(MyTest, CustomFunction) {
    int multiplier = GENERATOR(1, 2, 3);
    USE_GENERATOR();

    MyClass obj;

    // Appeler la fonction personnalisée en utilisant CALL_ON_TEST (utilise 'this' implicite)
    int sum = GTESTG_PRIVATE_CALL_ON_TEST(MyTest, MyClass, GetSum, &obj);
    EXPECT_EQ(sum, 30);  // 10 + 20

    // Alternative : Passer l'objet de test explicitement
    int sum2 = GTESTG_PRIVATE_CALL(MyClass, GetSum, static_cast<MyTest*>(this), &obj);
    EXPECT_EQ(sum2, 30);
}
```

Pour des exemples complets, voir `test_private_access.cpp` et `test_define_macros.cpp`.

## Macros de Comparaison de Tableaux

La bibliothèque fournit des macros pratiques pour comparer des tableaux élément par élément avec des messages d'erreur détaillés. Ces macros sont construites au-dessus des macros d'assertion de Google Test.

### Exemple Rapide

```cpp
TEST_G(ArrayTest, CompareArrays) {
    USE_GENERATOR();

    int expected[] = {1, 2, 3, 4, 5};
    int actual[] = {1, 2, 3, 4, 5};

    EXPECT_ARRAY_EQ(expected, actual, 5);  // Non-fatal assertion
}
```

### Macros Disponibles

#### Types Entiers et Génériques

- **`EXPECT_ARRAY_EQ(expected, actual, size)`** - Non-fatal : Compare deux tableaux élément par élément
  ```cpp
  int expected[] = {1, 2, 3};
  int actual[] = {1, 2, 3};
  EXPECT_ARRAY_EQ(expected, actual, 3);
  ```

- **`ASSERT_ARRAY_EQ(expected, actual, size)`** - Fatal : Compare deux tableaux élément par élément
  ```cpp
  std::vector<int> expected = {10, 20, 30};
  std::vector<int> actual = {10, 20, 30};
  ASSERT_ARRAY_EQ(expected.data(), actual.data(), 3);  // Test stops if fails
  ```

#### Types à Virgule Flottante

- **`EXPECT_ARRAY_NEAR(expected, actual, size, abs_error)`** - Non-fatal : Compare des tableaux de nombres à virgule flottante avec tolérance
  ```cpp
  double expected[] = {1.0, 2.0, 3.0};
  double actual[] = {1.001, 1.999, 3.002};
  EXPECT_ARRAY_NEAR(expected, actual, 3, 0.01);  // Tolerance: 0.01
  ```

- **`ASSERT_ARRAY_NEAR(expected, actual, size, abs_error)`** - Fatal : Compare des tableaux de nombres à virgule flottante avec tolérance
  ```cpp
  float expected[] = {1.5f, 2.5f, 3.5f};
  float actual[] = {1.501f, 2.499f, 3.502f};
  ASSERT_ARRAY_NEAR(expected, actual, 3, 0.01f);
  ```

- **`EXPECT_ARRAY_DOUBLE_EQ(expected, actual, size)`** - Non-fatal : Compare des tableaux de double avec tolérance par défaut
  ```cpp
  double expected[] = {1.5, 2.5, 3.5};
  double actual[] = {1.5, 2.5, 3.5};
  EXPECT_ARRAY_DOUBLE_EQ(expected, actual, 3);
  ```

- **`EXPECT_ARRAY_FLOAT_EQ(expected, actual, size)`** - Non-fatal : Compare des tableaux de float avec tolérance par défaut
  ```cpp
  float expected[] = {1.25f, 2.25f, 3.25f};
  float actual[] = {1.25f, 2.25f, 3.25f};
  EXPECT_ARRAY_FLOAT_EQ(expected, actual, 3);
  ```

### Messages d'Erreur

Lorsque les tableaux diffèrent, les macros fournissent des messages d'erreur détaillés :

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

### Travailler avec Différents Types de Conteneurs

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

### Combinaison avec GENERATOR

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

### Fonctionnalités Clés

- **Comparaison élément par élément** : Chaque élément est comparé individuellement
- **Messages d'erreur détaillés** : Affiche l'index qui diffère et les valeurs
- **Fonctionne avec tout type comparable** : int, float, double, string, types personnalisés avec operator==
- **Messages de réussite** : Affiche "Arrays are equal" lorsque tous les éléments correspondent
- **Compatible avec les vecteurs et tableaux** : Fonctionne avec les tableaux de style C, std::vector, std::array

### Notes Importantes

1. **Paramètre de taille requis** : Vous devez fournir explicitement la taille du tableau
2. **Fatal vs Non-fatal** : Utilisez ASSERT_* pour les assertions fatales, EXPECT_* pour les non-fatales
3. **Comparaisons à virgule flottante** : Utilisez NEAR, FLOAT_EQ ou DOUBLE_EQ pour les valeurs à virgule flottante
4. **Types personnalisés** : Votre type doit avoir operator== défini pour EXPECT_ARRAY_EQ
5. **Tableaux de taille zéro** : Fonctionne correctement avec les tableaux vides (taille = 0)

Voir `test_array_compare.cpp` pour des exemples complets.

## Améliorations Futures

- Calcul dynamique du nombre total de combinaisons
- Support de différents types de données dans les générateurs
- Instanciations de tests nommées
- Support de motifs de valeurs plus complexes

## Licence

Ce projet est fourni tel quel à des fins éducatives et de développement.
