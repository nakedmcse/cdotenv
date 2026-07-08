# cdotenv

![tests](https://github.com/nakedmcse/cdotenv/actions/workflows/build.yml/badge.svg)
[![GitHub issues](https://img.shields.io/github/issues/nakedmcse/cdotenv.png)](https://github.com/nakedmcse/cdotenv/issues)
[![last-commit](https://img.shields.io/github/last-commit/nakedmcse/cdotenv)](https://github.com/nakedmcse/cdotenv/commits/master)

C STB Library implementing dotenv.

## Including the library
This is written as a simple self contained STB library, so all you have to do is copy 
the `cdotenv-stb.h` file to your project and then include it using:

```c
#define CDOTENV_STB_IMPLEMENTATION
#include "cdotenv-stb.h"
```

Define `CDOTENV_STB_IMPLEMENTATION` where you want the code added, otherwise it is just the header definitions.

### No Malloc
There is a path in the code that is written to not use malloc, instead just using plain arrays.  To use this you should 
define the max size of a string and the max number of items:

```c
#define CDOTENV_STB_IMPLEMENTATION
#define CDOTENV_NOMALLOC_MAX_ITEMS 256
#define CDOTENV_NOMALLOC_MAX_STRING 1024
#include "cdotenv-stb.h"
```

If you do not define them, they will default to 256 for the max number of items and 1024 for max string size.

### Testing
The repository itself comes with a set of unit tests in the `cdotenv-stb-test.c` file, that can be built and run using 
the following:
```shell
make all
./cdotenv-stb-test
```

## Parsing
The library can parse either standard `.env` formatted files or strings containing `.env` formatted lines.

### Files
To parse a given `.env` file to environment variables, simply setup a status variable and pass the filename
to the `loadDotEnv` function:

```c
cdotenvReturn status = {CDOTENV_OK, 0};

if (!loadDotEnv(".env.example", &status, false)) {
    printf("Failed to load file\n");
}

if(status.errorCode != CDOTENV_OK) {
    printf("Error parsing at %lu offset\n",status.offset);
}
```

For a no malloc file parse:
```c
cdotenvReturn status = {CDOTENV_OK, 0};

if (!loadDotEnv(".env.example", &status, true)) {
    printf("Failed to load file\n");
}

if(status.errorCode != CDOTENV_OK) {
    printf("Error parsing at %lu offset\n",status.offset);
}
```

### Strings
To parse a given string containing `.env` formatted lines to environment variables, you need to setup a variable array 
and status variable, then pass the string to the `parseDotEnv` function:

```c
const char* simpleVariables = "var1=one\nvar2=two\nvar3='three'";
size_t simpleLen = strlen(simpleVariables);
cdotenvVars simpleVars = {NULL, NULL, 0, 0};
cdotenvReturn status = {CDOTENV_OK, 0};

parseDotEnv(simpleVariables, simpleLen, &simpleVars, &status, false);

if(status.errorCode != CDOTENV_OK) {
    printf("Error parsing at %lu offset\n",status.offset);
}
cdotenvFree(&simpleVars);
```

For a no malloc string parse:
```c
const char* simpleVariables = "var1=one\nvar2=two\nvar3='three'";
size_t simpleLen = strlen(simpleVariables);
cdotenvKVnomalloc kvs[CDOTENV_NOMALLOC_MAX_ITEMS];
cdotenvVars simpleVars = {NULL, kvs, 0, CDOTENV_NOMALLOC_MAX_ITEMS};
cdotenvReturn status = {CDOTENV_OK, 0};

parseDotEnv(simpleVariables, simpleLen, &simpleVars, &status, true);

if(status.errorCode != CDOTENV_OK) {
    printf("Error parsing at %lu offset\n",status.offset);
}
cdotenvFree(&simpleVars);
```

### Errors
The status variable is used to return if there is an error in the parse of either a file or a string, and at which 
offset into the file or string that it occurred:

```c
cdotenvReturn status = {CDOTENV_OK, 0};

// Parse the file or string here

if(status.errorCode != CDOTENV_OK) {
    printf("Error parsing at %lu offset\n",status.offset);
}
```