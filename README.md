# 🖥️ Custom Compiler/Interpreter: ConfigScript (AED Compiladores)

Este repositório contém a entrega final da **Atividade Prática (AED)** da disciplina de **Compiladores**. O projeto consiste no desenvolvimento incremental de um mini-compilador/validador para uma linguagem de configuração e declaração de propriedades chamada **ConfigScript**.

O desenvolvimento foi dividido em 3 passos estruturais fundamentais, abrangendo desde o pré-processamento até a análise sintática completa com tratamento de erros.

---

## 📖 1. Proposta da Linguagem e Comandos (Passo 1)

### 💡 Ideia e Finalidade
A linguagem **ConfigScript** é uma linguagem de domínio específico (DSL) projetada para automação de tarefas, declaração de metadados e configuração de ambientes de execução. Sua principal finalidade é ler sequências de comandos estruturados do tipo `<Comando> <Valor>` para configurar variáveis globais, executar exclusão de arquivos indesejados, validar codificações Unicode, gerenciar variáveis de sistema e finalizar a execução de forma controlada.

### 🛠️ Comandos Principais
A linguagem opera de forma imperativa-declarativa, onde cada linha estruturada executa um comando associado a um argumento (valor):

| Comando | Tipo de Valor Esperado | Finalidade | Exemplo de Uso |
| :--- | :--- | :--- | :--- |
| `NumeroReal` | Valor numérico real (float/double) | Declara e valida constantes numéricas reais. | `NumeroReal 3.14159` |
| `Caractere` | Caractere único (char) | Define flags ou identificadores de caractere. | `Caractere Z` |
| `Apagar` | Caminho de arquivo/diretório | Comanda o sistema a remover um arquivo/recurso. | `Apagar relatorio.pdf` |
| `NomeComEspaço` | String de identificação de sistema | Associa chaves ou nomes a recursos internos. | `NomeComEspaço arquivo_de_sistema` |
| `Especial` | Sequência de caracteres especiais | Valida e armazena símbolos e caracteres de escape. | `Especial @#$%-!` |
| `Unicode` | String em codificação UTF-8 | Permite internacionalização de texto no parser. | `Unicode 你好` |
| `FimDoArquivo` | Nenhum (comando terminal) | Indica o fim lógico da execução das instruções. | `FimDoArquivo` |

---

## 🔍 2. Pré-processamento e Limpeza (Passo 1)

Antes de iniciar a geração de tokens (análise léxica), o código realiza um **pré-processamento** essencial no código-fonte carregado do arquivo de entrada. 

### Etapas do Pré-processador:
1. **Leitura Linha a Linha:** O arquivo é lido linha por linha para preservar o mapeamento preciso de linhas físicas para relatórios de erro (`lineNumber`).
2. **Remoção de Comentários de Linha Única (`//`):** Todo o conteúdo após os caracteres `//` em uma linha é ignorado.
3. **Remoção de Comentários de Múltiplas Linhas (`/* ... */`):** Utiliza uma flag de estado (`inMultiLineComment`) para ignorar blocos inteiros de comentários que cruzam várias linhas.
4. **Ignorar Espaços em Branco Extras:** Espaços em branco redundantes, tabulações e quebras de linha são ignorados por meio de um fluxo de string (`std::stringstream`), garantindo que apenas termos relevantes sejam repassados para a próxima fase.

---

## ⚡ 3. Analisador Léxico (Passo 2)

O **Analisador Léxico (Lexer)** é responsável por quebrar a string pré-processada em unidades lógicas chamadas **Lexemas** e classificá-las em **Tokens**.

### Lista de Tokens Reconhecidos
Os tokens são mapeados através de um `enum TokenType` na implementação:

| Token Enum | Expressão Regular / Valor Léxico | Descrição |
| :--- | :--- | :--- |
| `T_NUMERO_REAL` | `NumeroReal` | Palavra reservada para declaração real. |
| `T_CARACTERE` | `Caractere` | Palavra reservada para caractere. |
| `T_APAGAR` | `Apagar` | Palavra reservada para ação de apagar. |
| `T_NOME_ESPACO` | `NomeComEspaço` | Palavra reservada para nome. |
| `T_ESPECIAL` | `Especial` | Palavra reservada para delimitadores especiais. |
| `T_UNICODE` | `Unicode` | Palavra reservada para strings unicode. |
| `T_FIM_ARQUIVO` | `FimDoArquivo` | Palavra reservada de terminação lógica. |
| `T_VALOR` | Qualquer palavra não reservada | Representa o parâmetro associado ao comando anterior. |
| `T_ERRO` | Caracteres malformados | Sinaliza desconformidade com o alfabeto da linguagem. |

### Tratamento de Erros Léxicos
* **Design Robusto:** Em vez de falhar imediatamente no Lexer ao encontrar lexemas desconhecidos, a linguagem classifica de maneira inteligente strings avulsas como `T_VALOR`. Isso permite que erros de contextualização sejam tratados de forma elegante no parser sintático, fornecendo um feedback de contexto muito mais rico ao desenvolvedor.

---

## 📐 4. Gramática e Analisador Sintático (Passo 3)

### Gramática Livre de Contexto (GLC)
A sintaxe do **ConfigScript** é regida pela seguinte gramática formal, representada na Notação de Backus-Naur (BNF):

```bnf
<programa>           ::= <lista_comandos> <fim>
<lista_comandos>     ::= <comando_completo> <lista_comandos> | ε
<comando_completo>   ::= <comando> <valor>
<comando>            ::= "NumeroReal" 
                       | "Caractere" 
                       | "Apagar" 
                       | "NomeComEspaço" 
                       | "Especial" 
                       | "Unicode"
<valor>              ::= T_VALOR
<fim>                ::= "FimDoArquivo"
```

