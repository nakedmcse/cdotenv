// C DotENV Single File Library

#ifndef CDOTENV_STB_H
#define CDOTENV_STB_H
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct cdotenvKV {
    char* key;
    char* value;
    bool singleQuoted;
} cdotenvKV;

typedef struct cdotenvVars {
    cdotenvKV* items;
    size_t count;
    size_t capacity;
} cdotenvVars;

void parseDotEnv(const char* s, size_t size, cdotenvVars* vars);
void loadDotEnv(const char* filename);
int cdotenvNextToken(size_t *offset, const char *buffer, size_t size);
static bool cdotenvAppendStr(char **out, size_t *len, size_t *cap, const char *s, size_t n);
char *cdotenvExpand(const char *s);

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
static bool cdotenvAppendStr(char **out, size_t *len, size_t *cap, const char *s, size_t n) {
    if (*len + n + 1 > *cap) {
        size_t new_cap = *cap ? *cap : 64;

        while (*len + n + 1 > new_cap) {
            new_cap *= 2;
        }

        char *tmp = realloc(*out, new_cap);
        if (!tmp) return false;

        *out = tmp;
        *cap = new_cap;
    }

    memcpy(*out + *len, s, n);
    *len += n;
    (*out)[*len] = '\0';

    return true;
}

char *cdotenvExpand(const char *s) {
    if (!s) return NULL;

    char *out = NULL;
    size_t len = 0;
    size_t cap = 0;

    for (size_t i = 0; s[i] != '\0';) {
        if (s[i] == '$' && s[i + 1] == '{') {
            const char *start = s + i + 2;
            const char *end = strchr(start, '}');

            if (end) {
                size_t name_len = (size_t)(end - start);
                char *name = malloc(name_len + 1);
                if (!name) {
                    free(out);
                    return NULL;
                }

                memcpy(name, start, name_len);
                name[name_len] = '\0';
                const char *value = getenv(name);
                free(name);

                if (value) {
                    if (!cdotenvAppendStr(&out, &len, &cap, value, strlen(value))) {
                        free(out);
                        return NULL;
                    }
                }

                i = (size_t)(end - s) + 1;
                continue;
            }
        }

        if (!cdotenvAppendStr(&out, &len, &cap, s + i, 1)) {
            free(out);
            return NULL;
        }

        i++;
    }

    if (!out) {
        out = malloc(1);
        if (out) out[0] = '\0';
    }

    return out;
}

