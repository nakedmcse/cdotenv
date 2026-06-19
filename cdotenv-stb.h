// C DotENV Single File Library

#ifndef CDOTENV_STB_H
#define CDOTENV_STB_H
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
void loadDotEnv(const char* filename);
int cdotenvNextToken(size_t *offset, const char *buffer, size_t size);

#define CDOTENV_TOKEN_TYPE_ERROR -1
#define CDOTENV_TOKEN_TYPE_EOF 0
#define CDOTENV_TOKEN_TYPE_STRING 1
#define CDOTENV_TOKEN_TYPE_EQUALS 2
#define CDOTENV_TOKEN_TYPE_HASH 3
#define CDOTENV_TOKEN_TYPE_DOUBLE_QUOTE_OPEN 4
#define CDOTENV_TOKEN_TYPE_DOUBLE_QUOTE_OPEN_TRIPLE 5
#define CDOTENV_TOKEN_TYPE_DOUBLE_QUOTE_CLOSE 6
#define CDOTENV_TOKEN_TYPE_DOUBLE_QUOTE_CLOSE_TRIPLE 7
#define CDOTENV_TOKEN_TYPE_SINGLE_QUOTE_OPEN 8
#define CDOTENV_TOKEN_TYPE_SINGLE_QUOTE_OPEN_TRIPLE 9
#define CDOTENV_TOKEN_TYPE_SINGLE_QUOTE_CLOSE 10
#define CDOTENV_TOKEN_TYPE_SINGLE_QUOTE_CLOSE_TRIPLE 11

typedef struct cdotenvKV {
    char* key;
    char* value;
} cdotenvKV;

typedef struct cdotenvVars {
    cdotenvKV* items;
    size_t count;
    size_t capacity;
} cdotenvVars;

static inline void cdotenvVarsAppend(cdotenvVars *vars, cdotenvKV kv) {
    if (vars->count >= vars->capacity) {
        vars->capacity = vars->capacity ? vars->capacity * 2 : 256;
        vars->items = realloc(vars->items, vars->capacity * sizeof(cdotenvKV));
    }
    vars->items[vars->count++] = kv;
};

#ifdef CDOTENV_STB_IMPLEMENTATION
void loadDotEnv(const char* filename) {
    FILE *file = fopen(filename, "r");
    if (!file) return;

    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = malloc(size + 1);
    if (!buffer) {
        fclose(fp);
        return;
    }
    fread(buffer, size, 1, file);
    fclose(file);

    size_t offset = 0;
    cdotenvVars vars = {NULL, 0, 0 };
    int currentToken = cdotenvNextToken(&offset, buffer, size);
    while (currentToken != CDOTENV_TOKEN_TYPE_EOF && currentToken != CDOTENV_TOKEN_TYPE_ERROR) {
        switch (currentToken) {
            // TODO: Implement token handling, build vars, write to environment
        }
        currentToken = cdotenvNextToken(&offset, buffer, size);
    }
}

int cdotenvNextToken(size_t *offset, const char *buffer, size_t size) {
    static bool singleQuoteOpen = false;
    static bool doubleQuoteOpen = false;
    static bool tripleSingleQuoteOpen = false;
    static bool tripleDoubleQuoteOpen = false;

    if (!buffer || !size || *offset >= size) return CDOTENV_TOKEN_TYPE_EOF;

    if (buffer[*offset] == '=') {
        (*offset)++;
        return CDOTENV_TOKEN_TYPE_EQUALS;
    }

    if (buffer[*offset] == '#') {
        (*offset)++;
        return CDOTENV_TOKEN_TYPE_HASH;
    }

    // TODO: Implement remaining tokens

    return CDOTENV_TOKEN_TYPE_ERROR;
}

#endif

#endif //CDOTENV_STB_H