### Funcionamento do Analisador Sintático (Parser)
O parser lê a sequência de tokens gerada pelo analisador léxico de forma linear (análise sintática preditiva descendente simples de um passo):
1. Avalia se o token atual é uma palavra reservada de comando (`T_NUMERO_REAL` até `T_UNICODE`).
2. Verifica obrigatoriamente se o próximo token (`i + 1`) existe e é do tipo `T_VALOR`.
3. Se as duas condições forem atendidas, o comando é processado com sucesso como uma instrução válida da linguagem, e o parser avança dois tokens.
4. Se um comando for declarado sem um argumento correspondente, ou se houver um argumento solto sem comando prévio, o analisador sintático interrompe imediatamente a validação e aponta o erro estrutural exato e a linha onde ocorreu.
5. A análise encerra com sucesso ao encontrar o token `T_FIM_ARQUIVO` (`FimDoArquivo`).

---

## 🚀 Como Executar o Projeto

O compilador foi desenvolvido em **C++** padrão (C++11 ou superior) e pode ser facilmente compilado e executado em qualquer sistema operacional corporativo moderno (Windows, Linux, macOS).

### 🛠️ Passo 1: Compilação

Abra o seu terminal no diretório do projeto e execute o comando correspondente ao seu sistema/compilador:

#### No Windows (usando MinGW / GCC):
```bash
g++ -std=c++17 compilador.cpp -o compilador
```

#### No Linux / macOS (usando GCC / Clang):
```bash
g++ compilador.cpp -o compilador
```

---

### 📂 2. Arquivos de Teste de Entrada

O projeto inclui dois arquivos de testes principais para comprovar o comportamento do analisador:

#### 1. Entrada Válida (`entrada.cmp`)
Contém um script de automação de servidor coeso, com comentários de linha única, blocos de comentário multilinha, espaçamentos flexíveis e todos os comandos reconhecidos pela linguagem.

```
// =================================================================
// CONFIGSCRIPT - SCRIPT DE CONFIGURAÇÃO E MANUTENÇÃO DO SERVIDOR (VÁLIDO)
// =================================================================

// 1. Definição da versão da API de processamento do sistema
NumeroReal    2.5

// 2. Modo de Depuração (Flag de caractere único: 'Y' para Sim, 'N' para Não)
Caractere     Y

/* 3. Bloco de Manutenção Preventiva:
   Abaixo, limpamos os arquivos temporários do servidor anterior e
   identificamos o nome da máquina no cluster interno do sistema. */

Apagar        logs_temporarios.log
NomeComEspaço servidor_producao_leste

// 4. Parâmetros especiais de criptografia e identificador internacional (Unicode)
Especial      #KeyGroup_Prod_99!
Unicode       🌐_Servidor_Ativo_🌐

// 5. Finalização estrutural do script
FimDoArquivo
```

#### 2. Entrada Inválida (`invalidos.cmp`)
Demonstra como o analisador se comporta ao identificar um erro de sintaxe, onde o comando `Apagar` é invocado sem qualquer argumento logo antes de outro comando de declaração.

```
// =================================================================
// CONFIGSCRIPT - SCRIPT DE CONFIGURAÇÃO COM ERRO SINTÁTICO (INVÁLIDO)
// =================================================================

// 1. Definição da versão do sistema
NumeroReal    2.5

// 2. Modo de depuração ativo
Caractere     Y

// --- ERRO SINTÁTICO ---
// O comando 'Apagar' exige o nome de um arquivo para ser executado.
// Ao deixá-lo vazio ou pular direto para o próximo comando, o analisador sintático
// identificará que 'NomeComEspaço' não é um argumento válido de 'Apagar', mas sim
// outro comando reservado, abortando a compilação imediatamente.
Apagar        

NomeComEspaço servidor_desenvolvimento
FimDoArquivo
```

---

### 🏃 3. Executando os Testes

Para rodar os testes informados, passe o arquivo de entrada desejado como argumento para o compilador compilado:

#### Testando a Entrada Válida:
```bash
./compilador entrada.cmp
```
**Saída Esperada no Terminal:**
```text
Comando valido: 'NumeroReal' com valor '2.5' (Linha 6)
Comando valido: 'Caractere' com valor 'Y' (Linha 9)
Comando valido: 'Apagar' com valor 'logs_temporarios.log' (Linha 15)
Comando valido: 'NomeComEspaço' com valor 'servidor_producao_leste' (Linha 16)
Comando valido: 'Especial' com valor '#KeyGroup_Prod_99!' (Linha 19)
Comando valido: 'Unicode' com valor '🌐_Servidor_Ativo_🌐' (Linha 20)
Fim do arquivo encontrado na linha 23.
```

#### Testando a Entrada Inválida:
```bash
./compilador invalidos.cmp
```
**Saída Esperada no Terminal:**
```text
Comando valido: 'NumeroReal' com valor '2.5' (Linha 6)
Comando valido: 'Caractere' com valor 'Y' (Linha 9)
[ERRO SINTATICO] Esperado um valor apos o comando 'Apagar' na linha 18
```

---

## 🗃️ Estrutura Final do Repositório

```text
├── compilador.cpp      # Código-fonte principal em C++ (Lexer, Parser e Main)
├── entrada.cmp         # Exemplo de código-fonte válido (sucesso completo)
├── invalidos.cmp       # Exemplo de código-fonte contendo erros sintáticos
└── README.md           # Documentação completa do projeto (Este arquivo)
```