int cdotenvNextToken(size_t *offset, const char *buffer, size_t size) {
    static bool singleQuoteOpen = false;
    static bool doubleQuoteOpen = false;
    static bool tripleSingleQuoteOpen = false;
    static bool tripleDoubleQuoteOpen = false;
    static bool seenEquals = false;

    bool quoted = singleQuoteOpen || doubleQuoteOpen || tripleSingleQuoteOpen || tripleDoubleQuoteOpen;

    if (!buffer || !size || *offset >= size) {
        singleQuoteOpen = false;
        doubleQuoteOpen = false;
        tripleSingleQuoteOpen = false;
        tripleDoubleQuoteOpen = false;
        seenEquals = false;
        if (quoted) return CDOTENV_TOKEN_TYPE_ERROR;
        return CDOTENV_TOKEN_TYPE_EOF;
    }

    char c = buffer[*offset];

    if (!quoted) {
        if (c == '=') {
            (*offset)++;
            seenEquals = true;
            return CDOTENV_TOKEN_TYPE_EQUALS;
        }

        if (c == '#') {
            while (*offset < size && buffer[*offset] != '\n') (*offset)++;
            if (*offset < size) (*offset)++;
            return CDOTENV_TOKEN_TYPE_HASH;
        }
    }

    if (c == '\n' && !tripleSingleQuoteOpen && !tripleDoubleQuoteOpen) {
        (*offset)++;
        seenEquals = false;
        return CDOTENV_TOKEN_TYPE_NEWLINE;
    }

    if (*offset + 2 < size && c == '"' && buffer[*offset + 1] == '"' && buffer[*offset + 2] == '"') {
        (*offset) += 3;
        tripleDoubleQuoteOpen = !tripleDoubleQuoteOpen;

        return tripleDoubleQuoteOpen
            ? CDOTENV_TOKEN_TYPE_DOUBLE_QUOTE_OPEN_TRIPLE
            : CDOTENV_TOKEN_TYPE_DOUBLE_QUOTE_CLOSE_TRIPLE;
    }

    if (*offset + 2 < size && c == '\'' && buffer[*offset + 1] == '\'' && buffer[*offset + 2] == '\'') {
        (*offset) += 3;
        tripleSingleQuoteOpen = !tripleSingleQuoteOpen;

        return tripleSingleQuoteOpen
            ? CDOTENV_TOKEN_TYPE_SINGLE_QUOTE_OPEN_TRIPLE
            : CDOTENV_TOKEN_TYPE_SINGLE_QUOTE_CLOSE_TRIPLE;
    }

    if (c == '"' && !singleQuoteOpen && !tripleSingleQuoteOpen && !tripleDoubleQuoteOpen) {
        (*offset)++;
        doubleQuoteOpen = !doubleQuoteOpen;

        return doubleQuoteOpen
            ? CDOTENV_TOKEN_TYPE_DOUBLE_QUOTE_OPEN
            : CDOTENV_TOKEN_TYPE_DOUBLE_QUOTE_CLOSE;
    }

    if (c == '\'' && !tripleSingleQuoteOpen && !tripleDoubleQuoteOpen) {
        (*offset)++;
        singleQuoteOpen = !singleQuoteOpen;

        return singleQuoteOpen
            ? CDOTENV_TOKEN_TYPE_SINGLE_QUOTE_OPEN
            : CDOTENV_TOKEN_TYPE_SINGLE_QUOTE_CLOSE;
    }

    while (*offset < size) {
        c = buffer[*offset];

        bool inTriple = tripleSingleQuoteOpen || tripleDoubleQuoteOpen;
        bool inNormal = singleQuoteOpen || doubleQuoteOpen;
        bool inAnyQuote = inTriple || inNormal;

        if (tripleDoubleQuoteOpen &&
            *offset + 2 < size && c == '"' && buffer[*offset + 1] == '"' && buffer[*offset + 2] == '"') break;
        if (tripleSingleQuoteOpen &&
            *offset + 2 < size && c == '\'' && buffer[*offset + 1] == '\'' && buffer[*offset + 2] == '\'') break;

        if (doubleQuoteOpen && c == '"') break;
        if (singleQuoteOpen && c == '\'') break;

        if ((singleQuoteOpen || doubleQuoteOpen) && c == '\n') {
            singleQuoteOpen = false;
            doubleQuoteOpen = false;
            tripleSingleQuoteOpen = false;
            tripleDoubleQuoteOpen = false;
            seenEquals = false;
            return CDOTENV_TOKEN_TYPE_ERROR;
        }

        if (!inAnyQuote) {
            if (c == '=') {
                if (seenEquals) {
                    seenEquals = false;
                    return CDOTENV_TOKEN_TYPE_ERROR;
                }
                break;
            }
            if (c == '\n') break;

            if (c == ' ' || c == '\t') {
                seenEquals = false;
                return CDOTENV_TOKEN_TYPE_ERROR;
            }

            if (c == '"') {
                seenEquals = false;
                return CDOTENV_TOKEN_TYPE_ERROR;
            }
        }

        (*offset)++;
    }

    return quoted && *offset == size
        ? CDOTENV_TOKEN_TYPE_ERROR
        : CDOTENV_TOKEN_TYPE_STRING;
}

void parseDotEnv(const char* s, size_t size, cdotenvVars* vars) {
    if (vars == NULL) return;
    size_t offset = 0;
    size_t previous = 0;
    int currentToken = cdotenvNextToken(&offset, s, size);
    char *currentKey = NULL;
    char *currentValue = NULL;
    bool seenEquals = false;
    bool inSingleQuote = false;

    while (currentToken != CDOTENV_TOKEN_TYPE_EOF && currentToken != CDOTENV_TOKEN_TYPE_ERROR) {
        switch (currentToken) {
            case CDOTENV_TOKEN_TYPE_STRING:
                if (seenEquals && currentKey && currentValue) {
                    const cdotenvKV kv = {currentKey, currentValue, inSingleQuote};
                    cdotenvVarsAppend(vars, kv);
                }
                else if (seenEquals && currentKey) {
                    currentValue = strndup(s+previous, offset-previous);
                }
                else {
                    // TODO: Validate Key
                    currentKey = strndup(s+previous, offset-previous);
                }
                break;

            case CDOTENV_TOKEN_TYPE_EQUALS:
                seenEquals = true;
                break;

            case CDOTENV_TOKEN_TYPE_NEWLINE:
                seenEquals = false;
                break;

            case CDOTENV_TOKEN_TYPE_SINGLE_QUOTE_OPEN:
            case CDOTENV_TOKEN_TYPE_SINGLE_QUOTE_OPEN_TRIPLE:
                inSingleQuote = true;
                break;

            case CDOTENV_TOKEN_TYPE_SINGLE_QUOTE_CLOSE:
            case CDOTENV_TOKEN_TYPE_SINGLE_QUOTE_CLOSE_TRIPLE:
                inSingleQuote = false;
                break;

            default:
                break;
        }
        previous = offset;
        currentToken = cdotenvNextToken(&offset, s, size);
    }

    if (vars->count > 0) {
        for (size_t i = 0; i < vars->count; i++) {
            setenv(vars->items[i].key,
                vars->items[i].singleQuoted ? vars->items[i].value : cdotenvExpand(vars->items[i].value),
                1);
        }
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