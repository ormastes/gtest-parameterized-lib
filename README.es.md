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
    // ⚠️ CRÍTICO: Las llamadas GENERATOR() DEBEN estar en la PARTE SUPERIOR, ANTES de USE_GENERATOR()
    // Este orden es obligatorio - la biblioteca cuenta los generadores antes de ejecutar la prueba
    int a = GENERATOR(1, 2);      // Será 1 o 2
    int b = GENERATOR(10, 20);    // Será 10 o 20
    USE_GENERATOR();              // DEBE llamarse DESPUÉS de todas las llamadas GENERATOR()

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

## Modos de Muestreo

La biblioteca soporta dos modos de muestreo para generar combinaciones de prueba:

### Modo FULL (Predeterminado - Producto Cartesiano)
El modo predeterminado genera todas las combinaciones posibles de valores (producto cartesiano). Este es el comportamiento tradicional que asegura una cobertura de prueba completa.

```cpp
TEST_G(MyTest, FullMode) {
    USE_GENERATOR();  // El predeterminado es modo FULL
    // o explícitamente: USE_GENERATOR(FULL);
    
    auto x = GENERATOR(1, 2);      // 2 valores
    auto y = GENERATOR(10, 20);    // 2 valores
    auto z = GENERATOR(100, 200);  // 2 valores
    
    // Genera 8 ejecuciones de prueba: 2 × 2 × 2 = 8 combinaciones
    // (1,10,100), (1,10,200), (1,20,100), (1,20,200),
    // (2,10,100), (2,10,200), (2,20,100), (2,20,200)
}
```

### Modo ALIGNED (Iteración Paralela)
El modo ALIGNED itera a través de todas las columnas en paralelo, como una cremallera. Cada columna avanza a su siguiente valor en cada ejecución, volviéndose al inicio cuando llega al final. El número total de ejecuciones es igual al tamaño de la columna más grande.

```cpp
TEST_G(MyTest, AlignedMode) {
    USE_GENERATOR(ALIGNED);  // Habilitar modo ALIGNED
    
    auto x = GENERATOR(1, 2);           // 2 valores
    auto y = GENERATOR(10, 20, 30, 40); // 4 valores  
    auto z = GENERATOR(100, 200, 300);  // 3 valores
    
    // Genera 4 ejecuciones de prueba (tamaño máximo de columna):
    // Ejecución 0: (1, 10, 100)  - todos en índice 0
    // Ejecución 1: (2, 20, 200)  - todos en índice 1
    // Ejecución 2: (1, 30, 300)  - x vuelve a 0, otros en índice 2
    // Ejecución 3: (2, 40, 100)  - x en 1, y en 3, z vuelve a 0
}
```

#### Características Clave del Modo ALIGNED:
- **Determinístico**: Los valores se seleccionan en orden (0, 1, 2, ...) con envolvimiento
- **Orden de Declaración**: Las columnas se procesan en el orden en que se declaran
- **Menos Ejecuciones**: Total de ejecuciones = tamaño máximo de columna (no el producto)
- **Cobertura Equilibrada**: Cada valor en cada columna se usa aproximadamente por igual

#### Detalles de Implementación:
La biblioteca implementa el modo ALIGNED eficientemente usando el mecanismo `GTEST_SKIP()` de Google Test:
1. Durante el registro de pruebas, la biblioteca cuenta el tamaño de cada columna GENERATOR
2. En tiempo de ejecución, la biblioteca calcula el tamaño máximo de columna
3. Las ejecuciones de prueba más allá del tamaño máximo se omiten usando `GTEST_SKIP()`
4. Cada columna se envuelve cuando llega a su final (usando operador módulo)

Este enfoque asegura que:
- Solo se ejecutan las pruebas necesarias (no todos los productos cartesianos)
- El recuento de pruebas reportado por Google Test refleja las pruebas realmente ejecutadas
- No hay penalización de rendimiento por generar combinaciones innecesarias

