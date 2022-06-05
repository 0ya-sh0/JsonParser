#include "parser.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void test(int expr)
{
    if (expr)
        printf("pass\n");
    else
        printf("fail\n");
}

void testWhiteSpace()
{
    printf("Test White Space\n");
    parser p;
    char *buff1 = "ab";
    p.length = strlen(buff1);
    p.text = buff1;
    p.pos = 0;
    // printParser(&p);
    parseWhiteSpace(&p);
    if (p.pos == 0)
        printf("pass\n");
    else
        printf("fail\n");
    // printParser(&p);

    char *buff2 = "  ab  ";
    p.length = strlen(buff2);
    p.text = buff2;
    p.pos = 0;
    // printParser(&p);
    parseWhiteSpace(&p);
    if (p.pos == 2)
        printf("pass\n");
    else
        printf("fail\n");
    // printParser(&p);
}

void testNull()
{
    printf("Test Null\n");
    parser p;
    char *buff1 = "ab";
    p.length = strlen(buff1);
    p.text = buff1;
    p.pos = 0;
    // printParser(&p);
    if (parseNull(&p) == 0 && p.pos == 0)
        printf("pass\n");
    else
        printf("fail\n");
    // printParser(&p);

    char *buff2 = "null  ab  ";
    p.length = strlen(buff2);
    p.text = buff2;
    p.pos = 0;
    // printParser(&p);
    if (parseNull(&p) == 1 && p.pos == 4)
        printf("pass\n");
    else
        printf("fail\n");
    // printParser(&p);

    if (parseNull(&p) == 0 && p.pos == 4)
        printf("pass\n");
    else
        printf("fail\n");
    // printParser(&p);
}

void testBool()
{
    printf("Test Bool\n");
    parser p;
    JsonBool *r;
    char *buff1 = "ab";
    p.length = strlen(buff1);
    p.text = buff1;
    p.pos = 0;
    // printParser(&p);
    r = parseBool(&p);
    if (r == NULL)
        printf("pass\n");
    else
        printf("fail\n");
    // printParser(&p);

    char *buff2 = "true  ab  ";
    p.length = strlen(buff2);
    p.text = buff2;
    p.pos = 0;
    // printParser(&p);
    r = parseBool(&p);
    if (r->val == 1)
        printf("pass\n");
    else
        printf("fail\n");
    // printParser(&p);

    char *buff3 = "false  ab  ";
    p.length = strlen(buff3);
    p.text = buff3;
    p.pos = 0;
    // printParser(&p);
    r = parseBool(&p);
    if (r->val == 0)
        printf("pass\n");
    else
        printf("fail\n");
    // printParser(&p);
}

void testNumber()
{
    printf("Test Number\n");
    JsonNumber *r;
    parser p;

    char *buff1 = "abcd";
    p.text = buff1;
    p.pos = 0;
    p.length = strlen(buff1);
    r = parseNumber(&p);
    test(r == NULL && p.pos == 0);

    char *buff2 = "1234abcd";
    p.text = buff2;
    p.pos = 0;
    p.length = strlen(buff2);
    r = parseNumber(&p);
    test(r != NULL && p.pos == 4 && r->val == 1234);
}

void testString()
{
    printf("Test String\n");
    JsonString *r;
    parser p;

    char *buff1 = "abcd";
    p.text = buff1;
    p.pos = 0;
    p.length = strlen(buff1);
    r = parseString(&p);
    test(r == NULL && p.pos == 0);

    char *buff2 = "\"1234\"abcd";
    p.text = buff2;
    p.pos = 0;
    p.length = strlen(buff2);
    r = parseString(&p);
    test(r != NULL && p.pos == 6 && strcmp(r->val, "1234") == 0);
}

void testArray()
{
    printf("Test Array\n");
    JsonArray *r;
    parser p;
    char *buff;

    buff = "abcd";
    p.text = buff;
    p.pos = 0;
    p.length = strlen(buff);
    r = parseArray(&p);
    test(r == NULL && p.pos == 0);

    buff = "[]";
    p.text = buff;
    p.pos = 0;
    p.length = strlen(buff);
    r = parseArray(&p);
    test(r != NULL && p.pos == p.length && r->length == 0);

    buff = "[    ]";
    p.text = buff;
    p.pos = 0;
    p.length = strlen(buff);
    r = parseArray(&p);
    test(r != NULL && p.pos == p.length && r->length == 0);

    buff = "[ 1234 ]";
    p.text = buff;
    p.pos = 0;
    p.length = strlen(buff);
    r = parseArray(&p);
    test(r != NULL && p.pos == p.length && r->length == 1 && r->val[0]->type == VALUE_NUMBER);

    buff = "[ 1234  , \"abcd\" ]";
    p.text = buff;
    p.pos = 0;
    p.length = strlen(buff);
    r = parseArray(&p);
    test(r != NULL && p.pos == p.length && r->length == 2 && r->val[0]->type == VALUE_NUMBER && r->val[1]->type == VALUE_STRING);

    buff = "[ 1234, 5677, true]";
    p.text = buff;
    p.pos = 0;
    p.length = strlen(buff);
    r = parseArray(&p);
    test(r != NULL && p.pos == p.length && r->length == 3 && r->val[0]->type == VALUE_NUMBER && r->val[1]->type == VALUE_NUMBER && r->val[2]->type == VALUE_BOOL);
}

