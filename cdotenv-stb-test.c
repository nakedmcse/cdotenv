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
    assert(strncmp(tdvalue, "#one\n\"two\"\nthree\n", 24) == 0);

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
    assert(strncmp(tsvalue, "#one\n\"two\"\nthree\n", 24) == 0);

    token = cdotenvNextToken(&offset, tripleValue, tlen, false);
    assert(token == CDOTENV_TOKEN_TYPE_SINGLE_QUOTE_CLOSE_TRIPLE);

    token = cdotenvNextToken(&offset, tripleValue, tlen, false);
    assert(token == CDOTENV_TOKEN_TYPE_EOF);

    printf("Tokenizer test passed\n");
}

void test_tokenizer_error(void) {
    const char* simpleError = "key=value=value2";
    size_t len = strlen(simpleError);
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

    printf("Tokenizer error test passed\n");
}

void test_parser(bool nomalloc) {
    const char* simpleVariables = "var1=one\nvar2=two\nvar3='three'";
    size_t simpleLen = strlen(simpleVariables);

    if (nomalloc) {
        cdotenvKVnomalloc kvs[CDOTENV_NOMALLOC_MAX_ITEMS];
        cdotenvVars simpleVars = {NULL, kvs, 0, CDOTENV_NOMALLOC_MAX_ITEMS};
        cdotenvReturn status = {CDOTENV_OK, 0};

        parseDotEnv(simpleVariables, simpleLen, &simpleVars, &status, nomalloc);
        assert(status.errorCode == CDOTENV_OK);
        assert(simpleVars.count == 3);
        assert(strncmp(simpleVars.itemsNoMalloc[0].key, "var1", 4) == 0);
        assert(strncmp(simpleVars.itemsNoMalloc[0].value, "one", 3) == 0);
        assert(simpleVars.itemsNoMalloc[0].singleQuoted == false);
        assert(strncmp(getenv("var1"), simpleVars.itemsNoMalloc[0].value, 3) == 0);
        assert(strncmp(simpleVars.itemsNoMalloc[1].key, "var2", 4) == 0);
        assert(strncmp(simpleVars.itemsNoMalloc[1].value, "two", 3) == 0);
        assert(simpleVars.itemsNoMalloc[1].singleQuoted == false);
        assert(strncmp(getenv("var2"), simpleVars.itemsNoMalloc[1].value, 3) == 0);
        assert(strncmp(simpleVars.itemsNoMalloc[2].key, "var3", 4) == 0);
        assert(strncmp(simpleVars.itemsNoMalloc[2].value, "three", 3) == 0);
        assert(simpleVars.itemsNoMalloc[2].singleQuoted == true);
        assert(strncmp(getenv("var3"), simpleVars.itemsNoMalloc[2].value, 3) == 0);
    } else {
        cdotenvVars simpleVars = {NULL, NULL, 0, 0};
        cdotenvReturn status = {CDOTENV_OK, 0};

        parseDotEnv(simpleVariables, simpleLen, &simpleVars, &status, nomalloc);
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
    }

    const char* simpleReplacement = "name=myname\nemail=${name}@email.com";
    size_t replLen = strlen(simpleReplacement);

    if (nomalloc) {
        cdotenvKVnomalloc kvs[CDOTENV_NOMALLOC_MAX_ITEMS];
        cdotenvVars replVars = {NULL, kvs, 0, CDOTENV_NOMALLOC_MAX_ITEMS};
        cdotenvReturn status2 = {CDOTENV_OK, 0};

        parseDotEnv(simpleReplacement, replLen, &replVars, &status2, nomalloc);
        assert(status2.errorCode == CDOTENV_OK);
        assert(replVars.count == 2);
        assert(strncmp(getenv("email"), "myname@email.com", 16) == 0);
        assert(strncmp(replVars.itemsNoMalloc[1].value, "myname@email.com", 16) == 0);
    } else {
        cdotenvVars replVars = {NULL, NULL, 0, 0};
        cdotenvReturn status2 = {CDOTENV_OK, 0};

        parseDotEnv(simpleReplacement, replLen, &replVars, &status2, nomalloc);
        assert(status2.errorCode == CDOTENV_OK);
        assert(replVars.count == 2);
        assert(strncmp(getenv("email"), "myname@email.com", 16) == 0);
        assert(strncmp(replVars.items[1].value, "myname@email.com", 16) == 0);

        cdotenvFree(&replVars);
        assert(replVars.items == NULL);
        assert(replVars.count == 0);
        assert(replVars.capacity == 0);
    }

    printf(nomalloc ? "Parser test with nomalloc passed\n" : "Parser test passed\n");
}