#### Cuándo Usar Cada Modo:
- **Modo FULL**: Cuando necesitas pruebas exhaustivas de todas las combinaciones
- **Modo ALIGNED**: Cuando quieres muestreo representativo con menos ejecuciones de prueba

#### Ejemplo de Comparación:
```cpp
// Modo FULL: 3 × 2 × 2 = 12 ejecuciones
TEST_G(MyTest, FullExample) {
    USE_GENERATOR(FULL);
    auto a = GENERATOR(1, 2, 3);
    auto b = GENERATOR(10, 20);
    auto c = GENERATOR(100, 200);
    // Genera todas las 12 combinaciones
}

// Modo ALIGNED: max(3, 2, 2) = 3 ejecuciones
TEST_G(MyTest, AlignedExample) {
    USE_GENERATOR(ALIGNED);
    auto a = GENERATOR(1, 2, 3);
    auto b = GENERATOR(10, 20);
    auto c = GENERATOR(100, 200);
    // Genera solo 3 combinaciones:
    // (1, 10, 100), (2, 20, 200), (3, 10, 100)
}
```

#### Ejemplo de Prueba de Verificación:
El proyecto incluye `test_mode_counts.cpp` que verifica exhaustivamente que ambos modos funcionan correctamente:

```cpp
// Prueba con configuración 3x2x2
TEST_G(ModeCountTest, Full_3x2x2) {
    int a = GENERATOR(1, 2, 3);        // 3 valores
    int b = GENERATOR(10, 20);         // 2 valores
    int c = GENERATOR(100, 200);       // 2 valores
    USE_GENERATOR(FULL);

    // Verifica: genera exactamente 12 casos de prueba (3 × 2 × 2)
    // Todas las combinaciones son únicas
}

TEST_G(ModeCountTest, Aligned_3x2x2) {
    int a = GENERATOR(1, 2, 3);        // 3 valores
    int b = GENERATOR(10, 20);         // 2 valores
    int c = GENERATOR(100, 200);       // 2 valores
    USE_GENERATOR(ALIGNED);

    // Verifica: genera exactamente 3 casos de prueba (tamaño máximo de columna)
    // Resultados: (1,10,100), (2,20,200), (3,10,100)
}
```

Ejecute la prueba de verificación para ver la salida detallada:
```bash
./build/test_mode_counts
```

La salida esperada muestra:
- Modo FULL 3x2x2: 12 ejecuciones con todas las combinaciones únicas
- Modo ALIGNED 3x2x2: 3 ejecuciones (tamaño máximo de columna)
- Modo FULL 2x3x4: 24 ejecuciones con todas las combinaciones únicas
- Modo ALIGNED 2x3x4: 4 ejecuciones (tamaño máximo de columna)
- Modo FULL 5x1x3: 15 ejecuciones con todas las combinaciones únicas
- Modo ALIGNED 5x1x3: 5 ejecuciones (tamaño máximo de columna)

## Referencia de API

### Macros

- **`TEST_G(TestClassName, TestName)`** - Define una prueba parametrizada con generación automática de pruebas. Reemplaza tanto TEST_P como la necesidad de llamadas ENABLE_GENERATOR separadas.

- **`GENERATOR(...)`** - Define valores para un parámetro de prueba. Cada llamada crea una nueva dimensión de combinaciones de prueba.
  ```cpp
  int value = GENERATOR(1, 2, 3);  // Crea 3 variantes
  ```
  **IMPORTANTE**: Todas las llamadas GENERATOR() deben venir ANTES de USE_GENERATOR()

- **`USE_GENERATOR()`** - Debe llamarse una vez en cada TEST_G, DESPUÉS de todas las llamadas GENERATOR(). Usa el modo FULL por defecto.

- **`USE_GENERATOR(mode)`** - Debe llamarse una vez en cada TEST_G, DESPUÉS de todas las llamadas GENERATOR(). Especifica el modo de muestreo:
  - `USE_GENERATOR(FULL)` - Producto cartesiano de todos los valores (igual al predeterminado)
  - `USE_GENERATOR(ALIGNED)` - Iteración paralela a través de columnas

