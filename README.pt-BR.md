# GTest Generator - Biblioteca de Testes Parametrizados

Uma biblioteca simples apenas de cabeçalho que fornece sintaxe estilo Catch2 `GENERATE` para testes parametrizados do Google Test.

## Recursos Principais

- Sintaxe simples e intuitiva similar ao `GENERATE` do Catch2
- Implementação apenas de cabeçalho
- Geração automática de todas as combinações de teste
- Compatível com macros padrão `TEST_P` do Google Test

## Início Rápido

```cpp
#include "gtest_generator.h"

// Definir a fixture de teste usando TestWithGenerator
class MyTest : public ::gtest_generator::TestWithGenerator {
};

// Escrever teste parametrizado usando a macro TEST_G
TEST_G(MyTest, SimpleCase) {
    // ⚠️ CRÍTICO: Chamadas GENERATOR() DEVEM estar no TOPO, ANTES de USE_GENERATOR()
    // Esta ordem é obrigatória - a biblioteca conta os geradores antes de executar o teste
    int a = GENERATOR(1, 2);      // Será 1 ou 2
    int b = GENERATOR(10, 20);    // Será 10 ou 20
    USE_GENERATOR();              // DEVE ser chamado DEPOIS de todas as chamadas GENERATOR()

    EXPECT_LT(a, b);
    printf("Teste: a=%d, b=%d\n", a, b);
}
// TEST_G automaticamente gerencia a geração de testes - não precisa de ENABLE_GENERATOR!
```

## Instalação

Esta é uma biblioteca apenas de cabeçalho. Simplesmente copie `gtest_generator.h` para seu projeto e inclua-o.

## Compilando o Projeto

### Pré-requisitos

- CMake 3.14 ou superior
- Biblioteca Google Test
- Compilador compatível com C++14

### Instruções de Compilação

```bash
# Criar diretório de build
mkdir build && cd build

# Configurar com CMake
cmake ..

# Compilar o projeto
cmake --build .

# Executar os testes
./mygen_test
```

## Mais Exemplos

### Múltiplos Valores

```cpp
TEST_G(MyTest, MultipleValues) {
    int x = GENERATOR(1, 2, 3);       // 3 valores
    int y = GENERATOR(10, 20, 30);    // 3 valores
    USE_GENERATOR();
    
    // Gera 9 combinações de teste
    EXPECT_LT(x, y);
}
```

### Número Diferente de Valores

```cpp
TEST_G(MyTest, AsymmetricCombinations) {
    int size = GENERATOR(1, 2, 3);     // 3 valores
    int scale = GENERATOR(10, 100);    // 2 valores
    USE_GENERATOR();

    // Gera 6 combinações de teste
    int result = size * scale;
    EXPECT_GT(result, 0);
}
```

### Usando Valores Gerados em Expressões
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

### Lógica de Teste Complexa
```cpp
TEST_G(MyTest, ConditionalLogic) {
    int mode = GENERATOR(0, 1, 2);     // 3 modos
    int value = GENERATOR(100, 200);   // 2 valores
    USE_GENERATOR();

    int result;
    switch (mode) {
        case 0: result = value + 10; break;
        case 1: result = value * 2; break;
        case 2: result = value - 50; break;
    }

    EXPECT_GT(result, 0);
    printf("Modo %d com valor %d resulta em %d\n", mode, value, result);
}
```

## Exemplos Avançados

### Trabalhando com Classes e Objetos

#### Geração Direta de Objetos
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
    // Gerar objetos completos diretamente
    auto obj1 = GENERATOR(TestObject(1, "primeiro"), TestObject(2, "segundo"));
    auto obj2 = GENERATOR(TestObject(10, "dez"), TestObject(20, "vinte"));
    USE_GENERATOR();

    EXPECT_LT(obj1, obj2);
    printf("Objetos: obj1={%d, %s}, obj2={%d, %s}\n",
           obj1.value, obj1.name.c_str(),
           obj2.value, obj2.name.c_str());
}
```

#### Usando GENERATOR em Argumentos do Construtor
```cpp
TEST_G(MyTest, ConstructorWithGenerators) {
    // Valores GENERATOR usados como argumentos do construtor
    int val1 = GENERATOR(1, 2);
    int val2 = GENERATOR(10, 20);
    USE_GENERATOR();

    TestObject objects[] = {
        TestObject(val1, "teste"),
        TestObject(val2, "demo")
    };

    EXPECT_LT(objects[0].value, objects[1].value);
    printf("Objetos de array: [0]={%d,%s}, [1]={%d,%s}\n",
           objects[0].value, objects[0].name.c_str(),
           objects[1].value, objects[1].name.c_str());
}
```

### Trabalhando com Ponteiros e Memória Dinâmica

#### Gerando Ponteiros para Objetos
```cpp
TEST_G(MyTest, PointerGeneration) {
    // Gerar ponteiros para diferentes objetos
    // Nota: Tenha cuidado com o gerenciamento de memória
    auto* ptr1 = GENERATOR(new TestObject(1, "primeiro"),
                          new TestObject(2, "segundo"));
    auto* ptr2 = GENERATOR(new TestObject(10, "dez"),
                          new TestObject(20, "vinte"));
    USE_GENERATOR();

    EXPECT_LT(*ptr1, *ptr2);
    printf("Ponteiros: ptr1={%d, %s}, ptr2={%d, %s}\n",
           ptr1->value, ptr1->name.c_str(),
           ptr2->value, ptr2->name.c_str());

    // Limpar
    delete ptr1;
    delete ptr2;
}
```

#### Chamadas GENERATOR Aninhadas (Avançado)
```cpp
TEST_G(MyTest, NestedGenerators) {
    // Geração aninhada complexa - cada GENERATOR externo contém chamadas GENERATOR internas
    int inner1 = GENERATOR(1, 2);
    int inner2 = GENERATOR(3, 4);
    int inner3 = GENERATOR(10, 20);
    int inner4 = GENERATOR(30, 40);
    USE_GENERATOR();

    auto* obj1 = new TestObject(inner1, "primeiro");
    auto* obj2 = new TestObject(inner3, "segundo");

    EXPECT_LT(obj1->value, obj2->value);
    printf("Aninhado: obj1={%d}, obj2={%d}\n", obj1->value, obj2->value);

    delete obj1;
    delete obj2;
}
```

### Trabalhando com Contêineres STL

#### Gerando Tamanhos e Conteúdos de Contêineres
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

    printf("Vetor: tamanho=%d, multiplicador=%d, elementos=[", size, multiplier);
    for (int v : vec) printf("%d ", v);
    printf("]\n");
}
```

