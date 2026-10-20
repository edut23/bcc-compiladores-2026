# MyBC

Interpretador de expressões aritméticas da disciplina de **Compiladores**. Lê a entrada padrão, reconhece a expressão (análise léxica + parser LL(1)), avalia com uma máquina de pilha simples e imprime o resultado.

O executável chama-se `mybc`. Os nomes dos fontes originais (`main`, `lexer`, `parser`) foram mantidos; a tabela de símbolos e a máquina virtual foram extraídas para `symtab.[ch]` e `vmachine.[ch]`.

---

## Compilar e executar

```bash
make              # gera ./mybc
make clean        # remove .o
make mostlyclean  # remove .o e o executável
```

A entrada é a **stdin**.

```bash
./mybc
echo "2+3*4" | ./mybc
printf 'x:=2\nx+1\n' | ./mybc
```

| Fluxo | Destino |
|-------|---------|
| resultado da expressão | **stdout** |
| diagnóstico de erro | **stderr** |

Após um erro, o processo **não** termina: a sentença atual é descartada e a próxima (nova linha ou `;`) é avaliada normalmente. O programa só encerra no EOF, com status `0`.

---

## Arquitetura

```
fonte (stdin)
    → gettoken()          lexer.c
    → lookahead / match() parser.c
    → expr()              parser.c  (sintaxe + ações semânticas)
         ├─ push / pop / acc      vmachine.c
         └─ store / recall        symtab.c
    → stdout (valor) ou stderr (erro)
```

| Arquivo | Papel |
|---------|--------|
| `main.c` | Abre stdin/stdout, lê o 1º token e chama `mybc()` |
| `lexer.c` / `lexer.h` | Tokens, `lexeme[]`, comentários `{...}`, contador `line` |
| `parser.c` / `parser.h` | Parser LL(1), `match()`, recuperação de erro, `mybc()` |
| `symtab.c` / `symtab.h` | Nomes de variáveis e valores (`lookup`, `store`, `recall`) |
| `vmachine.c` / `vmachine.h` | Acumulador `acc` e pilha (`push`, `pop`) |
| `tokens.h` | Códigos `ID`, `DEC`, `OCT`, `HEX`, `ASGN` (`:=`) |

---

## Linguagem reconhecida

```
E → ['+' | '-'] T { ('+' | '-') T }
T → F { ('*' | '/') F }
F → ID | ID := E | DEC | OCT | HEX | '(' E ')'
```

- `*` e `/` têm precedência sobre `+` e `-`.
- Sinal unário opcional no início da expressão (`-4+5` → `1`).
- Atribuição `:=` é associativa à direita (`x:=y:=1`).
- Várias expressões: uma por linha, ou separadas por `;`.
- Comentários estilo Pascal: `{ ... }`. Dentro do comentário, `\{` e `\}` são literais (não abrem/fecham).

### Tokens

| Token | Forma | Exemplo |
|-------|--------|---------|
| ID | `[A-Za-z][A-Za-z0-9]*` | `x`, `soma1` |
| DEC | `0` ou `[1-9][0-9]*` | `0`, `42` |
| OCT | `0[0-7]+` | `012` → 10 |
| HEX | `0[xX][0-9A-Fa-f]+` | `0xff` → 255 |
| ASGN | `:=` | `x:=2` |
| operadores | `+ - * / ( )` | — |
| delimitador | `\n` ou `;` | — |

---

## Avaliação (ações semânticas)

A sintaxe (`match`, `switch`, `goto`) fica separada das ações de interpretação, marcadas no código como `/*N*/ ... /*N*/` (Aula 07):

| Nº | Ação | Significado |
|----|------|-------------|
| 1 | `acc = -acc` | menos unário |
| 2 | `push()` | guarda o operando esquerdo |
| 3 | `acc = pop() + acc` | soma |
| 4 | `acc = pop() - acc` | subtração |
| 5 | `acc = pop() * acc` | multiplicação |
| 6 | `acc = pop() / acc` | divisão |
| 7 | `acc` ← literal | DEC / OCT / HEX |
| 8 | `acc = recall(name)` | lê variável (R-value) |
| 9 | `store(name, acc)` | atribuição (L-value) |

O valor da expressão fica em `acc` e é impresso só se não houve erro.

---

## Exemplos de uso

```text
2+3*4              → 14
(2+3)*4            → 20
-4+5               → 1
x:=2               → 2
x+1                → 3
012                → 10
0xff               → 255
{ comentario } 1+1 → 2
2+3; 4*5           → 5   depois  20
```

---

## Erros na linha de comando

Todas as mensagens vão para **stderr**, com o número da linha. O token visto é a string do lexema (ou o caractere), não o código interno (`1024`, …).

| Situação | Mensagem (stderr) | stdout |
|----------|-------------------|--------|
| token errado (`2+`, `2)` , falta `)`) | `token inesperado '...' na linha N (esperado ...)` | nada dessa sentença |
| lixo depois da expressão (`1+2)`) | `token inesperado ... na linha N` | nada dessa sentença |
| caractere que não inicia expressão (`*1`, `)`) | `caractere inesperado ... na linha N` | nada |
| divisão por zero (`1/0`) | `divisão por zero na linha N` | nada |
| `{` sem `}` até o EOF | `comentário não fechado na linha N` | — |
| fim de arquivo no meio da expressão | `EOF prematuro na linha N (esperado ...)` | nada |

A sentença com erro é descartada até `\n` ou `;`. A seguinte é avaliada.

```bash
printf '2+\n3*4\n' | ./mybc
# stderr: token inesperado ... na linha 1 (esperado '(')
# stdout: 12
```

```bash
printf '{ sem fechar' | ./mybc
# stderr: comentário não fechado na linha 1
```

---

## Continuidade após erro

Na versão mínima, `match()` chamava `exit(-2)` e o interpretador morria no primeiro erro. Agora:

1. `match()` imprime o diagnóstico, liga `has_error` e **não** consome o token.
2. `expr()` / `match()` retornam imediatamente se `has_error` (evita laço infinito nos `goto`).
3. `mybc()` **não** imprime `acc` quando há erro.
4. `skip_to_stmt_end()` avança até `\n`, `;` ou EOF.
5. A pilha (`sp`) é zerada no início de cada sentença.

O mesmo vale para comentário não fechado: não há `exit(-4)`.