void testObjectEntry()
{
    printf("Test Object Entry\n");
    JsonObjectEntry *r;
    parser p;
    char *buff;

    buff = "abcd";
    p.text = buff;
    p.pos = 0;
    p.length = strlen(buff);
    r = parseObjectEntry(&p);
    test(r == NULL && p.pos == 0);

    buff = "\"name\"   :     [ 1234, 5677, true]";
    p.text = buff;
    p.pos = 0;
    p.length = strlen(buff);
    r = parseObjectEntry(&p);
    test(r != NULL && p.pos == p.length);
}

void testObject()
{
    printf("Test Object\n");
    JsonObject *r;
    parser p;
    char *buff;

    buff = "abcd";
    p.text = buff;
    p.pos = 0;
    p.length = strlen(buff);
    r = parseObject(&p);
    test(r == NULL && p.pos == 0);

    buff = "{}";
    p.text = buff;
    p.pos = 0;
    p.length = strlen(buff);
    r = parseObject(&p);
    test(r != NULL && p.pos == p.length && r->length == 0);

    buff = "{   }";
    p.text = buff;
    p.pos = 0;
    p.length = strlen(buff);
    r = parseObject(&p);
    test(r != NULL && p.pos == p.length && r->length == 0);

    buff = "{  \"name\" : \"foobarbaz\"  }";
    p.text = buff;
    p.pos = 0;
    p.length = strlen(buff);
    r = parseObject(&p);
    test(r != NULL && p.pos == p.length && r->length == 1 && strcmp(r->val[0]->key->val, "name") == 0 && r->val[0]->val->type == VALUE_STRING);

    buff = "{  \"name\" : \"foobarbaz\", \"hobbies\" : [1234]  }";
    p.text = buff;
    p.pos = 0;
    p.length = strlen(buff);
    r = parseObject(&p);
    test(r != NULL && p.pos == p.length && r->length == 2 && strcmp(r->val[1]->key->val, "hobbies") == 0 && r->val[1]->val->type == VALUE_ARRAY);
}

void testValue()
{
    printf("Test Value\n");
    JsonValue *r;
    parser p;
    char *buff;

    buff = "null";
    p.text = buff;
    p.pos = 0;
    p.length = strlen(buff);
    r = parseJson(&p);
    test(r != NULL && p.pos == p.length && r->type == VALUE_NULL);

    buff = "123466";
    p.text = buff;
    p.pos = 0;
    p.length = strlen(buff);
    r = parseJson(&p);
    test(r != NULL && p.pos == p.length && r->type == VALUE_NUMBER);

    buff = "\"foobarbaz\"";
    p.text = buff;
    p.pos = 0;
    p.length = strlen(buff);
    r = parseJson(&p);
    test(r != NULL && p.pos == p.length && r->type == VALUE_STRING);

    buff = "true";
    p.text = buff;
    p.pos = 0;
    p.length = strlen(buff);
    r = parseJson(&p);
    test(r != NULL && p.pos == p.length && r->type == VALUE_BOOL);

    buff = "false";
    p.text = buff;
    p.pos = 0;
    p.length = strlen(buff);
    r = parseJson(&p);
    test(r != NULL && p.pos == p.length && r->type == VALUE_BOOL);

    buff = "[ 1234  , \"foobarbaz\" ]";
    p.text = buff;
    p.pos = 0;
    p.length = strlen(buff);
    r = parseJson(&p);
    test(r != NULL && p.pos == p.length && r->type == VALUE_ARRAY);

    buff = "{  \"name\" : \"foobarbaz\", \"hobbies\" : [1234]  }";
    p.text = buff;
    p.pos = 0;
    p.length = strlen(buff);
    r = parseJson(&p);
    test(r != NULL && p.pos == p.length && r->type == VALUE_OBJECT);

    buff = "aaa";
    p.text = buff;
    p.pos = 0;
    p.length = strlen(buff);
    r = parseJson(&p);
    test(r == NULL && p.pos == 0);
}

int main(int argc, char const *argv[])
{
    testWhiteSpace();
    testNull();
    testBool();
    testNumber();
    testString();
    testArray();
    testObjectEntry();
    testObject();
    testValue();

    return 0;
}