#### Gerando Combinações de Strings
```cpp
TEST_G(MyTest, StringCombinations) {
    auto prefix_choice = GENERATOR(0, 1);
    auto suffix_choice = GENERATOR(0, 1);
    auto repeat = GENERATOR(1, 2);
    USE_GENERATOR();

    std::string prefix = prefix_choice ? "Olá" : "Oi";
    std::string suffix = suffix_choice ? "Mundo" : "Pessoal";

    std::string result;
    for (int i = 0; i < repeat; ++i) {
        if (i > 0) result += " ";
        result += prefix + " " + suffix;
    }

    EXPECT_FALSE(result.empty());
    printf("String: prefix='%s', suffix='%s', repeat=%d => '%s'\n",
           prefix.c_str(), suffix.c_str(), repeat, result.c_str());
}
```

### Trabalhando com Ponteiros Inteligentes

#### Usando unique_ptr com GENERATOR
```cpp
TEST_G(MyTest, SmartPointers) {
    auto value1 = GENERATOR(1, 2);
    auto value2 = GENERATOR(10, 20);
    USE_GENERATOR();

    auto ptr1 = std::make_unique<TestObject>(value1, "primeiro");
    auto ptr2 = std::make_unique<TestObject>(value2, "segundo");

    EXPECT_LT(*ptr1, *ptr2);
    printf("Ponteiros inteligentes: ptr1={%d, %s}, ptr2={%d, %s}\n",
           ptr1->value, ptr1->name.c_str(),
           ptr2->value, ptr2->name.c_str());
}
```

### Exemplos de Estruturas Complexas

#### Gerando Estruturas com Múltiplos Campos
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
    printf("Pontos: p1=(%d,%d), p2=(%d,%d), distância=%d\n",
           p1.x, p1.y, p2.x, p2.y, distance);
}
```

## Modos de Amostragem

A biblioteca suporta dois modos de amostragem para gerar combinações de teste:

### Modo FULL (Padrão - Produto Cartesiano)
O modo padrão gera todas as combinações possíveis de valores (produto cartesiano). Este é o comportamento tradicional que garante cobertura completa de teste.

```cpp
TEST_G(MyTest, FullMode) {
    USE_GENERATOR();  // O padrão é modo FULL
    // ou explicitamente: USE_GENERATOR(FULL);
    
    auto x = GENERATOR(1, 2);      // 2 valores
    auto y = GENERATOR(10, 20);    // 2 valores
    auto z = GENERATOR(100, 200);  // 2 valores
    
    // Gera 8 execuções de teste: 2 × 2 × 2 = 8 combinações
    // (1,10,100), (1,10,200), (1,20,100), (1,20,200),
    // (2,10,100), (2,10,200), (2,20,100), (2,20,200)
}
```

### Modo ALIGNED (Iteração Paralela)
O modo ALIGNED itera através de todas as colunas em paralelo, como um zíper. Cada coluna avança para seu próximo valor a cada execução, voltando ao início quando atinge o final. O número total de execuções é igual ao tamanho da maior coluna.

```cpp
TEST_G(MyTest, AlignedMode) {
    USE_GENERATOR(ALIGNED);  // Habilitar modo ALIGNED
    
    auto x = GENERATOR(1, 2);           // 2 valores
    auto y = GENERATOR(10, 20, 30, 40); // 4 valores  
    auto z = GENERATOR(100, 200, 300);  // 3 valores
    
    // Gera 4 execuções de teste (tamanho máximo da coluna):
    // Execução 0: (1, 10, 100)  - todos no índice 0
    // Execução 1: (2, 20, 200)  - todos no índice 1
    // Execução 2: (1, 30, 300)  - x volta para 0, outros no índice 2
    // Execução 3: (2, 40, 100)  - x em 1, y em 3, z volta para 0
}
```

#### Características Principais do Modo ALIGNED:
- **Determinístico**: Valores são selecionados em ordem (0, 1, 2, ...) com envolvimento
- **Ordem de Declaração**: Colunas são processadas na ordem em que são declaradas
- **Menos Execuções**: Total de execuções = tamanho máximo da coluna (não o produto)
- **Cobertura Equilibrada**: Cada valor em cada coluna é usado aproximadamente igualmente

#### Detalhes de Implementação:
A biblioteca implementa o modo ALIGNED eficientemente usando o mecanismo `GTEST_SKIP()` do Google Test:
1. Durante o registro de testes, a biblioteca conta o tamanho de cada coluna GENERATOR
2. Em tempo de execução, a biblioteca calcula o tamanho máximo da coluna
3. Execuções de teste além do tamanho máximo são puladas usando `GTEST_SKIP()`
4. Cada coluna volta ao início quando atinge o final (usando operador módulo)

Esta abordagem garante que:
- Apenas as execuções de teste necessárias são realizadas (não todos os produtos cartesianos)
- A contagem de testes relatada pelo Google Test reflete os testes realmente executados
- Não há penalidade de desempenho por gerar combinações desnecessárias

#### Quando Usar Cada Modo:
- **Modo FULL**: Quando você precisa de testes exaustivos de todas as combinações
- **Modo ALIGNED**: Quando você quer amostragem representativa com menos execuções de teste

#### Exemplo de Comparação:
```cpp
// Modo FULL: 3 × 2 × 2 = 12 execuções
TEST_G(MyTest, FullExample) {
    USE_GENERATOR(FULL);
    auto a = GENERATOR(1, 2, 3);
    auto b = GENERATOR(10, 20);
    auto c = GENERATOR(100, 200);
    // Gera todas as 12 combinações
}

