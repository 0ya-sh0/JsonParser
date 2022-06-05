#include "parser.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
    if (argc < 2)
        return 0;
    const char *fname = argv[1];

    FILE *f = fopen(fname, "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET); /* same as rewind(f); */

    char *string = malloc(fsize + 1);
    fread(string, fsize, 1, f);
    fclose(f);

    string[fsize] = 0;
    parser p = {.text = string, .length = strlen(string), .pos = 0};

    JsonValue *value = parseJson(&p);
    printJsonValue(value, 0);
    return 0;
}