## Cómo Funciona

La biblioteca usa variables estáticas thread-local para mantener el estado del generador entre instanciaciones de prueba. Cuando se llama `GENERATOR`:

1. Registra los valores posibles para ese generador
2. Basándose en el índice del parámetro de prueba actual, calcula qué valor devolver
3. Todas las combinaciones se generan automáticamente

## Notas Importantes de Uso

### ⚠️ CRÍTICO: Orden de GENERATOR y USE_GENERATOR

**MUY IMPORTANTE:** Todas las llamadas `GENERATOR()` **DEBEN** estar en la **PARTE SUPERIOR** del caso de prueba, **ANTES** de `USE_GENERATOR()`. Este orden es obligatorio para que la biblioteca funcione correctamente.

✅ **CORRECTO**:
```cpp
TEST_G(MyTest, Example) {
    // Paso 1: TODAS las llamadas GENERATOR() primero en la PARTE SUPERIOR
    int a = GENERATOR(1, 2);
    int b = GENERATOR(10, 20);

    // Paso 2: USE_GENERATOR() DESPUÉS de todos los generadores
    USE_GENERATOR();

    // Paso 3: Tu lógica de prueba aquí
    EXPECT_LT(a, b);
}
```

❌ **INCORRECTO - NO funcionará**:
```cpp
TEST_G(MyTest, Example) {
    USE_GENERATOR();             // ❌ ¡MAL! Debe venir después de los generadores
    int a = GENERATOR(1, 2);     // ❌ ¡Demasiado tarde!
    int b = GENERATOR(10, 20);
}
```

❌ **INCORRECTO - NO funcionará**:
```cpp
TEST_G(MyTest, Example) {
    int a = GENERATOR(1, 2);
    USE_GENERATOR();             // ❌ ¡MAL! Debe venir después de TODOS los generadores
    int b = GENERATOR(10, 20);   // ❌ Este generador está después de USE_GENERATOR()
}
```

**Por qué esto importa:**
- La biblioteca cuenta los generadores durante una fase de pre-ejecución
- `USE_GENERATOR()` marca el final de la fase de declaración de generadores
- Los generadores declarados después de `USE_GENERATOR()` serán ignorados o causarán errores

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

## Acceso a Miembros Privados para Pruebas

La biblioteca proporciona una forma simple y limpia de acceder a miembros privados en pruebas usando declaraciones friend explícitas.

### Ejemplo Rápido

```cpp
// En su clase de producción
class MyClass {
private:
    int privateValue;
    std::string privateName;

    int computeSecret(int x) const { return privateValue * x; }

public:
    MyClass(int v, const std::string& n) : privateValue(v), privateName(n) {}

    // Conceder acceso friend a pruebas específicas
    GTESTG_FRIEND_TEST(MyClassTest, AccessPrivateMembers);
    GTESTG_FRIEND_TEST(MyClassTest, ModifyPrivateMembers);
};

// En su archivo de prueba
struct MyClassTest : ::testing::Test {
    MyClass obj{42, "secret"};
};

TEST_FRIEND(MyClassTest, AccessPrivateMembers) {
    // ¡Acceso directo a miembros privados!
    EXPECT_EQ(obj.privateValue, 42);
    EXPECT_EQ(obj.privateName, "secret");
}

TEST_FRIEND(MyClassTest, ModifyPrivateMembers) {
    // Puede modificar miembros privados
    obj.privateValue = 100;
    EXPECT_EQ(obj.privateValue, 100);

    // Puede llamar métodos privados
    int result = obj.computeSecret(2);
    EXPECT_EQ(result, 200);
}
```

### Características Clave

- **Simple y Limpio**: Usa declaraciones friend estándar de C++
- **Acceso Selectivo**: Conceda acceso solo a las pruebas específicas que lo necesitan
- **Cero Sobrecarga**: Mecanismo puro en tiempo de compilación, sin costo en tiempo de ejecución
- **Seguridad de Tipos**: Seguridad de tipos garantizada por el compilador
- **Seguro para Producción**: Las declaraciones friend no tienen impacto en tiempo de ejecución