void test_parser_error(bool nomalloc) {
    const char* simpleVariables = "var1=one\nvar2=two\nvar3='three";
    size_t simpleLen = strlen(simpleVariables);

    if (nomalloc) {
        cdotenvKVnomalloc kvs[CDOTENV_NOMALLOC_MAX_ITEMS];
        cdotenvVars simpleVars = {NULL, kvs, 0, CDOTENV_NOMALLOC_MAX_ITEMS};
        cdotenvReturn status = {CDOTENV_OK, 0};

        parseDotEnv(simpleVariables, simpleLen, &simpleVars, &status, nomalloc);
        assert(status.errorCode == CDOTENV_ERROR);
        assert(simpleVars.count == 3);
        assert(strncmp(simpleVars.itemsNoMalloc[0].key, "var1", 4) == 0);
        assert(strncmp(simpleVars.itemsNoMalloc[0].value, "one", 3) == 0);
        assert(simpleVars.itemsNoMalloc[0].singleQuoted == false);
        assert(strncmp(getenv("var1"), simpleVars.itemsNoMalloc[0].value, 3) == 0);
        assert(strncmp(simpleVars.itemsNoMalloc[1].key, "var2", 4) == 0);
        assert(strncmp(simpleVars.itemsNoMalloc[1].value, "two", 3) == 0);
        assert(simpleVars.itemsNoMalloc[1].singleQuoted == false);
        assert(strncmp(getenv("var2"), simpleVars.itemsNoMalloc[1].value, 3) == 0);
        assert(strncmp(simpleVars.itemsNoMalloc[2].key, "var3", 4) == 0);
        assert(strncmp(simpleVars.itemsNoMalloc[2].value, "three", 3) == 0);
        assert(simpleVars.itemsNoMalloc[2].singleQuoted == true);
        assert(strncmp(getenv("var3"), simpleVars.itemsNoMalloc[2].value, 3) == 0);
    } else {
        cdotenvVars simpleVars = {NULL, NULL, 0, 0};
        cdotenvReturn status = {CDOTENV_OK, 0};

        parseDotEnv(simpleVariables, simpleLen, &simpleVars, &status, nomalloc);
        assert(status.errorCode == CDOTENV_ERROR);
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
    }

    const char* badName = "var1=one\nvar-2=bad";
    size_t badLen = strlen(badName);

    if (nomalloc) {
        cdotenvKVnomalloc kvs[CDOTENV_NOMALLOC_MAX_ITEMS];
        cdotenvVars badVars = {NULL, kvs, 0, CDOTENV_NOMALLOC_MAX_ITEMS};
        cdotenvReturn badNameStatus = {CDOTENV_OK, 0};

        parseDotEnv(badName, badLen, &badVars, &badNameStatus, nomalloc);
        assert(badNameStatus.errorCode == CDOTENV_ERROR);
        assert(badNameStatus.offset == 9);
        assert(badVars.count == 1);
    } else {
        cdotenvVars badVars = {NULL, NULL, 0, 0};
        cdotenvReturn badNameStatus = {CDOTENV_OK, 0};

        parseDotEnv(badName, badLen, &badVars, &badNameStatus, nomalloc);
        assert(badNameStatus.errorCode == CDOTENV_ERROR);
        assert(badNameStatus.offset == 9);
        assert(badVars.count == 1);
    }

    printf(nomalloc ? "Parser error test with nomalloc passed\n" : "Parser error test passed\n");
}

void test_parser_load(bool nomalloc) {
    cdotenvReturn status = {CDOTENV_OK, 0};
    bool loaded = loadDotEnv(".env.example", &status, nomalloc);

    assert(status.errorCode == CDOTENV_OK);
    assert(loaded == true);
    assert(strncmp(getenv("var1"), "one", 3) == 0);
    assert(strncmp(getenv("var2"), "double quoted", 13) == 0);
    assert(strncmp(getenv("var3"), "one@example", 11) == 0);
    assert(strncmp(getenv("var4"), "${var1}@example", 15) == 0);
    assert(strncmp(getenv("var5"), "one\ntwo\nthree", 13) == 0);
    printf(nomalloc ? "Parser load test with nomalloc passed\n" : "Parser load test passed\n");
}

void test_expand_nomalloc(void) {
    const char *s = "var1=\"${one}\"\nvar2=\"${two}\"";
    char t[CDOTENV_NOMALLOC_MAX_STRING];
    setenv("one", "one expansion", 1);
    setenv("two", "two expansion", 1);
    cdotenvExpandNoMalloc(s, t);

    assert(strncmp(t, "var1=\"one expansion\"\nvar2=\"two expansion\"", 41) == 0);
    printf("Expand No Malloc test passed\n");
}

int main(void) {
    test_tokenizer();
    test_tokenizer_error();
    test_parser(false);
    test_parser(true);
    test_parser_error(false);
    test_parser_error(true);
    test_parser_load(false);
    test_parser_load(true);
    test_expand_nomalloc();
    return 0;
}