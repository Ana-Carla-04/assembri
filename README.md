# 🧠 MIPS Assembler em C++

## 📌 Descrição

Este projeto consiste na implementação de um **assembler para a arquitetura MIPS**, desenvolvido em C++. O programa lê um arquivo contendo código Assembly MIPS e realiza sua tradução para **código de máquina**, podendo gerar saída em **formato binário (.bin)** ou **hexadecimal (.hex)**.

Além disso, o sistema também calcula estatísticas de uso das instruções e estima o **CPI (Cycles Per Instruction)** com base em um arquivo auxiliar.

---

## ⚙️ Funcionalidades

* 🔍 **Primeira passagem (First Pass)**

  * Identifica e armazena *labels* (rótulos)
  * Calcula endereços de memória das instruções
  * Constrói uma tabela de símbolos

* 🔄 **Segunda passagem (Second Pass)**

  * Traduz instruções Assembly para código de máquina
  * Suporte a:

    * Instruções do tipo **R**
    * Instruções do tipo **I**
    * Instruções do tipo **J**
  * Resolve desvios (`beq`, `bne`) e saltos (`j`, `jal`) com base nos labels

* 📁 **Geração de arquivos**

  * Saída em:

    * Binário (`.bin`)
    * Hexadecimal (`.hex`), compatível com simuladores (ex: Logisim)

* 📊 **Análise de desempenho**

  * Conta quantas vezes cada instrução é usada
  * Calcula o total de ciclos com base em um arquivo `infos.csv`
  * Estima o CPI do programa

---

## 🧩 Estrutura do Código

### Principais componentes:

* **Structs**

  * `LabelInfo`: armazena informações dos labels
  * `InstrucaoR`, `InstrucaoI`, `InstrucaoJ`: tabelas de instruções
  * `Registrador`: mapeamento de registradores MIPS

* **Funções principais**

  * `limparLinha()`: remove comentários e espaços
  * `extrairLabel()`: identifica labels nas linhas
  * `concatenarR/I/J()`: monta instruções binárias
  * `salvar_Intrucao_*()`: escreve no arquivo de saída
  * `lerRegistrador()`: converte nome do registrador para número
  * `ehNumero()`: valida se string é número

---

## ▶️ Como usar

### 1. Compilar

```bash
g++ assembler.cpp -o assembler
```

### 2. Executar

```bash
./assembler programa.asm -b   # saída binária
./assembler programa.asm -h   # saída hexadecimal
```

---

## 📥 Entrada esperada

Arquivo `.asm` contendo instruções MIPS, por exemplo:

```asm
main:
    add $t0, $t1, $t2
    beq $t0, $zero, fim
    j main
fim:
```

---

## 📤 Saída gerada

* `programa.bin` → código binário
* `programa.hex` → código hexadecimal (com header `v2.0 raw`)

---

## 📊 Arquivo auxiliar (infos.csv)

Arquivo usado para cálculo de ciclos:

```csv
add,1
sub,1
lw,5
sw,4
beq,3
j,2
```

---

## ⚠️ Limitações

* Não suporta pseudo-instruções (ex: `move`, `li`)
* Suporte limitado a registradores definidos manualmente

---

## 🚀 Possíveis melhorias

* Suporte a pseudo-instruções
* Melhor tratamento de erros
* Interface gráfica ou CLI mais amigável
* Suporte a mais registradores e instruções
* Testes automatizados
* Otimização do parsing (menos repetição de código)

---

## 👨‍💻 Autores
Ana Carla de Freitas Oliveira
João Lourenço de Azevedo Neto

Projeto desenvolvido como prática de:

* Arquitetura de Computadores
* Linguagem Assembly (MIPS)
* Manipulação de arquivos em C++
* Estruturas de dados e parsing

---