### Referencia de API

#### Macros para Clases Objetivo

| Macro | Propósito | Uso |
|-------|---------|-------|
| `GTESTG_FRIEND_TEST(Suite, TestName)` | Conceder acceso friend a una prueba TEST_F específica | Colocar en definición de clase |
| `GTESTG_FRIEND_TEST_G(TestClassName, TestName)` | Conceder acceso friend a una prueba TEST_G | Colocar en definición de clase |
| `GTESTG_FRIEND_TEST_SUITE(Suite)` | Conceder acceso friend a todas las pruebas en una suite | Colocar en definición de clase |

#### Macros para Archivos de Prueba

| Macro | Propósito | Uso |
|-------|---------|-------|
| `TEST_FRIEND(Suite, TestName)` | Definir una prueba con acceso friend | Igual que TEST_F |
| `TEST_G_FRIEND(TestClassName, TestName)` | Definir una prueba generadora con acceso friend | Igual que TEST_G |

### Ejemplos de Uso

#### Acceso Privado Básico
```cpp
// Clase objetivo
class Widget {
private:
    int secret_ = 42;
public:
    // Conceder acceso a prueba específica
    GTESTG_FRIEND_TEST(WidgetTest, CheckSecret);
};

// Archivo de prueba
struct WidgetTest : ::testing::Test {
    Widget w;
};

TEST_FRIEND(WidgetTest, CheckSecret) {
    EXPECT_EQ(w.secret_, 42);  // Acceso directo a miembro privado
}
```

#### Pruebas Generadoras con Acceso Privado
```cpp
// Clase objetivo
class Calculator {
private:
    int factor_ = 10;
    int multiply(int x) const { return x * factor_; }
public:
    // Conceder acceso a prueba generadora
    GTESTG_FRIEND_TEST_G(CalcTest, TestMultiply);
};

// Archivo de prueba
struct CalcTest : ::gtest_generator::TestWithGenerator {
    Calculator calc;
};

TEST_G_FRIEND(CalcTest, TestMultiply) {
    int input = GENERATOR(1, 2, 3);
    USE_GENERATOR();

    // Acceso a método y miembro privados
    int result = calc.multiply(input);
    EXPECT_EQ(result, input * calc.factor_);
}
```

#### Conceder Acceso a Todas las Pruebas en una Suite
```cpp
// Clase objetivo - concede acceso a TODAS las pruebas en MyTestSuite
class MyClass {
private:
    int value_ = 100;
public:
    // Conceder acceso a toda la suite de pruebas
    GTESTG_FRIEND_TEST_SUITE(MyTestSuite);
};

// Archivo de prueba - todas las pruebas en esta suite tienen acceso
struct MyTestSuite : ::testing::Test {
    MyClass obj;
};

TEST_F(MyTestSuite, Test1) {
    EXPECT_EQ(obj.value_, 100);  // Tiene acceso
}

TEST_F(MyTestSuite, Test2) {
    obj.value_ = 200;  // También tiene acceso
    EXPECT_EQ(obj.value_, 200);
}
```

#### Trabajando con Herencia
```cpp
// Clase base
class Base {
private:
    int base_secret_ = 10;
public:
    GTESTG_FRIEND_TEST(DerivedTest, AccessBoth);
};

// Clase derivada
class Derived : public Base {
private:
    int derived_secret_ = 20;
public:
    GTESTG_FRIEND_TEST(DerivedTest, AccessBoth);
};

// Prueba
struct DerivedTest : ::testing::Test {
    Derived d;
};

TEST_FRIEND(DerivedTest, AccessBoth) {
    EXPECT_EQ(d.base_secret_, 10);     // Acceso a privado de base
    EXPECT_EQ(d.derived_secret_, 20);  // Acceso a privado de derivada
}
```

### Notas Importantes

