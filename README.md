# bcc-compiladores-2026

Compilador didático da disciplina de **Compiladores** (BCC). O projeto evolui aula a aula: por enquanto temos a **análise léxica** e um **parser LL(1)** para expressões aritméticas (front-end incompleto — ainda sem AST, análise semântica ou geração de código).

O executável gerado chama-se `mybc` (*my basic calculator* / reconhecêdor de expressões).

---

## Como compilar e rodar

```bash
make          # gera ./mybc a partir de main.o, lexer.o e parser.o
make clean    # remove .o
```

A entrada é lida da **stdin**:

```bash
./mybc
# digite a expressão e encerre com Ctrl+D (Linux/macOS) ou Ctrl+Z+Enter (Windows)

echo "2+(3*4)" | ./mybc
```

Se a expressão for válida (segundo a gramática abaixo), o programa termina com código `0`. Caso contrário, emite diagnóstico em `stderr` e sai com código de erro.

---

## Arquitetura atual

| Arquivo     | Papel |
|-------------|--------|
| `main.c`    | Driver: abre `stdin`, obtém o 1º *lookahead* e chama `E()` |
| `lexer.c`   | Analisador léxico: espaços, comentários Pascal, IDs e literais |
| `parser.c`  | Parser descendente recursivo LL(1) de expressões |
| `tokens.h`  | Códigos de token (`ID`, `DEC`, `OCT`, `HEX`) |
| `Makefile`  | Build com `-I.` |

Fluxo:

```
fonte (stdin) → gettoken() → lookahead → match() / E → T → F
```

---

## O que já foi implementado

### 1. Análise léxica (`lexer.c`)

#### Espaços em branco
- `skipspaces()` descarta espaços, tabs e quebras de linha.
- Quebras de linha incrementam o contador global `line` (usado nos diagnósticos).

#### Comentários estilo Pascal — `skipcomments()`
Enunciado (2026-09-09):

- Comentário abre com `{` e fecha com `}`.
- `\{` e `\}` são caracteres literais **dentro** do comentário (não abrem/fecham).
- Vários comentários (e espaços) em sequência devem ser ignorados antes do próximo token.

**Bug corrigido (EOF / `}`):**  
A versão anterior usava `while ((head = getc()) != '}')` sem tratar `EOF`, o que **travava** em comentário não fechado (`getc` continua devolvendo `EOF` e `EOF != '}'`).  
Na tentativa de corrigir isso, um `getc()` extra após `\{` / `\}` podia **consumir o `}` de fechamento** e reportar `unclosed comment` em comentário válido (ex.: `{\}}`).

A versão atual (sem `goto`, no mesmo estilo `while (1)` / `break` do restante do lexer):

- laço externo: pula espaços e, se achar `{`, consome o comentário; senão devolve o caractere e sai;
- laço interno: lê até `}`; se ver `\`, consome **só** o próximo caractere (não um `getc` a mais);
- se `head == EOF` no meio do comentário → `unclosed comment at line N` e `exit(-4)` (sem hang);
- não faz `ungetc(EOF)`.

Exemplos:

| Entrada | Comportamento |
|---------|----------------|
| `{ ok } 1+2` | Comentário ignorado; tokens de `1+2` |
| `{ não fecha` + EOF | Erro: `unclosed comment` (não trava) |
| `{\}}` | Comentário válido (`\}` literal + `}` fecha) |
| `{ \{ \} }` | Comentário válido |

#### Tokens reconhecidos

| Token | Forma | Código |
|-------|--------|--------|
| Identificador | `[A-Za-z][A-Za-z0-9]*` | `ID` |
| Decimal | `0` ou `[1-9][0-9]*` | `DEC` |
| Octal | `0[0-7]+` | `OCT` |
| Hexadecimal | `0[xX][0-9A-Fa-f]+` | `HEX` |
| Operadores / pontuação | `+ - * / ( )` etc. | próprio ASCII |

`gettoken()` chama `skipcomments()`, depois tenta `isID` → `isOCT` → `isHEX` → `isDEC` (nessa ordem: `0` pode ser prefixo de octal/hex) e, por fim, devolve o caractere ASCII lido.

### 2. Análise sintática LL(1) (`parser.c`)

Gramática de expressões (com sinal unário opcional e eliminação de recursão à esquerda / cauda):

```
E → ['+' | '-'] T { ('+' | '-') T }
T → F { ('*' | '/') F }
F → ID | DEC | OCT | HEX | '(' E ')'
```

- *Lookahead* de 1 token (`lookahead`).
- `match(required)` consome se bater; senão diagnostica *token mismatch* ou `premature EOF` e sai com `exit(-2)`.
- Recursões de cauda de `R`/`Q` foram substituídas por laços `while` (menos uso de pilha).

Ainda **não** há avaliação numérica nem árvore sintática: o parser só **reconhece** se a entrada é uma expressão bem formada.

### 3. Driver (`main.c`)

- `source = stdin`
- `lookahead = gettoken(stdin)`
- chama `E()` e retorna `0` se tudo ok

---

## Códigos de saída (diagnósticos)

| Código | Situação |
|--------|----------|
| `0` | Expressão reconhecida com sucesso |
| `-2` | Erro sintático (`match` / EOF prematuro) |
| `-4` | Comentário `{ ...` sem `}` até o EOF |

---

## Testes manuais sugeridos (comentários / EOF)

```bash
# comentário ok + expressão
printf '{ comentario } 1+2\n' | ./mybc

# escapes dentro do comentário
printf '{\\} } 3*4\n' | ./mybc
printf '{\\}} 5\n' | ./mybc

# EOF sem fechar '}' — deve errar sem travar
printf '{ comentario sem fechar' | ./mybc
```

---

## Estrutura do repositório

```
bcc-compiladores-2026/
├── main.c / main.h
├── lexer.c / lexer.h
├── parser.c / parser.h
├── tokens.h
├── Makefile
├── test-pas.pas      # rascunho Pascal (não é entrada do mybc atual)
└── README.md
```
