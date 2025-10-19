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
    // IMPORTANTE: Todas as chamadas GENERATOR() devem vir ANTES de USE_GENERATOR()
    int a = GENERATOR(1, 2);      // Será 1 ou 2
    int b = GENERATOR(10, 20);    // Será 10 ou 20
    USE_GENERATOR();              // Deve ser chamado DEPOIS de todas as chamadas GENERATOR()

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

## Exemplos Avançados

### Trabalhando com Classes e Objetos

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

### Trabalhando com Contêineres STL

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
    printf("Vetor: tamanho=%d, multiplicador=%d\n", size, multiplier);
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

### A Ordem Correta é Crítica

✅ **CORRETO**:
```cpp
TEST_G(MyTest, Example) {
    int a = GENERATOR(1, 2);     // Primeiro: Definir geradores
    int b = GENERATOR(10, 20);   // 
    USE_GENERATOR();             // Depois: Chamar USE_GENERATOR()
    // Lógica do teste aqui
}
```

❌ **INCORRETO**:
```cpp
TEST_G(MyTest, Example) {
    USE_GENERATOR();             // Errado! Isso deve vir depois das chamadas GENERATOR
    int a = GENERATOR(1, 2);     
    int b = GENERATOR(10, 20);   
    // Lógica do teste aqui
}
```

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

TEST_FRIEND(MyClassTest, AccessPrivateMembers) {
    // Acesso direto a membros privados!
    EXPECT_EQ(obj.privateValue, 42);
    EXPECT_EQ(obj.privateName, "secret");
}

TEST_FRIEND(MyClassTest, ModifyPrivateMembers) {
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
| `TEST_FRIEND(Suite, TestName)` | Definir um teste com acesso friend | Igual a TEST_F |
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

TEST_FRIEND(WidgetTest, CheckSecret) {
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

TEST_FRIEND(DerivedTest, AccessBoth) {
    EXPECT_EQ(d.base_secret_, 10);     // Acesso ao privado da base
    EXPECT_EQ(d.derived_secret_, 20);  // Acesso ao privado da derivada
}
```

### Notas Importantes

1. **Concessão Explícita Necessária**: Cada teste que precisa de acesso privado deve ser listado explicitamente na classe alvo
2. **Sem Mágica**: Usa declarações friend padrão do C++ - simples e previsível
3. **TEST_FRIEND é Opcional**: `TEST_FRIEND` é apenas uma macro de conveniência que mapeia para `TEST_F`. Você pode usar `TEST_F` normal se a classe tiver a declaração `GTESTG_FRIEND_TEST` apropriada
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
