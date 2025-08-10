# GTest Generator - Biblioteca de Pruebas Parametrizadas

Una biblioteca simple de solo encabezados que proporciona sintaxis estilo Catch2 `GENERATE` para pruebas parametrizadas de Google Test.

## Características Principales

- Sintaxis simple e intuitiva similar a `GENERATE` de Catch2
- Implementación de solo encabezados
- Generación automática de todas las combinaciones de prueba
- Compatible con macros estándar `TEST_P` de Google Test

## Inicio Rápido

```cpp
#include "gtest_generator.h"

// Definir el fixture de prueba usando TestWithGenerator
class MyTest : public ::gtest_generator::TestWithGenerator {
};

// Escribir prueba parametrizada usando la macro TEST_G
TEST_G(MyTest, SimpleCase) {
    // IMPORTANTE: Todas las llamadas GENERATOR() deben venir ANTES de USE_GENERATOR()
    int a = GENERATOR(1, 2);      // Será 1 o 2
    int b = GENERATOR(10, 20);    // Será 10 o 20
    USE_GENERATOR();              // Debe llamarse DESPUÉS de todas las llamadas GENERATOR()

    EXPECT_LT(a, b);
    printf("Prueba: a=%d, b=%d\n", a, b);
}
// ¡TEST_G maneja automáticamente la generación de pruebas - no se necesita ENABLE_GENERATOR!
```

## Instalación

Esta es una biblioteca de solo encabezados. Simplemente copie `gtest_generator.h` a su proyecto e inclúyalo.

## Construir el Proyecto

### Requisitos Previos

- CMake 3.14 o superior
- Biblioteca Google Test
- Compilador compatible con C++14

### Instrucciones de Construcción

```bash
# Crear directorio de construcción
mkdir build && cd build

# Configurar con CMake
cmake ..

# Construir el proyecto
cmake --build .

# Ejecutar las pruebas
./mygen_test
```

## Más Ejemplos

### Múltiples Valores

```cpp
TEST_G(MyTest, MultipleValues) {
    int x = GENERATOR(1, 2, 3);       // 3 valores
    int y = GENERATOR(10, 20, 30);    // 3 valores
    USE_GENERATOR();
    
    // Genera 9 combinaciones de prueba
    EXPECT_LT(x, y);
}
```

### Diferente Número de Valores

```cpp
TEST_G(MyTest, AsymmetricCombinations) {
    int size = GENERATOR(1, 2, 3);     // 3 valores
    int scale = GENERATOR(10, 100);    // 2 valores
    USE_GENERATOR();
    
    // Genera 6 combinaciones de prueba
    int result = size * scale;
    EXPECT_GT(result, 0);
}
```

## Ejemplos Avanzados

### Trabajando con Clases y Objetos

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
    // Generar objetos completos directamente
    auto obj1 = GENERATOR(TestObject(1, "primero"), TestObject(2, "segundo"));
    auto obj2 = GENERATOR(TestObject(10, "diez"), TestObject(20, "veinte"));
    USE_GENERATOR();
    
    EXPECT_LT(obj1, obj2);
    printf("Objetos: obj1={%d, %s}, obj2={%d, %s}\n", 
           obj1.value, obj1.name.c_str(), 
           obj2.value, obj2.name.c_str());
}
```

### Trabajando con Contenedores STL

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
    printf("Vector: tamaño=%d, multiplicador=%d\n", size, multiplier);
}
```

## Referencia de API

### Macros

- **`TEST_G(TestClassName, TestName)`** - Define una prueba parametrizada con generación automática de pruebas. Reemplaza tanto TEST_P como la necesidad de llamadas ENABLE_GENERATOR separadas.

- **`GENERATOR(...)`** - Define valores para un parámetro de prueba. Cada llamada crea una nueva dimensión de combinaciones de prueba.
  ```cpp
  int value = GENERATOR(1, 2, 3);  // Crea 3 variantes
  ```
  **IMPORTANTE**: Todas las llamadas GENERATOR() deben venir ANTES de USE_GENERATOR()

- **`USE_GENERATOR()`** - Debe llamarse una vez en cada TEST_G, DESPUÉS de todas las llamadas GENERATOR().

## Cómo Funciona

La biblioteca usa variables estáticas thread-local para mantener el estado del generador entre instanciaciones de prueba. Cuando se llama `GENERATOR`:

1. Registra los valores posibles para ese generador
2. Basándose en el índice del parámetro de prueba actual, calcula qué valor devolver
3. Todas las combinaciones se generan automáticamente

## Notas Importantes de Uso

### El Orden Correcto es Crítico

✅ **CORRECTO**:
```cpp
TEST_G(MyTest, Example) {
    int a = GENERATOR(1, 2);     // Primero: Definir generadores
    int b = GENERATOR(10, 20);   // 
    USE_GENERATOR();             // Luego: Llamar USE_GENERATOR()
    // Lógica de prueba aquí
}
```

❌ **INCORRECTO**:
```cpp
TEST_G(MyTest, Example) {
    USE_GENERATOR();             // ¡Incorrecto! Esto debe venir después de las llamadas GENERATOR
    int a = GENERATOR(1, 2);     
    int b = GENERATOR(10, 20);   
    // Lógica de prueba aquí
}
```

## Salida de Ejemplo

```
[==========] Ejecutando 4 pruebas de 1 suite de pruebas.
[----------] 4 pruebas de Generator/MyTest__SimpleCase
[ RUN      ] Generator/MyTest__SimpleCase.__/0
Prueba: a=1, b=10
[       OK ] Generator/MyTest__SimpleCase.__/0 (0 ms)
[ RUN      ] Generator/MyTest__SimpleCase.__/1
Prueba: a=2, b=10
[       OK ] Generator/MyTest__SimpleCase.__/1 (0 ms)
[ RUN      ] Generator/MyTest__SimpleCase.__/2
Prueba: a=1, b=20
[       OK ] Generator/MyTest__SimpleCase.__/2 (0 ms)
[ RUN      ] Generator/MyTest__SimpleCase.__/3
Prueba: a=2, b=20
[       OK ] Generator/MyTest__SimpleCase.__/3 (0 ms)
[==========] 4 pruebas de 1 suite de pruebas ejecutadas. (0 ms en total)
[  PASARON  ] 4 pruebas.
```

## Limitaciones

- El fixture de prueba debe heredar de `::gtest_generator::TestWithGenerator`
- Debe usar la macro `TEST_G` en lugar del `TEST_P` estándar
- Todas las llamadas `GENERATOR()` deben venir antes de `USE_GENERATOR()`
- Los tipos complejos (objetos, punteros) funcionan con GENERATOR pero pueden requerir instanciación de plantilla adecuada
- La gestión de memoria es responsabilidad del usuario al generar punteros con `new`

## Licencia

Este proyecto se proporciona tal cual para fines educativos y de desarrollo.