// Modo ALIGNED: max(3, 2, 2) = 3 execuções
TEST_G(MyTest, AlignedExample) {
    USE_GENERATOR(ALIGNED);
    auto a = GENERATOR(1, 2, 3);
    auto b = GENERATOR(10, 20);
    auto c = GENERATOR(100, 200);
    // Gera apenas 3 combinações:
    // (1, 10, 100), (2, 20, 200), (3, 10, 100)
}
```

#### Exemplo de Teste de Verificação:
O projeto inclui `test_mode_counts.cpp` que verifica de forma abrangente que ambos os modos funcionam corretamente:

```cpp
// Teste com configuração 3x2x2
TEST_G(ModeCountTest, Full_3x2x2) {
    int a = GENERATOR(1, 2, 3);        // 3 valores
    int b = GENERATOR(10, 20);         // 2 valores
    int c = GENERATOR(100, 200);       // 2 valores
    USE_GENERATOR(FULL);

    // Verifica: gera exatamente 12 casos de teste (3 × 2 × 2)
    // Todas as combinações são únicas
}

TEST_G(ModeCountTest, Aligned_3x2x2) {
    int a = GENERATOR(1, 2, 3);        // 3 valores
    int b = GENERATOR(10, 20);         // 2 valores
    int c = GENERATOR(100, 200);       // 2 valores
    USE_GENERATOR(ALIGNED);

    // Verifica: gera exatamente 3 casos de teste (tamanho máximo da coluna)
    // Resultados: (1,10,100), (2,20,200), (3,10,100)
}
```

Execute o teste de verificação para ver a saída detalhada:
```bash
./build/test_mode_counts
```

A saída esperada mostra:
- Modo FULL 3x2x2: 12 execuções com todas as combinações únicas
- Modo ALIGNED 3x2x2: 3 execuções (tamanho máximo da coluna)
- Modo FULL 2x3x4: 24 execuções com todas as combinações únicas
- Modo ALIGNED 2x3x4: 4 execuções (tamanho máximo da coluna)
- Modo FULL 5x1x3: 15 execuções com todas as combinações únicas
- Modo ALIGNED 5x1x3: 5 execuções (tamanho máximo da coluna)

## Referência da API

### Macros

- **`TEST_G(TestClassName, TestName)`** - Define um teste parametrizado com geração automática de testes. Substitui tanto TEST_P quanto a necessidade de chamadas ENABLE_GENERATOR separadas.

- **`GENERATOR(...)`** - Define valores para um parâmetro de teste. Cada chamada cria uma nova dimensão de combinações de teste.
  ```cpp
  int value = GENERATOR(1, 2, 3);  // Cria 3 variantes
  ```
  **IMPORTANTE**: Todas as chamadas GENERATOR() devem vir ANTES de USE_GENERATOR()

- **`USE_GENERATOR()`** - Deve ser chamado uma vez em cada TEST_G, DEPOIS de todas as chamadas GENERATOR(). Usa o modo FULL por padrão.

- **`USE_GENERATOR(mode)`** - Deve ser chamado uma vez em cada TEST_G, DEPOIS de todas as chamadas GENERATOR(). Especifica o modo de amostragem:
  - `USE_GENERATOR(FULL)` - Produto cartesiano de todos os valores (igual ao padrão)
  - `USE_GENERATOR(ALIGNED)` - Iteração paralela através das colunas

## Como Funciona

A biblioteca usa variáveis estáticas thread-local para manter o estado do gerador entre instanciações de teste. Quando `GENERATOR` é chamado:

1. Ele registra os valores possíveis para aquele gerador
2. Baseado no índice do parâmetro de teste atual, calcula qual valor retornar
3. Todas as combinações são geradas automaticamente

## Notas Importantes de Uso

### ⚠️ CRÍTICO: Ordem de GENERATOR e USE_GENERATOR

**MUITO IMPORTANTE:** Todas as chamadas `GENERATOR()` **DEVEM** estar no **TOPO** do caso de teste, **ANTES** de `USE_GENERATOR()`. Esta ordem é obrigatória para o funcionamento correto da biblioteca.

✅ **CORRETO**:
```cpp
TEST_G(MyTest, Example) {
    // Passo 1: TODAS as chamadas GENERATOR() primeiro no TOPO
    int a = GENERATOR(1, 2);
    int b = GENERATOR(10, 20);

    // Passo 2: USE_GENERATOR() DEPOIS de todos os geradores
    USE_GENERATOR();

    // Passo 3: Sua lógica de teste aqui
    EXPECT_LT(a, b);
}
```

❌ **INCORRETO - NÃO funcionará**:
```cpp
TEST_G(MyTest, Example) {
    USE_GENERATOR();             // ❌ ERRADO! Deve vir depois dos geradores
    int a = GENERATOR(1, 2);     // ❌ Tarde demais!
    int b = GENERATOR(10, 20);
}
```

❌ **INCORRETO - NÃO funcionará**:
```cpp
TEST_G(MyTest, Example) {
    int a = GENERATOR(1, 2);
    USE_GENERATOR();             // ❌ ERRADO! Deve vir depois de TODOS os geradores
    int b = GENERATOR(10, 20);   // ❌ Este gerador está depois de USE_GENERATOR()
}
```

**Por que isso importa:**
- A biblioteca conta os geradores durante uma fase de pré-execução
- `USE_GENERATOR()` marca o fim da fase de declaração de geradores
- Geradores declarados após `USE_GENERATOR()` serão ignorados ou causarão erros

## Saída de Exemplo

```
[==========] Executando 4 testes de 1 suíte de testes.
[----------] 4 testes de Generator/MyTest__SimpleCase
[ RUN      ] Generator/MyTest__SimpleCase.__/0
Teste: a=1, b=10
[       OK ] Generator/MyTest__SimpleCase.__/0 (0 ms)
[ RUN      ] Generator/MyTest__SimpleCase.__/1
Teste: a=2, b=10
[       OK ] Generator/MyTest__SimpleCase.__/1 (0 ms)
[ RUN      ] Generator/MyTest__SimpleCase.__/2
Teste: a=1, b=20
[       OK ] Generator/MyTest__SimpleCase.__/2 (0 ms)
[ RUN      ] Generator/MyTest__SimpleCase.__/3
Teste: a=2, b=20
[       OK ] Generator/MyTest__SimpleCase.__/3 (0 ms)
[==========] 4 testes de 1 suíte de testes executados. (0 ms no total)
[  PASSOU  ] 4 testes.
```

## Limitações

- A fixture de teste deve herdar de `::gtest_generator::TestWithGenerator`
- Deve usar a macro `TEST_G` em vez do `TEST_P` padrão
- Todas as chamadas `GENERATOR()` devem vir antes de `USE_GENERATOR()`
- Tipos complexos (objetos, ponteiros) funcionam com GENERATOR mas podem requerer instanciação de template apropriada
- Ao usar GENERATOR em argumentos de construtor, armazene primeiro o valor gerado em uma variável
- O gerenciamento de memória é responsabilidade do usuário ao gerar ponteiros com `new`
- Armazenamento thread-local é usado, portanto o comportamento em ambientes de teste multi-threaded pode precisar de consideração

## Acesso a Membros Privados para Testes

A biblioteca fornece uma maneira simples e limpa de acessar membros privados em testes usando declarações friend explícitas.

### Exemplo Rápido

```cpp
// Na sua classe de produção
class MyClass {
private:
    int privateValue;
    std::string privateName;