1. **Se Requiere Concesión Explícita**: Cada prueba que necesite acceso privado debe ser listada explícitamente en la clase objetivo
2. **Sin Magia**: Usa declaraciones friend estándar de C++ - simple y predecible
3. **TEST_FRIEND es Opcional**: `TEST_FRIEND` es solo una macro de conveniencia que mapea a `TEST_F`. Puede usar `TEST_F` regular si la clase tiene la declaración `GTESTG_FRIEND_TEST` apropiada
4. **Seguridad en Tiempo de Compilación**: Si una prueba intenta acceder a miembros privados sin tener acceso friend concedido, obtendrá un error de compilación
5. **Mantenimiento**: Cuando agregue una nueva prueba que necesite acceso privado, recuerde agregar la declaración `GTESTG_FRIEND_TEST` correspondiente a la clase objetivo

### Cuándo Usar Esta Característica

Use el acceso a miembros privados cuando:
- Pruebe estado interno que no está expuesto a través de la interfaz pública
- Verifique lógica privada compleja
- Configure estados internos específicos para pruebas
- Pruebe código heredado que no puede ser fácilmente refactorizado

Evite usar cuando:
- La necesidad de acceso privado indique un diseño pobre
- La prueba de interfaz pública sería suficiente
- Crearía un acoplamiento estrecho entre pruebas e implementación

Consulte `test_friend_access.cpp` para ejemplos completos.

### Macros TEST_FRIEND y TEST_G_FRIEND

La biblioteca proporciona macros `TEST_FRIEND` y `TEST_G_FRIEND` que crean infraestructura de prueba con soporte integrado para el patrón VirtualAccessor. Estas macros funcionan sin problemas con la declaración `GTESTG_FRIEND_ACCESS_PRIVATE()`.

**Puntos Clave:**
- `GTESTG_FRIEND_ACCESS_PRIVATE()` otorga acceso friend para **ambos** enfoques basados en clases (VirtualAccessor) y basados en funciones (gtestg_private_accessMember)
- Use `TEST_FRIEND` para pruebas estilo TEST_F regulares
- Use `TEST_G_FRIEND` para pruebas parametrizadas basadas en generadores
- Continúe usando macros `GTESTG_PRIVATE_MEMBER` para acceder a miembros privados

**Ejemplo con TEST_FRIEND:**
```cpp
class Widget {
private:
    int secret_ = 42;
public:
    Widget() = default;

    // Una sola macro otorga ambos tipos de acceso friend
    GTESTG_FRIEND_ACCESS_PRIVATE();
};

// Declarar accessor
GTESTG_PRIVATE_DECLARE_MEMBER(Widget, secret_);

struct WidgetTest : ::testing::Test {
    Widget w;
};

TEST_FRIEND(WidgetTest, AccessPrivate) {
    // Acceder a miembro privado usando accessor basado en función
    int& secret = GTESTG_PRIVATE_MEMBER(Widget, secret_, &w);
    EXPECT_EQ(secret, 42);
    secret = 100;
    EXPECT_EQ(secret, 100);
}
```

**Ejemplo con TEST_G_FRIEND:**
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

**Soporte Multi-Archivo:**
`TEST_FRIEND` y `TEST_G_FRIEND` funcionan correctamente cuando las pruebas se definen en múltiples archivos .cpp vinculados al mismo ejecutable, al igual que `TEST_G` regular. Vea `test_friend_multi_file1.cpp` y `test_friend_multi_file2.cpp` para ejemplos.

## Macros de Comparación de Arrays

La biblioteca proporciona macros convenientes para comparar arrays elemento por elemento con mensajes de error detallados. Estas macros están construidas sobre las macros de aserción de Google Test.

### Ejemplo Rápido

```cpp
TEST_G(ArrayTest, CompareArrays) {
    USE_GENERATOR();

    int expected[] = {1, 2, 3, 4, 5};
    int actual[] = {1, 2, 3, 4, 5};

    EXPECT_ARRAY_EQ(expected, actual, 5);  // Non-fatal assertion
}
```

