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
    // IMPORTANT : Tous les appels GENERATOR() doivent venir AVANT USE_GENERATOR()
    int a = GENERATOR(1, 2);      // Sera 1 ou 2
    int b = GENERATOR(10, 20);    // Sera 10 ou 20
    USE_GENERATOR();              // Doit être appelé APRÈS tous les appels GENERATOR()

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

## Exemples Avancés

### Travailler avec des Classes et Objets

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

### Travailler avec les Conteneurs STL

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
    printf("Vecteur: taille=%d, multiplicateur=%d\n", size, multiplier);
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

### L'Ordre Correct est Critique

✅ **CORRECT** :
```cpp
TEST_G(MyTest, Example) {
    int a = GENERATOR(1, 2);     // D'abord : Définir les générateurs
    int b = GENERATOR(10, 20);   // 
    USE_GENERATOR();             // Ensuite : Appeler USE_GENERATOR()
    // Logique de test ici
}
```

❌ **INCORRECT** :
```cpp
TEST_G(MyTest, Example) {
    USE_GENERATOR();             // Faux ! Ceci doit venir après les appels GENERATOR
    int a = GENERATOR(1, 2);     
    int b = GENERATOR(10, 20);   
    // Logique de test ici
}
```

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

La bibliothèque fournit un moyen sûr au niveau des types pour accéder aux membres privés dans les tests sans utiliser `#define private public` ou modifier le code de production.

### Exemple Rapide

```cpp
// Dans votre classe de production
class MyClass {
private:
    int privateValue;
    std::string privateName;
public:
    MyClass(int v, const std::string& n) : privateValue(v), privateName(n) {}

    // Accorder un accès ami pour les tests
    FRIEND_ACCESS_PRIVATE();
};

// Dans votre fichier de test
using TestBase = gtest_generator::TestWithGenerator;

// Déclarer les accesseurs - passer seulement le nom du champ
DECLARE_ACCESS_PRIVATE(id1, TestBase, MyClass, privateValue);
DECLARE_ACCESS_PRIVATE(id2, TestBase, MyClass, privateName);

TEST_G(MyTest, AccessPrivate) {
    int value = GENERATOR(10, 20);
    USE_GENERATOR();

    MyClass obj(value, "test");

    // Accéder et modifier les membres privés
    int& privateRef = ACCESS_PRIVATE(TestBase, TestBase_MyClass_privateValue,
                                      MyClass, &obj);
    EXPECT_EQ(privateRef, value);
    privateRef = 100;
    EXPECT_EQ(privateRef, 100);
}
```

### Fonctionnalités Clés

- **Sûr au niveau des types** : Utilise la spécialisation de templates et les déclarations d'ami
- **Zéro surcharge** : Mécanisme entièrement au moment de la compilation
- **Sûr pour la production** : `FRIEND_ACCESS_PRIVATE()` peut être défini comme une macro vide dans les builds de production
- **Partageable** : Le bloc de déclaration (lignes 260-274 dans `gtest_generator.h`) peut être copié dans les en-têtes communs

### Notes Importantes

1. **Utiliser des alias de types** : Le paramètre TestCase ne peut pas contenir `::`, utilisez `using TestBase = gtest_generator::TestWithGenerator;`
2. **Noms de champs uniquement** : Passez seulement le nom du champ (par exemple, `privateValue`), pas `&MyClass::privateValue`
3. **IDs auto-générés** : Les IDs suivent le modèle `TestCase_TargetClass_MemberName` (par exemple, `TestBase_MyClass_privateValue`)

### Utilisation Avancée

**Membres Statiques :**
```cpp
DECLARE_ACCESS_PRIVATE_STATIC(TestBase, MyClass, staticCounter);
```

**Fonctions d'Accesseur Personnalisées :**
```cpp
DECLARE_ACCESS_PRIVATE_FUNCTION(TestBase, MyClass, CustomAccess) {
    return target->privateField1 + target->privateField2;
}
```

Voir `test_private_access.cpp` et `example_common_header.h` pour des exemples complets.

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

1. **Le paramètre de taille est requis** : Vous devez explicitement fournir la taille du tableau
2. **Fatal vs Non-fatal** : Utilisez ASSERT_* pour les assertions fatales, EXPECT_* pour les non-fatales
3. **Comparaisons de nombres à virgule flottante** : Utilisez NEAR, FLOAT_EQ, ou DOUBLE_EQ pour les valeurs à virgule flottante
4. **Types personnalisés** : Vos types doivent avoir operator== défini pour EXPECT_ARRAY_EQ
5. **Tableaux de taille zéro** : Fonctionne correctement avec les tableaux vides (size = 0)

Voir `test_array_compare.cpp` pour des exemples complets.

## Améliorations Futures

- Calcul dynamique du nombre total de combinaisons
- Prise en charge de différents types de données dans les générateurs
- Instanciations de tests nommées
- Prise en charge de modèles de valeurs plus complexes

## Licence

Ce projet est fourni tel quel à des fins éducatives et de développement.