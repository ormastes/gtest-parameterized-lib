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

## Licença

Este projeto é fornecido como está para fins educacionais e de desenvolvimento.