    int computeSecret(int x) const { return privateValue * x; }

public:
    MyClass(int v, const std::string& n) : privateValue(v), privateName(n) {}

    // Conceder acesso friend a testes específicos
    GTESTG_FRIEND_TEST(MyClassTest, AccessPrivateMembers);
    GTESTG_FRIEND_TEST(MyClassTest, ModifyPrivateMembers);
};

// No seu arquivo de teste
struct MyClassTest : ::testing::Test {
    MyClass obj{42, "secret"};
};

TEST_F_FRIEND(MyClassTest, AccessPrivateMembers) {
    // Acesso direto a membros privados!
    EXPECT_EQ(obj.privateValue, 42);
    EXPECT_EQ(obj.privateName, "secret");
}

TEST_F_FRIEND(MyClassTest, ModifyPrivateMembers) {
    // Pode modificar membros privados
    obj.privateValue = 100;
    EXPECT_EQ(obj.privateValue, 100);

    // Pode chamar métodos privados
    int result = obj.computeSecret(2);
    EXPECT_EQ(result, 200);
}
```

### Recursos Principais

- **Simples e Limpo**: Usa declarações friend padrão do C++
- **Acesso Seletivo**: Conceda acesso apenas aos testes específicos que precisam
- **Sem Overhead**: Mecanismo puro em tempo de compilação, sem custo em tempo de execução
- **Seguro de Tipo**: Segurança de tipo garantida pelo compilador
- **Seguro para Produção**: Declarações friend não têm impacto em tempo de execução

### Referência da API

#### Macros para Classes Alvo

| Macro | Propósito | Uso |
|-------|---------|-------|
| `GTESTG_FRIEND_TEST(Suite, TestName)` | Conceder acesso friend a um teste TEST_F específico | Colocar na definição da classe |
| `GTESTG_FRIEND_TEST_G(TestClassName, TestName)` | Conceder acesso friend a um teste TEST_G | Colocar na definição da classe |
| `GTESTG_FRIEND_TEST_SUITE(Suite)` | Conceder acesso friend a todos os testes em uma suite | Colocar na definição da classe |

#### Macros para Arquivos de Teste

| Macro | Propósito | Uso |
|-------|---------|-------|
| `TEST_F_FRIEND(Suite, TestName)` | Definir um teste com acesso friend | Igual a TEST_F |
| `TEST_G_FRIEND(TestClassName, TestName)` | Definir um teste gerador com acesso friend | Igual a TEST_G |

### Exemplos de Uso

#### Acesso Privado Básico
```cpp
// Classe alvo
class Widget {
private:
    int secret_ = 42;
public:
    // Conceder acesso a teste específico
    GTESTG_FRIEND_TEST(WidgetTest, CheckSecret);
};

