/* Confere o sizeof de keywordList (calculado em keywords.c) e o isKeyword. */

#include <stdio.h>
#include <keywords.h>

static const char *keywords[] = {
    "begin", "if", "then", "else", "while", "do", "repeat", "until",
    "not", "or", "div", "mod", "and", "true", "false", "end",
};

static const char *not_keywords[] = {
    "Begin", "BEGIN", "beg", "endx", "endif", "x", "variable", "",
};

int main(void)
{
    int expected = (int)(sizeof(keywords) / sizeof(keywords[0]));
    int nnot = (int)(sizeof(not_keywords) / sizeof(not_keywords[0]));
    int fails = 0;
    int i;

    printf("keylistLen = %d (sizeof(keywordList) / sizeof(keywordList[0]) em keywords.c)\n",
           keylistLen);
    printf("esperado   = %d\n", expected);

    if (keylistLen != expected) {
        printf("FALHOU: sizeof deu %d\n", keylistLen);
        fails++;
    }

    for (i = 0; i < expected; i++) {
        int token = isKeyword(keywords[i]);
        printf("  isKeyword(\"%s\") = %d %s\n", keywords[i], token, token ? "ok" : "FALHOU");
        if (!token)
            fails++;
    }

    for (i = 0; i < nnot; i++) {
        int token = isKeyword(not_keywords[i]);
        printf("  isKeyword(\"%s\") = %d %s\n",
               not_keywords[i], token, token ? "FALHOU" : "ok");
        if (token)
            fails++;
    }

    if (fails) {
        printf("%d falha(s)\n", fails);
        return 1;
    }

    printf("ok: sizeof contou %d e isKeyword reconheceu todas\n", keylistLen);
    return 0;
}
