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

    if (!f)
    {
        perror("fopen:");
        exit(0);
    }

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *string = malloc(fsize + 1);

    if (!string)
    {
        perror("malloc:");
        exit(0);
    }
    fread(string, fsize, 1, f);
    fclose(f);

    string[fsize] = 0;
    parser p;
    initParser(&p, string);

    JsonValue *value = parseJson(&p);
    if (value == NULL)
    {
        printParseError(&p);
        return 0;
    }
    printJsonValue(value, 0);
    freeJsonValue(value);
    return 0;
}