### Macros Disponibles

#### Tipos Enteros y Genéricos

- **`EXPECT_ARRAY_EQ(expected, actual, size)`** - No fatal: Compara dos arrays elemento por elemento
  ```cpp
  int expected[] = {1, 2, 3};
  int actual[] = {1, 2, 3};
  EXPECT_ARRAY_EQ(expected, actual, 3);
  ```

- **`ASSERT_ARRAY_EQ(expected, actual, size)`** - Fatal: Compara dos arrays elemento por elemento
  ```cpp
  std::vector<int> expected = {10, 20, 30};
  std::vector<int> actual = {10, 20, 30};
  ASSERT_ARRAY_EQ(expected.data(), actual.data(), 3);  // Test stops if fails
  ```

#### Tipos de Punto Flotante

- **`EXPECT_ARRAY_NEAR(expected, actual, size, abs_error)`** - No fatal: Compara arrays de punto flotante con tolerancia
  ```cpp
  double expected[] = {1.0, 2.0, 3.0};
  double actual[] = {1.001, 1.999, 3.002};
  EXPECT_ARRAY_NEAR(expected, actual, 3, 0.01);  // Tolerance: 0.01
  ```

- **`ASSERT_ARRAY_NEAR(expected, actual, size, abs_error)`** - Fatal: Compara arrays de punto flotante con tolerancia
  ```cpp
  float expected[] = {1.5f, 2.5f, 3.5f};
  float actual[] = {1.501f, 2.499f, 3.502f};
  ASSERT_ARRAY_NEAR(expected, actual, 3, 0.01f);
  ```

- **`EXPECT_ARRAY_DOUBLE_EQ(expected, actual, size)`** - No fatal: Compara arrays de double con tolerancia predeterminada
  ```cpp
  double expected[] = {1.5, 2.5, 3.5};
  double actual[] = {1.5, 2.5, 3.5};
  EXPECT_ARRAY_DOUBLE_EQ(expected, actual, 3);
  ```

- **`EXPECT_ARRAY_FLOAT_EQ(expected, actual, size)`** - No fatal: Compara arrays de float con tolerancia predeterminada
  ```cpp
  float expected[] = {1.25f, 2.25f, 3.25f};
  float actual[] = {1.25f, 2.25f, 3.25f};
  EXPECT_ARRAY_FLOAT_EQ(expected, actual, 3);
  ```

### Mensajes de Error

Cuando los arrays difieren, las macros proporcionan mensajes de error detallados:

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

### Trabajando con Diferentes Tipos de Contenedores

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

### Combinando con GENERATOR

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

### Características Clave

- **Comparación elemento por elemento**: Cada elemento se compara individualmente
- **Mensajes de error detallados**: Muestra qué índice difiere y los valores
- **Funciona con cualquier tipo comparable**: int, float, double, string, tipos personalizados con operator==
- **Mensajes de éxito**: Muestra "Arrays are equal" cuando todos los elementos coinciden
- **Compatible con vectores y arrays**: Funciona con arrays estilo C, std::vector, std::array

### Notas Importantes

1. **Parámetro de tamaño es requerido**: Debe proporcionar explícitamente el tamaño del array
2. **Fatal vs No fatal**: Use ASSERT_* para aserciones fatales, EXPECT_* para no fatales
3. **Comparaciones de punto flotante**: Use NEAR, FLOAT_EQ o DOUBLE_EQ para valores de punto flotante
4. **Tipos personalizados**: Su tipo debe tener operator== definido para EXPECT_ARRAY_EQ
5. **Arrays de tamaño cero**: Funciona correctamente con arrays vacíos (tamaño = 0)

Consulte `test_array_compare.cpp` para ejemplos completos.

## Mejoras Futuras

- Cálculo dinámico del número total de combinaciones
- Soporte para diferentes tipos de datos en generadores
- Instanciaciones de test con nombre
- Soporte para patrones de valores más complejos

## Licencia

Este proyecto se proporciona tal cual para fines educativos y de desarrollo.
