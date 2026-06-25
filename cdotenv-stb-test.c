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
    int token = cdotenvNextToken(&offset, simpleValue, len, true);
    assert(token == CDOTENV_TOKEN_TYPE_HASH);

    previous = offset;
    token = cdotenvNextToken(&offset, simpleValue, len, false);
    assert(token == CDOTENV_TOKEN_TYPE_STRING);
    char *key = strndup(simpleValue+previous, offset-previous);
    assert(strncmp(key, "key", 3) == 0);

    token = cdotenvNextToken(&offset, simpleValue, len, false);
    assert(token == CDOTENV_TOKEN_TYPE_EQUALS);

    previous = offset;
    token = cdotenvNextToken(&offset, simpleValue, len,false);
    assert(token == CDOTENV_TOKEN_TYPE_STRING);
    char *value = strndup(simpleValue+previous, offset-previous);
    assert(strncmp(value, "value", 5) == 0);

    token = cdotenvNextToken(&offset, simpleValue, len, false);
    assert(token == CDOTENV_TOKEN_TYPE_NEWLINE);

    previous = offset;
    token = cdotenvNextToken(&offset, simpleValue, len, false);
    assert(token == CDOTENV_TOKEN_TYPE_STRING);
    char *dkey = strndup(simpleValue+previous, offset-previous);
    assert(strncmp(dkey, "dkey", 4) == 0);

    token = cdotenvNextToken(&offset, simpleValue, len, false);
    assert(token == CDOTENV_TOKEN_TYPE_EQUALS);

    token = cdotenvNextToken(&offset, simpleValue, len, false);
    assert(token == CDOTENV_TOKEN_TYPE_DOUBLE_QUOTE_OPEN);

    previous = offset;
    token = cdotenvNextToken(&offset, simpleValue, len, false);
    assert(token == CDOTENV_TOKEN_TYPE_STRING);
    char *dvalue = strndup(simpleValue+previous, offset-previous);
    assert(strncmp(dvalue, "double quoted", 13) == 0);

    token = cdotenvNextToken(&offset, simpleValue, len, false);
    assert(token == CDOTENV_TOKEN_TYPE_DOUBLE_QUOTE_CLOSE);

    token = cdotenvNextToken(&offset, simpleValue, len, false);
    assert(token == CDOTENV_TOKEN_TYPE_NEWLINE);

    previous = offset;
    token = cdotenvNextToken(&offset, simpleValue, len, false);
    assert(token == CDOTENV_TOKEN_TYPE_STRING);
    char *skey = strndup(simpleValue+previous, offset-previous);
    assert(strncmp(skey, "skey", 4) == 0);

    token = cdotenvNextToken(&offset, simpleValue, len, false);
    assert(token == CDOTENV_TOKEN_TYPE_EQUALS);

    token = cdotenvNextToken(&offset, simpleValue, len, false);
    assert(token == CDOTENV_TOKEN_TYPE_SINGLE_QUOTE_OPEN);

    previous = offset;
    token = cdotenvNextToken(&offset, simpleValue, len, false);
    assert(token == CDOTENV_TOKEN_TYPE_STRING);
    char *svalue = strndup(simpleValue+previous, offset-previous);
    assert(strncmp(svalue, "\"single quoted\"", 15) == 0);

    token = cdotenvNextToken(&offset, simpleValue, len, false);
    assert(token == CDOTENV_TOKEN_TYPE_SINGLE_QUOTE_CLOSE);

    token = cdotenvNextToken(&offset, simpleValue, len, false);
    assert(token == CDOTENV_TOKEN_TYPE_EOF);

    // Tokenize triple values
    offset = 0;
    previous = 0;
    token = cdotenvNextToken(&offset, tripleValue, tlen, true);
    assert(token == CDOTENV_TOKEN_TYPE_STRING);
    char *tdkey = strndup(tripleValue+previous, offset-previous);
    assert(strncmp(tdkey, "tdkey", 5) == 0);

    token = cdotenvNextToken(&offset, tripleValue, tlen, false);
    assert(token == CDOTENV_TOKEN_TYPE_EQUALS);

    token = cdotenvNextToken(&offset, tripleValue, tlen, false);
    assert(token == CDOTENV_TOKEN_TYPE_DOUBLE_QUOTE_OPEN_TRIPLE);

    previous = offset;
    token = cdotenvNextToken(&offset, tripleValue, tlen, false);
    assert(token == CDOTENV_TOKEN_TYPE_STRING);
    char *tdvalue = strndup(tripleValue+previous, offset-previous);
    assert(strncmp(tdvalue, "\n#one\n\"two\"\nthree\n", 24) == 0);

    token = cdotenvNextToken(&offset, tripleValue, tlen, false);
    assert(token == CDOTENV_TOKEN_TYPE_DOUBLE_QUOTE_CLOSE_TRIPLE);

    token = cdotenvNextToken(&offset, tripleValue, tlen, false);
    assert(token == CDOTENV_TOKEN_TYPE_NEWLINE);

    previous = offset;
    token = cdotenvNextToken(&offset, tripleValue, tlen, false);
    assert(token == CDOTENV_TOKEN_TYPE_STRING);
    char *tskey = strndup(tripleValue+previous, offset-previous);
    assert(strncmp(tskey, "tskey", 5) == 0);

    token = cdotenvNextToken(&offset, tripleValue, tlen, false);
    assert(token == CDOTENV_TOKEN_TYPE_EQUALS);

    token = cdotenvNextToken(&offset, tripleValue, tlen, false);
    assert(token == CDOTENV_TOKEN_TYPE_SINGLE_QUOTE_OPEN_TRIPLE);

    previous = offset;
    token = cdotenvNextToken(&offset, tripleValue, tlen, false);
    assert(token == CDOTENV_TOKEN_TYPE_STRING);
    char *tsvalue = strndup(tripleValue+previous, offset-previous);
    assert(strncmp(tsvalue, "\n#one\n\"two\"\nthree\n", 24) == 0);

    token = cdotenvNextToken(&offset, tripleValue, tlen, false);
    assert(token == CDOTENV_TOKEN_TYPE_SINGLE_QUOTE_CLOSE_TRIPLE);

    token = cdotenvNextToken(&offset, tripleValue, tlen, false);
    assert(token == CDOTENV_TOKEN_TYPE_EOF);

    printf("Tokenizer test passed\n");
}