// Arquivo de teste
struct WidgetTest : ::testing::Test {
    Widget w;
};

TEST_F_FRIEND(WidgetTest, CheckSecret) {
    EXPECT_EQ(w.secret_, 42);  // Acesso direto ao membro privado
}
```

#### Testes Geradores com Acesso Privado
```cpp
// Classe alvo
class Calculator {
private:
    int factor_ = 10;
    int multiply(int x) const { return x * factor_; }
public:
    // Conceder acesso ao teste gerador
    GTESTG_FRIEND_TEST_G(CalcTest, TestMultiply);
};

// Arquivo de teste
struct CalcTest : ::gtest_generator::TestWithGenerator {
    Calculator calc;
};

TEST_G_FRIEND(CalcTest, TestMultiply) {
    int input = GENERATOR(1, 2, 3);
    USE_GENERATOR();

    // Acesso ao método e membro privados
    int result = calc.multiply(input);
    EXPECT_EQ(result, input * calc.factor_);
}
```

#### Conceder Acesso a Todos os Testes em uma Suite
```cpp
// Classe alvo - concede acesso a TODOS os testes em MyTestSuite
class MyClass {
private:
    int value_ = 100;
public:
    // Conceder acesso à suite de testes completa
    GTESTG_FRIEND_TEST_SUITE(MyTestSuite);
};

// Arquivo de teste - todos os testes nesta suite têm acesso
struct MyTestSuite : ::testing::Test {
    MyClass obj;
};

TEST_F(MyTestSuite, Test1) {
    EXPECT_EQ(obj.value_, 100);  // Tem acesso
}

TEST_F(MyTestSuite, Test2) {
    obj.value_ = 200;  // Também tem acesso
    EXPECT_EQ(obj.value_, 200);
}
```

#### Trabalhando com Herança
```cpp
// Classe base
class Base {
private:
    int base_secret_ = 10;
public:
    GTESTG_FRIEND_TEST(DerivedTest, AccessBoth);
};

// Classe derivada
class Derived : public Base {
private:
    int derived_secret_ = 20;
public:
    GTESTG_FRIEND_TEST(DerivedTest, AccessBoth);
};

// Teste
struct DerivedTest : ::testing::Test {
    Derived d;
};

TEST_F_FRIEND(DerivedTest, AccessBoth) {
    EXPECT_EQ(d.base_secret_, 10);     // Acesso ao privado da base
    EXPECT_EQ(d.derived_secret_, 20);  // Acesso ao privado da derivada
}
```

### Notas Importantes

1. **Concessão Explícita Necessária**: Cada teste que precisa de acesso privado deve ser listado explicitamente na classe alvo
2. **Sem Mágica**: Usa declarações friend padrão do C++ - simples e previsível
3. **TEST_F_FRIEND é Opcional**: `TEST_F_FRIEND` é apenas uma macro de conveniência que mapeia para `TEST_F`. Você pode usar `TEST_F` normal se a classe tiver a declaração `GTESTG_FRIEND_TEST` apropriada
4. **Segurança em Tempo de Compilação**: Se um teste tentar acessar membros privados sem ter acesso friend concedido, você receberá um erro de compilação
5. **Manutenção**: Ao adicionar um novo teste que precisa de acesso privado, lembre-se de adicionar a declaração `GTESTG_FRIEND_TEST` correspondente à classe alvo

### Quando Usar Este Recurso

Use acesso a membros privados quando:
- Testando estado interno que não está exposto através da interface pública
- Verificando lógica privada complexa
- Configurando estados internos específicos para testes
- Testando código legado que não pode ser facilmente refatorado

Evite usar quando:
- A necessidade de acesso privado indica design ruim
- Testes de interface pública seriam suficientes
- Criaria acoplamento estreito entre testes e implementação

Consulte `test_friend_access.cpp` para exemplos completos.

### Macros TEST_F_FRIEND e TEST_G_FRIEND

A biblioteca fornece macros `TEST_F_FRIEND` e `TEST_G_FRIEND` que criam infraestrutura de teste com suporte integrado para o padrão VirtualAccessor. Essas macros funcionam perfeitamente com a declaração `GTESTG_FRIEND_ACCESS_PRIVATE()`.

**Pontos Principais:**
- `GTESTG_FRIEND_ACCESS_PRIVATE()` concede acesso friend para **ambas** as abordagens baseadas em classe (VirtualAccessor) e baseadas em função (gtestg_private_accessMember)
- Use `TEST_F_FRIEND` para testes regulares estilo TEST_F
- Use `TEST_G_FRIEND` para testes parametrizados baseados em geradores
- Continue usando macros `GTESTG_PRIVATE_MEMBER` para acessar membros privados

**Exemplo com TEST_F_FRIEND:**
```cpp
class Widget {
private:
    int secret_ = 42;
public:
    Widget() = default;

