# MyPas

Compilador Pascal da disciplina de **Compiladores**. Nesta etapa o foco é a análise léxica e o parser LL(1) da sintaxe (diagramas de Grogono). A geração de código ainda não está ligada: o programa é reconhecido ou rejeitado, sem emitir objeto.

O executável chama-se `mypas`. Os cabeçalhos ficam em `include/`.

---

## Compilar e executar

```bash
make              # gera ./mypas
make clean        # remove .o
make mostlyclean  # remove .o e o executável
make test-iskeyword
```

A entrada é a **stdin**.

```bash
./mypas < test-pas.pas
```

| Fluxo | Destino |
|-------|---------|
| código objeto (ainda não emitido) | **stdout** (`output`) |
| diagnóstico de erro | **stderr** |

O processo **não** chama `exit` no primeiro erro de sintaxe: `match()` marca `has_error`, imprime o diagnóstico e as funções do parser retornam. O programa só encerra no fim de `program()`, com status `0`.

---

## Arquitetura

```
fonte (stdin)
    → gettoken()          lexer.c
    → lookahead / match() parser.c
    → program()           parser.c  (sintaxe Pascal)
         └─ (futuro)      symtab.c + vmachine.c
    → stderr (erro) ou silêncio se a sintaxe estiver ok
```

| Arquivo | Papel |
|---------|--------|
| `main.c` | Abre stdin/stdout, lê o 1º token e chama `program()` |
| `lexer.c` / `include/lexer.h` | Tokens, `lexeme[]`, comentários `{...}`, contador `line` |
| `parser.c` / `include/parser.h` | Parser LL(1) MyPas, `match()`, `has_error` |
| `keywords.c` / `include/keywords.h` | Palavras-reservadas e `iskeyword()` |
| `symtab.c` / `include/symtab.h` | Tabela de símbolos (`lookup` / `append`) |
| `vmachine.c` / `include/vmachine.h` | Acumulador e pilha (ainda não usados no parser) |
| `include/tokens.h` | `ID`, `DEC`, `FLT`, `OCT`, `HEX`, `ASGN`, `LEQ`, `NEQ`, `GEQ` |

---

## Linguagem reconhecida

```
program  → PROGRAM ID ( idlist ) ; block .
idlist   → ID { , ID }
block    → [ VAR idlist : type ; { idlist : type ; } ]
           { (PROCEDURE | FUNCTION) ID parmlist [ : type ] ; block ; }
           BEGIN stmtlist END
parmlist → [ ( [VAR] idlist : type { ; [VAR] idlist : type } ) ]
stmtlist → stmt { ; stmt }
stmt     → ID [ := expr | parmlist ]
         | BEGIN stmtlist END
         | IF expr THEN stmt [ ELSE stmt ]
         | REPEAT stmtlist UNTIL expr
         | WHILE expr DO stmt
         | ε
type     → INTEGER | REAL | BOOLEAN
expr     → smpexpr [ relop smpexpr ]
smpexpr  → [ + | - ] factor { ( * | / | DIV | MOD | AND ) factor } { ( + | - ) term }
relop    → < | <= | <> | > | >= | =
```

- Palavras-reservadas são reconhecidas no léxico (`iskeyword`); `begin` não é `ID`.
- `*` `/` `div` `mod` `and` têm precedência sobre `+` `-`.
- Sinal unário opcional no início da expressão simples (`-a+b`).
- Atribuição `:=` é associativa à direita (`x := y := 1` dentro de expressão).
- Newline é espaço em branco (Pascal), não delimitador de sentença.
- Comentários `{ ... }`. Dentro do comentário, `\{` e `\}` são literais.

Ainda **não** entram (constam nos diagramas, mas a aula ainda não chegou): `const`/`type`/`label`, `for`/`case`/`goto`/`with`, `not`/`or`/`true`/`false` como fator, arrays, records, etc.

### Tokens

| Token | Forma | Exemplo |
|--------|--------|---------|
| ID | `[A-Za-z][A-Za-z0-9]*` (se não for palavra-reservada) | `x`, `soma1` |
| palavra-reservada | ver `keywords.c` | `program`, `begin`, `div` |
| FLT | literal numérico via `fscanf("%lg")` | `3.14`, `42` |
| DEC | `0` ou `[1-9][0-9]*` (fallback; em geral o número já virou FLT) | `0` |
| OCT | `0[0-7]+` | `012` |
| HEX | `0[xX][0-9A-Fa-f]+` | `0xff` |
| ASGN | `:=` | `b := a` |
| LEQ / NEQ / GEQ | `<=` `<>` `>=` | `a <= b` |
| operadores | `+ - * / ( ) < > =` | — |
| delimitadores | `,` `:` `;` `.` | — |

OCT e HEX são tentados **antes** de FLT, senão `012` e `0xff` virariam um float.

---

## Erros na linha de comando

Todas as mensagens vão para **stderr**, com o número da linha. O token visto é o lexema (ou a palavra-reservada), não o código interno (`1024`, `2048`, …).

| Situação | Mensagem (stderr) |
|----------|-------------------|
| token errado (`program 1;` , falta `)`) | `token inesperado '...' na linha N (esperado ...)` |
| fim de arquivo no meio do programa | `EOF prematuro na linha N (esperado ...)` |
| `{` sem `}` até o EOF | `comentário não fechado na linha N` |

```bash
printf 'program p(input, output);\nbegin\nend.\n' | ./mypas
# silêncio: sintaxe ok

printf 'begin end.\n' | ./mypas
# stderr: token inesperado begin na linha 1 (esperado program)
```

O `test-pas.pas` da aula tem `b := a**2`. `**` não existe na gramática: o parser lê `a *` e depois espera um fator, não outro `*`. Para um programa válido use `b := a * a`.

---

## Continuidade após erro

Na versão mínima da aula, `match()` chama `exit(-2)` e o compilador morre no primeiro mismatch. Aqui:

1. `match()` imprime o diagnóstico, liga `has_error` e **não** consome o token.
2. As rotinas com `goto` (`idlist`, `block`, `stmtlist`, `smpexpr`, …) retornam se `has_error` (evita laço infinito).
3. Não há `exit(-2)` / `exit(-4)`.