void test_tokenizer_error(void) {
    const char* simpleError = "key=value=value2";
    const char* unclosedQuote = "key='unclosed";
    size_t len = strlen(simpleError);
    size_t ulen = strlen(unclosedQuote);
    size_t offset = 0;
    size_t previous = 0;

    previous = offset;
    int token = cdotenvNextToken(&offset, simpleError, len, true);
    assert(token == CDOTENV_TOKEN_TYPE_STRING);
    char *key = strndup(simpleError+previous, offset-previous);
    assert(strncmp(key, "key", 3) == 0);

    token = cdotenvNextToken(&offset, simpleError, len, false);
    assert(token == CDOTENV_TOKEN_TYPE_EQUALS);

    token = cdotenvNextToken(&offset, simpleError, len, false);
    assert(token == CDOTENV_TOKEN_TYPE_ERROR);
    assert(offset == 9);

    offset = 0;
    previous = 0;
    token = cdotenvNextToken(&offset, unclosedQuote, ulen, true);
    assert(token == CDOTENV_TOKEN_TYPE_STRING);
    char *ukey = strndup(unclosedQuote+previous, offset-previous);
    assert(strncmp(ukey, "key", 3) == 0);

    token = cdotenvNextToken(&offset, unclosedQuote, ulen, false);
    assert(token == CDOTENV_TOKEN_TYPE_EQUALS);

    token = cdotenvNextToken(&offset, unclosedQuote, ulen, false);
    assert(token == CDOTENV_TOKEN_TYPE_SINGLE_QUOTE_OPEN);

    token = cdotenvNextToken(&offset, unclosedQuote, ulen, false);
    assert(token == CDOTENV_TOKEN_TYPE_ERROR);
    assert(offset == ulen);

    printf("Tokenizer error test passed\n");
}

void test_parser(void) {
    const char* simpleVariables = "var1=one\nvar2=two\nvar3='three'";
    size_t simpleLen = strlen(simpleVariables);
    cdotenvVars simpleVars = {NULL, 0, 0};
    cdotenvReturn status = {CDOTENV_OK, 0};

    parseDotEnv(simpleVariables, simpleLen, &simpleVars, &status);
    assert(status.errorCode == CDOTENV_OK);
    assert(simpleVars.count == 3);
    assert(strncmp(simpleVars.items[0].key, "var1", 4) == 0);
    assert(strncmp(simpleVars.items[0].value, "one", 3) == 0);
    assert(simpleVars.items[0].singleQuoted == false);
    assert(strncmp(getenv("var1"), simpleVars.items[0].value, 3) == 0);
    assert(strncmp(simpleVars.items[1].key, "var2", 4) == 0);
    assert(strncmp(simpleVars.items[1].value, "two", 3) == 0);
    assert(simpleVars.items[1].singleQuoted == false);
    assert(strncmp(getenv("var2"), simpleVars.items[1].value, 3) == 0);
    assert(strncmp(simpleVars.items[2].key, "var3", 4) == 0);
    assert(strncmp(simpleVars.items[2].value, "three", 3) == 0);
    assert(simpleVars.items[2].singleQuoted == true);
    assert(strncmp(getenv("var3"), simpleVars.items[2].value, 3) == 0);

    printf("Parser test passed\n");
}

int main(void) {
    test_tokenizer();
    test_tokenizer_error();
    test_parser();
    return 0;
}