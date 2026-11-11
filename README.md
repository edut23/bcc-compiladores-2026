# MyPas

Compilador Pascal da disciplina de **Compiladores**. Nesta etapa o foco é a análise léxica, o parser LL(1) da sintaxe (diagramas de Grogono) e o **início da geração de código** x86 (AT&T) para expressões, atribuição e controle (`IF` / `WHILE` / `REPEAT`).

O executável chama-se `mypas`. Os cabeçalhos ficam em `include/`.

---

## Compilar e executar

```bash
make              # gera ./mypas
make clean        # remove .o
make mostlyclean  # remove .o, executável e a.out
make test-iskeyword
```

### Uso

```bash
./mypas entrada.pas            # objeto em a.out
./mypas entrada.pas saida.s    # objeto no 2º argumento
./mypas < entrada.pas          # lê stdin; objeto em a.out
```

| Fluxo | Destino |
|-------|---------|
| código objeto (assembly) | arquivo `a.out` ou o 2º argumento (`objfile`) |
| diagnóstico de erro | **stderr** |

O processo **não** chama `exit` no primeiro erro de sintaxe: `match()` marca `has_error`, imprime o diagnóstico e as funções do parser retornam. O programa só encerra no fim de `program()`, com status `0`.

---

## Arquitetura

```
fonte (arquivo ou stdin)          → targetfile
    → gettoken()                  lexer.c
    → lookahead / match()         parser.c
    → program()                   parser.c
         ├─ labels IF/WHILE/REPEAT
         └─ movl / pushl / ops    → objfile (a.out ou argv[2])
    → stderr (erro) ou silêncio se a sintaxe estiver ok
```

| Arquivo | Papel |
|---------|--------|
| `main.c` | Abre `targetfile` / `objfile`, lê o 1º token e chama `program()` |
| `lexer.c` / `include/lexer.h` | Tokens, `lexeme[]`, comentários `{...}`, contador `line`, `isNUM` |
| `parser.c` / `include/parser.h` | Parser LL(1) MyPas + emissão de assembly |
| `keywords.c` / `include/keywords.h` | Palavras-reservadas e `iskeyword()` |
| `symtab.c` / `include/symtab.h` | Tabela de símbolos (`lookup` / `append`) — ainda pouco usada |
| `vmachine.c` / `include/vmachine.h` | Acumulador e pilha (ainda não linkados no Makefile) |
| `include/tokens.h` | `ID`, `UINT`, `FLT`, `ASGN`, `LEQ`, `NEQ`, `GEQ` |

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
stmt     → ID [ := expr | arglist ]
         | BEGIN stmtlist END
         | IF expr THEN stmt [ ELSE stmt ]
         | REPEAT stmtlist UNTIL expr
         | WHILE expr DO stmt
         | ε
type     → INTEGER | REAL | DOUBLE | BOOLEAN
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
| ID | `[A-Za-z][A-Za-z0-9_]*` (se não for palavra-reservada) | `x`, `soma1` |
| palavra-reservada | ver `keywords.c` | `program`, `begin`, `div` |
| UINT | literal numérico sem `.` / `E` (`isNUM`) | `42` |
| FLT | literal com `.` ou expoente | `3.14`, `1e3` |
| ASGN | `:=` | `b := a` |
| LEQ / NEQ / GEQ | `<=` `<>` `>=` | `a <= b` |
| operadores | `+ - * / ( ) < > =` | — |
| delimitadores | `,` `:` `;` `.` | — |

### Geração de código (etapa atual)

- Expressões: resultado em `%eax`; operandos esquerdos via `pushl` / pilha.
- Relacionais: `cmpl` + `setcc` → `0`/`1` em `%eax` (serve para o `jz` de `IF`/`WHILE`/`REPEAT`).
- Atribuição: `movl %eax, var` após `expr`.
- Controle: rótulos `.Ln` e saltos `jz` / `jmp` (`loopcount`).

O assembly emitido ainda **não** é um programa linkável completo (faltam diretivas, seção de dados, `main`, etc.): é o esqueleto da aula.

---

## Erros na linha de comando

Todas as mensagens vão para **stderr**, com o número da linha. O token visto é o lexema (ou a palavra-reservada), não o código interno (`1024`, `0x10000`, …).

| Situação | Mensagem (stderr) |
|----------|-------------------|
| token errado (`program 1;` , falta `)`) | `token inesperado '...' na linha N (esperado ...)` |
| fim de arquivo no meio do programa | `EOF prematuro na linha N (esperado ...)` |
| `{` sem `}` até o EOF | `comentário não fechado na linha N` |
| arquivo de entrada inexistente | `file ... not found. Exiting with error status.` |
| não abre o arquivo de saída | `cannot open output file ...` |

```bash
./mypas test-pas.pas out.s
# silêncio + out.s com assembly se a sintaxe estiver ok

printf 'begin end.\n' | ./mypas
# stderr: token inesperado begin na linha 1 (esperado program)
```

---

## Continuidade após erro

Na versão mínima da aula, `match()` chama `exit(-2)` e o compilador morre no primeiro mismatch. Aqui:

1. `match()` imprime o diagnóstico, liga `has_error` e **não** consome o token.
2. As rotinas com `goto` (`idlist`, `block`, `stmtlist`, `smpexpr`, …) retornam se `has_error` (evita laço infinito).
3. Não há `exit(-2)` / `exit(-4)`.