    // Uma única macro concede ambos os tipos de acesso friend
    GTESTG_FRIEND_ACCESS_PRIVATE();
};

// Declarar o accessor
GTESTG_PRIVATE_DECLARE_MEMBER(Widget, secret_);

struct WidgetTest : ::testing::Test {
    Widget w;
};

TEST_F_FRIEND(WidgetTest, AccessPrivate) {
    // Acessar membro privado usando accessor baseado em função
    int& secret = GTESTG_PRIVATE_MEMBER(Widget, secret_, &w);
    EXPECT_EQ(secret, 42);
    secret = 100;
    EXPECT_EQ(secret, 100);
}
```

**Exemplo com TEST_G_FRIEND:**
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

**Suporte Multi-Arquivo:**
`TEST_F_FRIEND` e `TEST_G_FRIEND` funcionam corretamente quando os testes são definidos em múltiplos arquivos .cpp vinculados ao mesmo executável, assim como `TEST_G` regular. Veja `test_friend_multi_file1.cpp` e `test_friend_multi_file2.cpp` para exemplos.

### Sistema Unificado de Acesso a Membros Privados

A biblioteca fornece um sistema unificado para acessar membros privados e protegidos em seus testes. Adicionando uma única macro `GTESTG_FRIEND_ACCESS_PRIVATE()` à sua classe, você habilita **duas abordagens complementares** para acesso a membros privados:

1. **Acesso Direto via TEST_F_FRIEND/TEST_G_FRIEND** - Recomendado para a maioria dos casos
2. **Acesso Baseado em Função via macros GTESTG_PRIVATE_MEMBER** - Para controle mais explícito

Ambas as abordagens funcionam perfeitamente juntas e podem ser usadas no mesmo teste.

#### O Núcleo: GTESTG_FRIEND_ACCESS_PRIVATE()

Adicione esta única macro à sua classe para habilitar acesso a membros privados:

```cpp
class MyClass {
private:
    int privateValue = 42;
    std::string privateName = "secret";
public:
    // Uma macro habilita ambas as abordagens de acesso
    GTESTG_FRIEND_ACCESS_PRIVATE();
};
```

Esta macro concede acesso friend a:
- **Template VirtualAccessor** - Usado por TEST_F_FRIEND e TEST_G_FRIEND
- **Função gtestg_private_accessMember** - Usada por macros GTESTG_PRIVATE_MEMBER

#### Abordagem 1: Usando TEST_F_FRIEND e TEST_G_FRIEND (Recomendado)

Para casos simples, use `TEST_F_FRIEND` ou `TEST_G_FRIEND` para criar testes que podem acessar diretamente membros privados:

**Exemplo com TEST_F_FRIEND:**
```cpp
struct WidgetTest : ::testing::Test {
    Widget w;
};

TEST_F_FRIEND(WidgetTest, AccessPrivate) {
    // Acesso direto a membros privados (via especialização VirtualAccessor)
    EXPECT_EQ(w.secret_, 42);
    w.secret_ = 100;
    EXPECT_EQ(w.secret_, 100);
}
```

**Exemplo com TEST_G_FRIEND:**
```cpp
struct WidgetGenTest : ::gtest_generator::TestWithGenerator {
    Widget w{999};
};

TEST_G_FRIEND(WidgetGenTest, GeneratorTest) {
    int factor = GENERATOR(1, 2, 5);
    USE_GENERATOR();

    // Acesso direto também funciona em testes parametrizados
    EXPECT_EQ(w.secret_, 999);
    printf("factor=%d, secret=%d\n", factor, w.secret_);
}
```

**Suporte Multi-Arquivo:**
`TEST_F_FRIEND` e `TEST_G_FRIEND` funcionam corretamente quando os testes são definidos em múltiplos arquivos .cpp vinculados ao mesmo executável. Veja `test_friend_multi_file1.cpp` e `test_friend_multi_file2.cpp` para exemplos.

**Como Funciona:**
- `TEST_F_FRIEND` e `TEST_G_FRIEND` criam uma especialização de template explícita de `VirtualAccessor<Suite, TestName>` dentro do namespace `gtestg_detail`
- Esta especialização recebe acesso friend via `GTESTG_FRIEND_ACCESS_PRIVATE()`
- Como `VirtualAccessor` é friend e deriva da sua fixture de teste, ele pode acessar membros privados da classe alvo
- O corpo do teste executa dentro do contexto desta classe friend, habilitando acesso direto a membros privados
- Cada teste recebe um tipo de tag único para criar uma especialização separada, evitando conflitos de nomenclatura

#### Abordagem 2: Usando Macros GTESTG_PRIVATE_MEMBER (Controle Explícito)

Para mais controle ou ao trabalhar com macros `TEST_F`/`TEST_G` regulares, use as macros de accessor baseadas em função. Esta abordagem requer declarar acesso para cada membro que você deseja acessar.

**Passo 1: Declare acesso fora da sua classe (no arquivo de teste):**
```cpp
// Declare quais membros você deseja acessar
GTESTG_PRIVATE_DECLARE_MEMBER(Widget, secret_);
GTESTG_PRIVATE_DECLARE_MEMBER(Widget, privateName);
```

**Passo 2: Acesse membros nos seus testes:**
```cpp
TEST_F_FRIEND(WidgetTest, AccessPrivate) {
    // Acesse usando a macro
    int& secret = GTESTG_PRIVATE_MEMBER(Widget, secret_, &w);
    EXPECT_EQ(secret, 42);
    secret = 100;
    EXPECT_EQ(secret, 100);
}
```

Esta abordagem é útil quando:
- Você deseja documentação explícita de quais membros estão sendo acessados
- Você precisa acessar membros estáticos
- Você deseja funções accessor personalizadas com lógica adicional

#### Combinando Ambas as Abordagens

Você pode usar ambas as abordagens no mesmo teste:

```cpp
class Widget {
private:
    int secret_ = 42;
    static int counter_;
public:
    GTESTG_FRIEND_ACCESS_PRIVATE();  // Habilita ambas as abordagens
};

