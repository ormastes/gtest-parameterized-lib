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

## Licence

Ce projet est fourni tel quel à des fins éducatives et de développement.