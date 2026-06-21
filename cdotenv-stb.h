// C DotENV Single File Library

#ifndef CDOTENV_STB_H
#define CDOTENV_STB_H
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct cdotenvKV {
    char* key;
    char* value;
} cdotenvKV;

typedef struct cdotenvVars {
    cdotenvKV* items;
    size_t count;
    size_t capacity;
} cdotenvVars;

void parseDotEnv(const char* s, size_t size, cdotenvVars* vars);
void loadDotEnv(const char* filename);
int cdotenvNextToken(size_t *offset, const char *buffer, size_t size);

#define CDOTENV_TOKEN_TYPE_ERROR -1
#define CDOTENV_TOKEN_TYPE_EOF 0
#define CDOTENV_TOKEN_TYPE_STRING 1
#define CDOTENV_TOKEN_TYPE_EQUALS 2
#define CDOTENV_TOKEN_TYPE_HASH 3
#define CDOTENV_TOKEN_TYPE_NEWLINE 4
#define CDOTENV_TOKEN_TYPE_DOUBLE_QUOTE_OPEN 5
#define CDOTENV_TOKEN_TYPE_DOUBLE_QUOTE_OPEN_TRIPLE 6
#define CDOTENV_TOKEN_TYPE_DOUBLE_QUOTE_CLOSE 7
#define CDOTENV_TOKEN_TYPE_DOUBLE_QUOTE_CLOSE_TRIPLE 8
#define CDOTENV_TOKEN_TYPE_SINGLE_QUOTE_OPEN 9
#define CDOTENV_TOKEN_TYPE_SINGLE_QUOTE_OPEN_TRIPLE 10
#define CDOTENV_TOKEN_TYPE_SINGLE_QUOTE_CLOSE 11
#define CDOTENV_TOKEN_TYPE_SINGLE_QUOTE_CLOSE_TRIPLE 12

static inline void cdotenvVarsAppend(cdotenvVars *vars, cdotenvKV kv) {
    if (vars->count >= vars->capacity) {
        vars->capacity = vars->capacity ? vars->capacity * 2 : 256;
        vars->items = realloc(vars->items, vars->capacity * sizeof(cdotenvKV));
    }
    vars->items[vars->count++] = kv;
};

