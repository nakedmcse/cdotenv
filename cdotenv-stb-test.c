// Tests for C DotENV
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#define CDOTENV_STB_IMPLEMENTATION
#include "cdotenv-stb.h"

void test_tokenizer(void) {
    const char* simpleValue = "#comment\nkey=value\ndkey=\"double quoted\"\nskey='\"single quoted\"'";
    const char* tripleValue = "tdkey=\"\"\"\n#one\n\"two\"\nthree\n\"\"\"\ntskey='''\n#one\n\"two\"\nthree\n'''";
    size_t len = strlen(simpleValue);
    size_t tlen = strlen(tripleValue);
    size_t offset = 0;
    size_t previous = 0;

    // Tokenize simple values
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
    assert(token == CDOTENV_TOKEN_TYPE_NEWLINE);

    previous = offset;
    token = cdotenvNextToken(&offset, simpleValue, len);
    assert(token == CDOTENV_TOKEN_TYPE_STRING);
    char *skey = strndup(simpleValue+previous, offset-previous);
    assert(strncmp(skey, "skey", 4) == 0);

    token = cdotenvNextToken(&offset, simpleValue, len);
    assert(token == CDOTENV_TOKEN_TYPE_EQUALS);

    token = cdotenvNextToken(&offset, simpleValue, len);
    assert(token == CDOTENV_TOKEN_TYPE_SINGLE_QUOTE_OPEN);

    previous = offset;
    token = cdotenvNextToken(&offset, simpleValue, len);
    assert(token == CDOTENV_TOKEN_TYPE_STRING);
    char *svalue = strndup(simpleValue+previous, offset-previous);
    assert(strncmp(svalue, "\"single quoted\"", 15) == 0);

    token = cdotenvNextToken(&offset, simpleValue, len);
    assert(token == CDOTENV_TOKEN_TYPE_SINGLE_QUOTE_CLOSE);

    token = cdotenvNextToken(&offset, simpleValue, len);
    assert(token == CDOTENV_TOKEN_TYPE_EOF);

    // Tokenize triple values
    offset = 0;
    previous = 0;
    token = cdotenvNextToken(&offset, tripleValue, tlen);
    assert(token == CDOTENV_TOKEN_TYPE_STRING);
    char *tdkey = strndup(tripleValue+previous, offset-previous);
    assert(strncmp(tdkey, "tdkey", 5) == 0);

    token = cdotenvNextToken(&offset, tripleValue, tlen);
    assert(token == CDOTENV_TOKEN_TYPE_EQUALS);

    token = cdotenvNextToken(&offset, tripleValue, tlen);
    assert(token == CDOTENV_TOKEN_TYPE_DOUBLE_QUOTE_OPEN_TRIPLE);

    previous = offset;
    token = cdotenvNextToken(&offset, tripleValue, tlen);
    assert(token == CDOTENV_TOKEN_TYPE_STRING);
    char *tdvalue = strndup(tripleValue+previous, offset-previous);
    assert(strncmp(tdvalue, "\n#one\n\"two\"\nthree\n", 24) == 0);

    token = cdotenvNextToken(&offset, tripleValue, tlen);
    assert(token == CDOTENV_TOKEN_TYPE_DOUBLE_QUOTE_CLOSE_TRIPLE);

    token = cdotenvNextToken(&offset, tripleValue, tlen);
    assert(token == CDOTENV_TOKEN_TYPE_NEWLINE);

    previous = offset;
    token = cdotenvNextToken(&offset, tripleValue, tlen);
    assert(token == CDOTENV_TOKEN_TYPE_STRING);
    char *tskey = strndup(tripleValue+previous, offset-previous);
    assert(strncmp(tskey, "tskey", 5) == 0);

    token = cdotenvNextToken(&offset, tripleValue, tlen);
    assert(token == CDOTENV_TOKEN_TYPE_EQUALS);

    token = cdotenvNextToken(&offset, tripleValue, tlen);
    assert(token == CDOTENV_TOKEN_TYPE_SINGLE_QUOTE_OPEN_TRIPLE);

    previous = offset;
    token = cdotenvNextToken(&offset, tripleValue, tlen);
    assert(token == CDOTENV_TOKEN_TYPE_STRING);
    char *tsvalue = strndup(tripleValue+previous, offset-previous);
    assert(strncmp(tsvalue, "\n#one\n\"two\"\nthree\n", 24) == 0);

    token = cdotenvNextToken(&offset, tripleValue, tlen);
    assert(token == CDOTENV_TOKEN_TYPE_SINGLE_QUOTE_CLOSE_TRIPLE);

    token = cdotenvNextToken(&offset, tripleValue, tlen);
    assert(token == CDOTENV_TOKEN_TYPE_EOF);

    printf("Tokenizer test passed\n");
}

int main(void) {
    test_tokenizer();
    return 0;
}