int Widget::counter_ = 0;

// Declare acesso para membro estático
GTESTG_PRIVATE_DECLARE_STATIC(Widget, counter_);

TEST_G_FRIEND(WidgetTest, CombinedAccess) {
    int value = GENERATOR(10, 20);
    USE_GENERATOR();

    Widget w;

    // Abordagem 1: Acesso direto a membro de instância
    w.secret_ = value;
    EXPECT_EQ(w.secret_, value);

    // Abordagem 2: Use macro para membro estático
    int& count = GTESTG_PRIVATE_STATIC(Widget, counter_);
    count++;
}
```

### Referência Completa da API para Macros GTESTG_PRIVATE_MEMBER

Esta seção fornece referência detalhada para as macros de acesso a membros privados baseadas em função (Abordagem 2).

#### Declarando Acesso a Membros

Coloque estas declarações **fora** da sua classe, tipicamente no seu arquivo de teste. Estas declarações informam ao sistema quais membros privados você deseja acessar:

| Macro | Propósito | Parâmetros | Exemplo |
|-------|-----------|------------|---------|
| `GTESTG_PRIVATE_DECLARE_MEMBER` | Declarar acesso a membros de instância | Target, MemberName | `GTESTG_PRIVATE_DECLARE_MEMBER(MyClass, privateField)` |
| `GTESTG_PRIVATE_DECLARE_STATIC` | Declarar acesso a membros estáticos | Target, MemberName | `GTESTG_PRIVATE_DECLARE_STATIC(MyClass, staticCounter)` |
| `GTESTG_PRIVATE_DECLARE_FUNCTION` | Declarar função accessor personalizada | ThisClass, Target, FuncName | `GTESTG_PRIVATE_DECLARE_FUNCTION(MyTest, MyClass, GetSum)` |

#### Macros para Acessar Membros

Use estas macros **dentro** das suas funções de teste para acessar membros privados:

| Macro | Propósito | Parâmetros | Exemplo |
|-------|-----------|------------|---------|
| `GTESTG_PRIVATE_MEMBER` | Acessar membro de instância | Target, MemberName, &obj | `GTESTG_PRIVATE_MEMBER(MyClass, privateField, &obj)` |
| `GTESTG_PRIVATE_STATIC` | Acessar membro estático | Target, MemberName | `GTESTG_PRIVATE_STATIC(MyClass, staticCounter)` |
| `GTESTG_PRIVATE_CALL` | Chamar função personalizada com objeto de teste explícito | Target, FuncName, test_obj, &obj | `GTESTG_PRIVATE_CALL(MyClass, GetSum, this, &obj)` |
| `GTESTG_PRIVATE_CALL_ON_TEST` | Chamar função personalizada (usa 'this' implícito) | ThisClass, Target, FuncName, &obj | `GTESTG_PRIVATE_CALL_ON_TEST(MyTest, MyClass, GetSum, &obj)` |

#### Exemplos de Uso Detalhados

Os exemplos a seguir demonstram padrões de uso abrangentes para as macros GTESTG_PRIVATE_*.

**Exemplo 1: Acessando Membros de Instância**
```cpp
class MyClass {
private:
    int privateValue = 42;
    std::string privateName = "secret";
public:
    GTESTG_FRIEND_ACCESS_PRIVATE();
};

// Declare acesso (no arquivo de teste)
GTESTG_PRIVATE_DECLARE_MEMBER(MyClass, privateValue);
GTESTG_PRIVATE_DECLARE_MEMBER(MyClass, privateName);

TEST_G(MyTest, AccessPrivate) {
    int value = GENERATOR(10, 20, 30);
    USE_GENERATOR();

    MyClass obj;

    // Acesse e modifique membros privados
    int& val = GTESTG_PRIVATE_MEMBER(MyClass, privateValue, &obj);
    EXPECT_EQ(val, 42);
    val = value;  // Pode modificar
    EXPECT_EQ(val, value);

    std::string& name = GTESTG_PRIVATE_MEMBER(MyClass, privateName, &obj);
    EXPECT_EQ(name, "secret");
    name = "modified";
}
```

**Exemplo 2: Acessando Membros Estáticos**
```cpp
class MyClass {
private:
    static int staticCounter;
public:
    GTESTG_FRIEND_ACCESS_PRIVATE();
};