#ifdef CDOTENV_STB_IMPLEMENTATION
int cdotenvNextToken(size_t *offset, const char *buffer, size_t size) {
    static bool singleQuoteOpen = false;
    static bool doubleQuoteOpen = false;
    static bool tripleSingleQuoteOpen = false;
    static bool tripleDoubleQuoteOpen = false;

    if (!buffer || !size || *offset >= size) return CDOTENV_TOKEN_TYPE_EOF;

    if (buffer[*offset] == '=' && !(tripleSingleQuoteOpen || tripleDoubleQuoteOpen || doubleQuoteOpen || singleQuoteOpen)) {
        (*offset)++;
        return CDOTENV_TOKEN_TYPE_EQUALS;
    }

    if (buffer[*offset] == '\n' && !(tripleSingleQuoteOpen || tripleDoubleQuoteOpen)) {
        (*offset)++;
        return CDOTENV_TOKEN_TYPE_NEWLINE;
    }

    if (buffer[*offset] == '#' && !(tripleSingleQuoteOpen || tripleDoubleQuoteOpen || doubleQuoteOpen || singleQuoteOpen)) {
        while (*offset < size && buffer[*offset] != '\n') (*offset)++;
        (*offset)++;
        return CDOTENV_TOKEN_TYPE_HASH;
    }

    if (buffer[*offset] == '"' && buffer[*offset + 1] == '"' && buffer[*offset + 2] == '"') {
        (*offset) += 3;
        tripleDoubleQuoteOpen = !tripleDoubleQuoteOpen;
        return tripleDoubleQuoteOpen ? CDOTENV_TOKEN_TYPE_DOUBLE_QUOTE_OPEN_TRIPLE : CDOTENV_TOKEN_TYPE_DOUBLE_QUOTE_CLOSE_TRIPLE;
    }

    if (buffer[*offset] == '\'' && buffer[*offset + 1] == '\'' && buffer[*offset + 2] == '\'') {
        (*offset) += 3;
        tripleSingleQuoteOpen = !tripleSingleQuoteOpen;
        return tripleSingleQuoteOpen ? CDOTENV_TOKEN_TYPE_SINGLE_QUOTE_OPEN_TRIPLE : CDOTENV_TOKEN_TYPE_SINGLE_QUOTE_CLOSE_TRIPLE;
    }

    if (buffer[*offset] == '"' && !singleQuoteOpen && !tripleSingleQuoteOpen && !tripleDoubleQuoteOpen) {
        (*offset)++;
        doubleQuoteOpen = !doubleQuoteOpen;
        return doubleQuoteOpen ? CDOTENV_TOKEN_TYPE_DOUBLE_QUOTE_OPEN : CDOTENV_TOKEN_TYPE_DOUBLE_QUOTE_CLOSE;
    }

    if (buffer[*offset] == '\'' && !tripleSingleQuoteOpen && !tripleDoubleQuoteOpen) {
        (*offset)++;
        singleQuoteOpen = !singleQuoteOpen;
        return singleQuoteOpen ? CDOTENV_TOKEN_TYPE_SINGLE_QUOTE_OPEN : CDOTENV_TOKEN_TYPE_SINGLE_QUOTE_CLOSE;
    }

    while ((*offset) < size) {
        if (tripleDoubleQuoteOpen && buffer[*offset] == '"' && buffer[*offset + 1] == '"' && buffer[*offset + 2] == '"') {
            break;
        }
        if (doubleQuoteOpen && buffer[*offset] == '"') {
            break;
        }
        if (doubleQuoteOpen && buffer[*offset] == '\n') {
            return CDOTENV_TOKEN_TYPE_ERROR;
        }

        if (tripleSingleQuoteOpen && buffer[*offset] == '\'' && buffer[*offset + 1] == '\'' && buffer[*offset + 2] == '\'') {
            break;
        }
        if (singleQuoteOpen && buffer[*offset] == '\'') {
            break;
        }
        if (singleQuoteOpen && buffer[*offset] == '\n') {
            return CDOTENV_TOKEN_TYPE_ERROR;
        }

        if (!singleQuoteOpen && !tripleDoubleQuoteOpen && !tripleSingleQuoteOpen && buffer[*offset] == '"') {
            return CDOTENV_TOKEN_TYPE_ERROR;
        }

        if ((buffer[*offset] == ' ' || buffer[*offset] == '\t') && !(singleQuoteOpen || doubleQuoteOpen || tripleSingleQuoteOpen || tripleDoubleQuoteOpen)) {
            return CDOTENV_TOKEN_TYPE_ERROR;
        }

        if (buffer[*offset] == '=' && !(singleQuoteOpen || doubleQuoteOpen || tripleSingleQuoteOpen || tripleDoubleQuoteOpen)) {
            break;
        }

        if (buffer[*offset] == '\n' && !(tripleSingleQuoteOpen || tripleDoubleQuoteOpen)) {
            break;
        }
        (*offset)++;
    }

    return CDOTENV_TOKEN_TYPE_STRING;
}

void parseDotEnv(const char* s, size_t size, cdotenvVars* vars) {
    if (vars == NULL) return;
    size_t offset = 0;
    int currentToken = cdotenvNextToken(&offset, s, size);
    while (currentToken != CDOTENV_TOKEN_TYPE_EOF && currentToken != CDOTENV_TOKEN_TYPE_ERROR) {
        switch (currentToken) {
            // TODO: Implement token handling, build vars, write to environment
        }
        currentToken = cdotenvNextToken(&offset, s, size);
    }
}

void loadDotEnv(const char* filename) {
    FILE *file = fopen(filename, "r");
    if (!file) return;

    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = malloc(size + 1);
    if (!buffer) {
        fclose(file);
        return;
    }
    fread(buffer, size, 1, file);
    fclose(file);

    cdotenvVars vars = {NULL, 0, 0 };
    parseDotEnv(buffer, size, &vars);
}
#endif

#endif //CDOTENV_STB_H