/* test_iskeyword.c
 *
 * Na VM:
 *   make test-iskeyword
 *
 * O sizeof de keywordList só é válido em keywords.c (array completo).
 * Daqui o tipo é incompleto (extern char *keywordList[]), então
 * sizeof(keywordList) nem compila. Se o cálculo tivesse sido feito
 * sobre um ponteiro, keylistLen seria 1 e só "begin" seria reconhecido.
 */

#include <stdio.h>
#include <keywords.h>

static const char *keywords[] = {
    "begin", "if", "then", "else", "while", "do",
    "repeat", "until", "not", "or", "div", "mod",
    "and", "true", "false", "end",
};

static const char *not_keywords[] = {
    "foo", "BEGIN", "iff", "ended", "x", "",
};

int main(void)
{
    int expected = (int)(sizeof(keywords) / sizeof(keywords[0]));
    int fail = 0;
    int i;

    printf("=== sizeof / isKeyword ===\n");
    printf("keylistLen (sizeof em keywords.c) = %d\n", keylistLen);
    printf("esperado                          = %d\n", expected);
    if (keylistLen == 1) {
        printf("FALHOU: sizeof viu um ponteiro, nao o array\n");
        return 1;
    }
    if (keylistLen != expected) {
        printf("FALHOU: quantidade diferente do esperado\n");
        fail = 1;
    } else {
        printf("sizeof OK: o array inteiro foi medido\n");
    }

    printf("\n--- keywords (tem que achar todas) ---\n");
    for (i = 0; i < expected; i++) {
        int tok = isKeyword(keywords[i]);
        printf("  %-8s -> %d%s\n", keywords[i], tok, tok ? "  OK" : "  FALHOU");
        if (!tok)
            fail = 1;
    }

    printf("\n--- nao-keywords (tem que ser 0) ---\n");
    for (i = 0; i < (int)(sizeof(not_keywords) / sizeof(not_keywords[0])); i++) {
        int tok = isKeyword(not_keywords[i]);
        printf("  %-8s -> %d%s\n", not_keywords[i], tok, tok ? "  FALHOU" : "  OK");
        if (tok)
            fail = 1;
    }

    printf("\nRESULTADO: %s\n", fail ? "FALHOU" : "PASSOU");
    return fail;
}