int MyClass::staticCounter = 100;

// Declare acesso ao membro estático
GTESTG_PRIVATE_DECLARE_STATIC(MyClass, staticCounter);

TEST_G(MyTest, AccessStatic) {
    USE_GENERATOR();

    // Acesse membro estático (nenhum objeto necessário)
    int& count = GTESTG_PRIVATE_STATIC(MyClass, staticCounter);
    EXPECT_EQ(count, 100);
    count = 200;  // Pode modificar
    EXPECT_EQ(count, 200);
}
```

**Exemplo 3: Funções Accessor Personalizadas**
```cpp
class MyClass {
private:
    int field1 = 10;
    int field2 = 20;
public:
    GTESTG_FRIEND_ACCESS_PRIVATE();
};

class MyTest : public ::gtest_generator::TestWithGenerator {};

// Declare função personalizada com acesso tanto ao contexto de teste quanto a membros privados
// THIS = objeto de teste, TARGET = objeto sendo acessado
GTESTG_PRIVATE_DECLARE_FUNCTION(MyTest, MyClass, GetSum) {
    // Pode acessar contexto de teste: THIS->GetParam()
    // Pode acessar membros privados: TARGET->field1, TARGET->field2
    return TARGET->field1 + TARGET->field2;
}

TEST_G(MyTest, CustomFunction) {
    int multiplier = GENERATOR(1, 2, 3);
    USE_GENERATOR();

    MyClass obj;

    // Chame função personalizada usando CALL_ON_TEST (usa 'this' implícito)
    int sum = GTESTG_PRIVATE_CALL_ON_TEST(MyTest, MyClass, GetSum, &obj);
    EXPECT_EQ(sum, 30);  // 10 + 20

    // Alternativa: Passe objeto de teste explicitamente
    int sum2 = GTESTG_PRIVATE_CALL(MyClass, GetSum, static_cast<MyTest*>(this), &obj);
    EXPECT_EQ(sum2, 30);
}
```

Para exemplos completos, veja `test_private_access.cpp` e `test_define_macros.cpp`.

## Macros de Comparação de Arrays

A biblioteca fornece macros convenientes para comparar arrays elemento por elemento com mensagens de erro detalhadas. Essas macros são construídas sobre as macros de asserção do Google Test.

### Exemplo Rápido

```cpp
TEST_G(ArrayTest, CompareArrays) {
    USE_GENERATOR();

    int expected[] = {1, 2, 3, 4, 5};
    int actual[] = {1, 2, 3, 4, 5};

    EXPECT_ARRAY_EQ(expected, actual, 5);  // Asserção não-fatal
}
```

### Macros Disponíveis

#### Tipos Inteiros e Genéricos

- **`EXPECT_ARRAY_EQ(expected, actual, size)`** - Não-fatal: Compara dois arrays elemento por elemento
- **`ASSERT_ARRAY_EQ(expected, actual, size)`** - Fatal: Compara dois arrays elemento por elemento

#### Tipos de Ponto Flutuante

- **`EXPECT_ARRAY_NEAR(expected, actual, size, abs_error)`** - Não-fatal: Compara arrays de ponto flutuante com tolerância
- **`ASSERT_ARRAY_NEAR(expected, actual, size, abs_error)`** - Fatal: Compara arrays de ponto flutuante com tolerância
- **`EXPECT_ARRAY_DOUBLE_EQ(expected, actual, size)`** - Não-fatal: Compara arrays de double com tolerância padrão
- **`EXPECT_ARRAY_FLOAT_EQ(expected, actual, size)`** - Não-fatal: Compara arrays de float com tolerância padrão

### Recursos Principais

- **Comparação elemento por elemento**: Cada elemento é comparado individualmente
- **Mensagens de erro detalhadas**: Mostra qual índice difere e os valores
- **Funciona com qualquer tipo comparável**: int, float, double, string, tipos personalizados com operator==
- **Mensagens de sucesso**: Mostra "Arrays are equal" quando todos os elementos correspondem
- **Compatível com vetores e arrays**: Funciona com arrays estilo C, std::vector, std::array

### Notas Importantes

1. **Parâmetro de tamanho é obrigatório**: Você deve fornecer explicitamente o tamanho do array
2. **Fatal vs Não-fatal**: Use ASSERT_* para asserções fatais, EXPECT_* para não-fatais
3. **Comparações de ponto flutuante**: Use NEAR, FLOAT_EQ ou DOUBLE_EQ para valores de ponto flutuante
4. **Tipos personalizados**: Seu tipo deve ter operator== definido para EXPECT_ARRAY_EQ
5. **Arrays de tamanho zero**: Funciona corretamente com arrays vazios (tamanho = 0)

Consulte `test_array_compare.cpp` para exemplos completos.

## Melhorias Futuras

- Cálculo dinâmico do número total de combinações
- Suporte para diferentes tipos de dados em geradores
- Instanciações de teste nomeadas
- Suporte para padrões de valores mais complexos

## Licença

Este projeto é fornecido como está para fins educacionais e de desenvolvimento.
