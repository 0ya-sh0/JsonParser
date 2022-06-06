# JsonParser
A basic JSON Parser written in C Lang

# For using in the project

Add parser.h and parser.c in the project

```c
#include "parser.h"

int main() {
    char *string; // JSON string to be parsed
    parser p;
    initParser(&p, string);
    JsonValue *value = parseJson(&p);
    if (value == NULL)
    {
        printParseError(&p);
    }
    else
    {
        printJsonValue(value, 0);
        freeJsonValue(value);
    }
    return 0;
}
```

# For just trying the demo
run `make all`

run `./bin/demo ./inputs/t0.json`
