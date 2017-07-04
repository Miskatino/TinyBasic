#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "tokens.h"
#include "editor.h"
#include "utils.h"

char line[MAX_LINE_LEN];
char toksBody[MAX_LINE_LEN * 2];

void* toks = toksBody;

int readLine(void) {
    if (fgets(line, sizeof(line), stdin) == NULL) {
        return 0;
    }
    trim(line);
    return 1;
}

void printNStr(nstring* t) {
    for (short i = 0; i < t->len; i++) {
        putc(t->text[i], stdout);
    }
};

void printToken(token* t) {
    switch (t->type) {
        case TT_NUMBER:
            printf("{INT %d}", t->body.integer);
            break;
        case TT_NAME:
            printf("{NAME \"");
            printNStr(&(t->body.str));
            printf("\"}");
            break;
        case TT_VARIABLE:
            printf("{VAR \"");
            printNStr(&(t->body.str));
            printf("\"}");
            break;
        case TT_FUNCTION:
            printf("{FN \"");
            printNStr(&(t->body.str));
            printf("\"}");
            break;
        case TT_COMMAND:
            printf("{CMD %d}", t->body.command);
            break;
        case TT_LITERAL:
            printf("{STR \"");
            printNStr(&(t->body.str));
            printf("\"}");
            break;
        case TT_SYMBOL:
            printf("{SYM '%c'}", t->body.symbol);
            break;
        case TT_FUNC_END:
            printf("{FE %d}", t->body.symbol);
            break;
        case TT_NONE:
            printf("{NONE}");
            break;
        default:
            printf("{ERROR}");
    }
}

void printTokens(void) {
    void* t = toks;
    while (1) {
        printToken(t);
        printf(" ");
        if (tokenClass(t) == TT_NONE) {
            break;
        }
        printf("tt%02x ", ((token*)t)->type);
        t += tokenSize(t);
    }
    printf("\n");
}

void printProgram() {
    prgline* p = findLine(1);
    while (p->num != 0) {
        printf("%d ", p->num);
        printNStr(&(p->str));
        printf("\n");
        p = findLine(p->num + 1);
    }
}

int processLine(void) {
    token* t = toks;
    parseLine(line, toks);
    if (tokenNameEqual(t, "QUIT")) {
        return 1;
    } else if (tokenNameEqual(t, "LIST")) {
        printProgram();
        return 0;
    }
    printTokens();
    if (getParseErrorPos() != NULL) {
        printf("Error '%s' at pos: %d\n", getParseErrorMsg(), (int) (getParseErrorPos() - line) + 1);
        return 0;
    }
    if (t->type == TT_NUMBER) {
        injectLine(skipSpaces(skipDigits(line)), t->body.integer);
    }
    return 0;
}

void init(void) {
    printf("\nTinyBasic 0.1-PoC\n\n");
    initEditor();
}

void dispatch(void) {
    int quit = 0;
    while (!quit) {
        if (!readLine()) {
            break;
        }
        quit = processLine();
    }
}

int main(void) {
    init();
    dispatch();
    return 0;
}
