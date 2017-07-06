#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "tokens.h"
#include "editor.h"
#include "exectoks.h"
#include "utils.h"

void printToken(token* t) {
    switch (t->type) {
        case TT_NUMBER:
            printf("{INT %d}", t->body.integer);
            break;
        case TT_NAME:
            printf("{NAME \"");
            outputNStr(&(t->body.str));
            printf("\"}");
            break;
        case TT_VARIABLE:
            printf("{VAR \"");
            outputNStr(&(t->body.str));
            printf("\"}");
            break;
        case TT_FUNCTION:
            printf("{FN \"");
            outputNStr(&(t->body.str));
            printf("\"}");
            break;
        case TT_COMMAND:
            printf("{CMD %d}", t->body.command);
            break;
        case TT_LITERAL:
            printf("{STR \"");
            outputNStr(&(t->body.str));
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
        case TT_SEPARATOR:
            printf("{SEP}");
            break;
        default:
            printf("{ERROR}");
            break;
    }
}

void printTokens(token* toks) {
    void* t = toks;
    while (1) {
        printToken(t);
        printf(" ");
        if (tokenClass(t) == TT_NONE) {
            break;
        }
        t += tokenSize(t);
    }
    printf("\n");
}

void printProgram() {
    prgline* p = findLine(1);
    while (p->num != 0) {
        printf("%d ", p->num);
        outputNStr(&(p->str));
        printf("\n");
        p = findLine(p->num + 1);
    }
}

int metaOrError(token* t, char* line) {
    if (tokenNameEqual(t, "QUIT")) {
        return 1;
    } else if (tokenNameEqual(t, "LIST")) {
        printProgram();
    } else {
        printf("%s (%d)\n", getParseErrorMsg(), (int) (getParseErrorPos() - line) + 1);
    }
    return 0;
}

int processLine(char* line) {
    char toksBody[MAX_LINE_LEN * 2];
    token* t = (void*) toksBody;
    parseLine(line, t);
    //printTokens(t);
    if (getParseErrorPos() != NULL) {
        return metaOrError(t, line);
    }
    if (t->type != TT_NUMBER) {
        executeTokens(t);
    } else {
        injectLine(skipSpaces(skipDigits(line)), t->body.integer);
    }
    return 0;
}

void init(void* prgBody) {
    printf("\nTinyBasic 0.1-PoC\n\n");
    initEditor(prgBody);
}

void dispatch(void) {
    char line[MAX_LINE_LEN];
    int quit = 0;
    while (!quit) {
        if (!readLine(line)) {
            break;
        }
        quit = processLine(line);
    }
}

int main(void) {
    char prgBody[MAX_PRG_SIZE];
    init(prgBody);
    dispatch();
    return 0;
}