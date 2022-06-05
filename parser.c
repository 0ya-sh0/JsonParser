#include "parser.h"
#include <string.h>
#include <stdio.h>

void printParser(const parser *p)
{
    printf("Parser Pos: %lu\n", p->pos);
    printf("Parser Text: \n'%s'\n", p->text + p->pos);
}

void printIndent(size_t count)
{
    for (size_t i = 0; i < count; i++)
    {
        printf(" ");
    }
}

void printJsonValue(const JsonValue *value, size_t indent)
{
    JsonObject *object;
    JsonObjectEntry **objectEntries;
    JsonArray *array;
    JsonValue **arrayValues;

    if (!value)
        return;
    switch (value->type)
    {
    case VALUE_NULL:
        printIndent(indent);
        printf("(NULL)");
        return;

    case VALUE_BOOL:
        printIndent(indent);
        printf("(BOOL: %s)", ((JsonBool *)value->val)->val ? "true" : "false");
        return;

    case VALUE_NUMBER:
        printIndent(indent);
        printf("(NUMBER: %u)", ((JsonNumber *)value->val)->val);
        return;

    case VALUE_STRING:
        printIndent(indent);
        printf("(STRING: '%s')", ((JsonString *)value->val)->val);
        return;

    case VALUE_OBJECT:
        object = (JsonObject *)value->val;
        printIndent(indent);
        printf("(OBJECT: %lu)\n", object->length);
        objectEntries = object->val;
        for (size_t i = 0; i < object->length; i++)
        {
            printIndent(indent + 4);
            printf("`%s`\n", objectEntries[i]->key->val);
            printJsonValue(objectEntries[i]->val, indent + 8);
            printf("\n");
        }
        return;

    case VALUE_ARRAY:
        array = (JsonArray *)value->val;
        arrayValues = array->val;
        printIndent(indent);
        printf("(ARRAY: %lu)\n", array->length);
        for (size_t i = 0; i < array->length; i++)
        {
            printJsonValue(arrayValues[i], indent + 4);
            printf("\n");
        }
        return;

    default:
        perror("unknown value type");
        exit(0);
        break;
    }
}

void parseWhiteSpace(parser *p)
{
    while (p->pos < p->length)
    {
        char c = p->text[p->pos];
        if ((c == ' ') || (c == '\t') || (c == '\n') || (c == '\r'))
            p->pos++;
        else
            return;
    }
}

int parseNull(parser *p)
{
    if (strncmp("null", p->text + p->pos, 4) == 0)
    {
        p->pos += 4;
        return 1;
    }
    return 0;
}

JsonBool *parseBool(parser *p)
{
    if (strncmp("true", p->text + p->pos, 4) == 0)
    {
        p->pos += 4;
        JsonBool *result = (JsonBool *)malloc(sizeof(JsonBool));
        result->val = 1;
        return result;
    }
    if (strncmp("false", p->text + p->pos, 5) == 0)
    {
        p->pos += 5;
        JsonBool *result = (JsonBool *)malloc(sizeof(JsonBool));
        result->val = 0;
        return result;
    }
    return NULL;
}

JsonNumber *parseNumber(parser *p)
{
    size_t count = 0;
    size_t pos = p->pos;
    while (pos < p->length)
    {
        char c = p->text[pos];
        if (c >= '0' && c <= '9')
        {
            count++;
            pos++;
        }
        else
        {
            break;
        }
    }
    if (count == 0)
        return NULL;
    JsonNumber *result = (JsonNumber *)malloc(sizeof(JsonNumber));
    result->val = 0;
    while (p->pos < pos)
    {
        result->val *= 10;
        result->val += p->text[p->pos] - '0';
        p->pos++;
    }
    return result;
}

JsonString *parseString(parser *p)
{
    if (p->pos + 2 > p->length)
        return NULL;
    if (p->text[p->pos] != '"')
        return NULL;

    size_t count = 0;
    size_t pos = p->pos + 1;
    while (pos < p->length)
    {
        char c = p->text[pos];
        if (c == '"')
        {
            pos++;
            break;
        }
        if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
        {
            count++;
            pos++;
        }
    }
    JsonString *result = (JsonString *)malloc(sizeof(JsonString));
    result->val = (char *)malloc(count + 1);
    p->pos++;
    for (size_t i = 0; i < count; i++)
    {
        result->val[i] = p->text[p->pos];
        p->pos++;
    }
    result->val[count] = '\0';
    p->pos++;
    return result;
}

