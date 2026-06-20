// Tests for C DotENV
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#define CDOTENV_STB_IMPLEMENTATION
#include "cdotenv-stb.h"

void test_tokenizer(void) {
    const char* simpleValue = "#comment\nkey=value\ndkey=\"double quoted\"";
    size_t len = strlen(simpleValue);
    size_t offset = 0;
    size_t previous = 0;
    int token = cdotenvNextToken(&offset, simpleValue, len);
    assert(token == CDOTENV_TOKEN_TYPE_HASH);

    previous = offset;
    token = cdotenvNextToken(&offset, simpleValue, len);
    assert(token == CDOTENV_TOKEN_TYPE_STRING);
    char *key = strndup(simpleValue+previous, offset-previous);
    assert(strncmp(key, "key", 3) == 0);

    token = cdotenvNextToken(&offset, simpleValue, len);
    assert(token == CDOTENV_TOKEN_TYPE_EQUALS);

    previous = offset;
    token = cdotenvNextToken(&offset, simpleValue, len);
    assert(token == CDOTENV_TOKEN_TYPE_STRING);
    char *value = strndup(simpleValue+previous, offset-previous);
    assert(strncmp(value, "value", 5) == 0);

    token = cdotenvNextToken(&offset, simpleValue, len);
    assert(token == CDOTENV_TOKEN_TYPE_NEWLINE);

    previous = offset;
    token = cdotenvNextToken(&offset, simpleValue, len);
    assert(token == CDOTENV_TOKEN_TYPE_STRING);
    char *dkey = strndup(simpleValue+previous, offset-previous);
    assert(strncmp(dkey, "dkey", 4) == 0);

    token = cdotenvNextToken(&offset, simpleValue, len);
    assert(token == CDOTENV_TOKEN_TYPE_EQUALS);

    token = cdotenvNextToken(&offset, simpleValue, len);
    assert(token == CDOTENV_TOKEN_TYPE_DOUBLE_QUOTE_OPEN);

    previous = offset;
    token = cdotenvNextToken(&offset, simpleValue, len);
    assert(token == CDOTENV_TOKEN_TYPE_STRING);
    char *dvalue = strndup(simpleValue+previous, offset-previous);
    assert(strncmp(dvalue, "double quoted", 13) == 0);

    token = cdotenvNextToken(&offset, simpleValue, len);
    assert(token == CDOTENV_TOKEN_TYPE_DOUBLE_QUOTE_CLOSE);

    token = cdotenvNextToken(&offset, simpleValue, len);
    assert(token == CDOTENV_TOKEN_TYPE_EOF);

    printf("Tokenizer test passed\n");
}

int main(void) {
    // TODO: Implement tests
    test_tokenizer();
    return 0;
}