JsonValue *parseJson(parser *p)
{
    JsonValue *result = NULL;

    if (parseNull(p))
    {
        result = (JsonValue *)malloc(sizeof(JsonValue));
        result->type = VALUE_NULL;
        result->val = NULL;
        return result;
    }

    JsonBool *bool;
    if ((bool = parseBool(p)) != NULL)
    {
        result = (JsonValue *)malloc(sizeof(JsonValue));
        result->type = VALUE_BOOL;
        result->val = bool;
        return result;
    }

    JsonNumber *number;
    if ((number = parseNumber(p)) != NULL)
    {
        result = (JsonValue *)malloc(sizeof(JsonValue));
        result->type = VALUE_NUMBER;
        result->val = number;
        return result;
    }

    JsonString *str;
    if ((str = parseString(p)) != NULL)
    {
        result = (JsonValue *)malloc(sizeof(JsonValue));
        result->type = VALUE_STRING;
        result->val = str;
        return result;
    }

    JsonArray *arr;
    if ((arr = parseArray(p)) != NULL)
    {
        result = (JsonValue *)malloc(sizeof(JsonValue));
        result->type = VALUE_ARRAY;
        result->val = arr;
        return result;
    }

    JsonObject *obj;
    if ((obj = parseObject(p)) != NULL)
    {
        result = (JsonValue *)malloc(sizeof(JsonValue));
        result->type = VALUE_OBJECT;
        result->val = obj;
        return result;
    }

    return NULL;
}

JsonArray *parseArray(parser *p)
{
    JsonValue *values[1024], *val;
    size_t valuesCount = 0;
    size_t prevPos = p->pos;

    if (p->text[p->pos] != '[')
    {
        // err
        p->pos = prevPos;
        return NULL;
    }
    p->pos++;
    parseWhiteSpace(p);

    // empty array
    if (p->text[p->pos] == ']')
    {
        p->pos++;
        JsonArray *result = (JsonArray *)malloc(sizeof(JsonArray));
        result->length = 0;
        result->val = NULL;
        return result;
    }

    if ((val = parseJson(p)) == NULL)
    {
        // err
        p->pos = prevPos;
        return NULL;
    }
    values[valuesCount] = val;
    valuesCount++;

    while (p->pos < p->length)
    {
        parseWhiteSpace(p);

        if (p->text[p->pos] == ']')
        {
            p->pos++;
            JsonArray *result = (JsonArray *)malloc(sizeof(JsonArray));
            result->length = valuesCount;
            result->val = (JsonValue **)malloc(sizeof(JsonValue *) * valuesCount);

            for (size_t i = 0; i < valuesCount; i++)
            {
                result->val[i] = values[i];
            }
            return result;
        }

        if (p->text[p->pos] != ',')
        {
            // err
            p->pos = prevPos;
            return NULL;
        }
        p->pos++;
        parseWhiteSpace(p);

        if ((val = parseJson(p)) == NULL)
        {
            // err
            p->pos = prevPos;
            return NULL;
        }
        values[valuesCount] = val;
        valuesCount++;
    }
    // free stuff
    p->pos = prevPos;
    return NULL;
}

JsonObjectEntry *parseObjectEntry(parser *p)
{
    size_t prevPos = p->pos;
    JsonString *key;
    JsonValue *val;

    if ((key = parseString(p)) == NULL)
    {
        p->pos = prevPos;
        return NULL;
    }

    parseWhiteSpace(p);
    if (p->text[p->pos] != ':')
    {
        p->pos = prevPos;
        return NULL;
    }
    p->pos++;
    parseWhiteSpace(p);

    if ((val = parseJson(p)) == NULL)
    {
        p->pos = prevPos;
        return NULL;
    }

    JsonObjectEntry *result = (JsonObjectEntry *)malloc(sizeof(JsonObjectEntry));
    result->key = key;
    result->val = val;
    return result;
}

JsonObject *parseObject(parser *p)
{
    JsonObjectEntry *values[1024], *val;
    size_t valuesCount = 0;
    size_t prevPos = p->pos;

    if (p->text[p->pos] != '{')
    {
        // err
        p->pos = prevPos;
        return NULL;
    }
    p->pos++;
    parseWhiteSpace(p);

    // empty array
    if (p->text[p->pos] == '}')
    {
        p->pos++;
        JsonObject *result = (JsonObject *)malloc(sizeof(JsonObject));
        result->length = 0;
        result->val = NULL;
        return result;
    }

    if ((val = parseObjectEntry(p)) == NULL)
    {
        // err
        p->pos = prevPos;
        return NULL;
    }
    values[valuesCount] = val;
    valuesCount++;

    while (p->pos < p->length)
    {
        parseWhiteSpace(p);

        if (p->text[p->pos] == '}')
        {
            p->pos++;
            JsonObject *result = (JsonObject *)malloc(sizeof(JsonObject));
            result->length = valuesCount;
            result->val = (JsonObjectEntry **)malloc(sizeof(JsonObjectEntry *) * valuesCount);

            for (size_t i = 0; i < valuesCount; i++)
            {
                result->val[i] = values[i];
            }
            return result;
        }

        if (p->text[p->pos] != ',')
        {
            // err
            p->pos = prevPos;
            return NULL;
        }
        p->pos++;
        parseWhiteSpace(p);

        if ((val = parseObjectEntry(p)) == NULL)
        {
            // err
            p->pos = prevPos;
            return NULL;
        }
        values[valuesCount] = val;
        valuesCount++;
    }
    // free stuff
    p->pos = prevPos;
    return NULL;
}