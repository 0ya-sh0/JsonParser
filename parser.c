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
        printf("(NUMBER: %s)", ((JsonNumber *)value->val)->val);
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

    int flag_has_sign = 0;
    if (p->text[pos] == '-')
    {
        flag_has_sign = 1;
        count++;
        pos++;
    }

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
    {
        return NULL;
    }
    if (count == 1 && flag_has_sign)
    {
        p->pos = pos;
        p->error = PERR_NUM_EXPECTED_DIGIT;
        return NULL;
    }
    size_t count_after_dot = 0;
    if (p->text[pos] == '.')
    {
        count++;
        pos++;
        while (pos < p->length)
        {
            char c = p->text[pos];
            if (c >= '0' && c <= '9')
            {
                count++;
                pos++;
                count_after_dot++;
            }
            else
            {
                break;
            }
        }
        if (count_after_dot == 0)
        {
            p->pos = pos;
            p->error = PERR_NUM_EXPECTED_DIGIT;
            return NULL;
        }
    }
    JsonNumber *result = (JsonNumber *)malloc(sizeof(JsonNumber));
    result->val = (char *)malloc(count + 1);
    for (size_t i = 0; i < count; i++)
    {
        result->val[i] = p->text[p->pos];
        p->pos++;
    }
    result->val[count] = '\0';
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
    int success = 0;
    while (pos < p->length)
    {
        char c = p->text[pos];
        if (c == '"' && p->text[pos - 1] != '\\')
        {
            success = 1;
            pos++;
            break;
        }
        count++;
        pos++;
    }

    if (!success)
    {
        p->error = PERR_STRING_NO_END;
        return NULL;
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
    if (p->error == PERR_NO_ERROR)
    {
        p->error = PERR_UNKNOWN_VALUE;
    }
    return NULL;
}

JsonArray *parseArray(parser *p)
{
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

    JsonValue **values, *val;
    if ((val = parseJson(p)) == NULL)
    {
        // err
        return NULL;
    }
    size_t valuesCount = 0, valuesCap = 1024;
    values = (JsonValue **)calloc(valuesCap, sizeof(JsonValue *));
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
            free(values);
            return result;
        }

        if (p->text[p->pos] != ',')
        {
            for (size_t i = 0; i < valuesCount; i++)
            {
                freeJsonValue(values[i]);
            }
            free(values);
            p->error = PERR_ARRAY_EXPECTED_COMMA;
            return NULL;
        }
        p->pos++;
        parseWhiteSpace(p);

        if ((val = parseJson(p)) == NULL)
        {
            for (size_t i = 0; i < valuesCount; i++)
            {
                freeJsonValue(values[i]);
            }
            free(values);
            return NULL;
        }
        if (valuesCount >= valuesCap)
        {
            values = realloc(values, valuesCap + 1024);
            valuesCap += 1024;
        }
        values[valuesCount] = val;
        valuesCount++;
    }
    for (size_t i = 0; i < valuesCount; i++)
    {
        freeJsonValue(values[i]);
    }
    free(values);
    p->pos = prevPos;
    p->error = PERR_ARRAY_NO_END;
    return NULL;
}

JsonObjectEntry *parseObjectEntry(parser *p)
{
    JsonString *key;
    JsonValue *val;

    if ((key = parseString(p)) == NULL)
    {
        return NULL;
    }

    parseWhiteSpace(p);
    if (p->text[p->pos] != ':')
    {
        freeJsonString(key);
        p->error = PERR_OBJECT_EXPECETD_COLON;
        return NULL;
    }
    p->pos++;
    parseWhiteSpace(p);

    if ((val = parseJson(p)) == NULL)
    {
        freeJsonString(key);
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
            for (size_t i = 0; i < valuesCount; i++)
            {
                freeJsonObjectEntry(values[i]);
            }
            p->error = PERR_OBJECT_EXPECTED_COMMA;
            return NULL;
        }
        p->pos++;
        parseWhiteSpace(p);

        if ((val = parseObjectEntry(p)) == NULL)
        {
            for (size_t i = 0; i < valuesCount; i++)
            {
                freeJsonObjectEntry(values[i]);
            }
            return NULL;
        }
        values[valuesCount] = val;
        valuesCount++;
    }
    for (size_t i = 0; i < valuesCount; i++)
    {
        freeJsonObjectEntry(values[i]);
    }
    p->pos = prevPos;
    p->error = PERR_OBJECT_NO_END;
    return NULL;
}

void initParser(parser *p, char *str)
{
    p->error = 0;
    p->text = str;
    p->length = strlen(str);
    p->pos = 0;
}

char *parserErrMessages[] = {
    "No error",
    "Json Object did not end, expected '}'",
    "Json Object expected ',' before",
    "Json Object entry expected ':' before",
    "Json Array did not end, expected ']'",
    "Json Array expected ','",
    "Json String did not end, expected '\"'",
    "Json Unknown value",
    "Json Number expected digit '[0-9]'"};

void printParseError(parser *p)
{
    char buff[60];
    printf("ERROR: \t%s\n", parserErrMessages[p->error]);
    if (p->error == PERR_NO_ERROR)
        return;

    size_t line = 1, col = 1;
    for (size_t i = 0; i < p->pos; i++)
    {
        char c = p->text[i];
        if (c == '\n')
        {
            line++;
            col = 1;
            continue;
        }
        col++;
    }

    printf("\tat line %lu, col %lu\n", line, col);
    size_t indent = 0;
    if (col < 55)
    {
        strncpy(buff, p->text + p->pos - col, 60);
        indent = col;
    }
    else
    {
        strncpy(buff, p->text + p->pos - 5, 60);
        indent = 5;
    }
    printf("\t");
    size_t count = strlen(buff);
    for (size_t i = 0; i < count; i++)
    {
        char c = buff[i];
        switch (c)
        {
        case '\n':
            printf("\\n");
            break;
        case '\r':
            printf("\\r");
            break;
        case '\t':
            printf("\\t");
            break;
        default:
            printf("%c", c);
            break;
        }
    }
    printf("\n\t");
    for (size_t i = 0; i < indent; i++)
    {
        printf(" ");
    }
    printf("^\n");
}

void freeJsonString(JsonString *value)
{
    free(value->val);
    free(value);
}

void freeJsonBool(JsonBool *value)
{
    free(value);
}

void freeJsonNumber(JsonNumber *value)
{
    free(value->val);
    free(value);
}

void freeJsonArray(JsonArray *value)
{
    for (size_t i = 0; i < value->length; i++)
    {
        freeJsonValue(value->val[i]);
    }
    free(value->val);
    free(value);
}

void freeJsonObject(JsonObject *value)
{

    for (size_t i = 0; i < value->length; i++)
    {
        freeJsonObjectEntry(value->val[i]);
    }
    free(value->val);
    free(value);
}

void freeJsonObjectEntry(JsonObjectEntry *value)
{
    freeJsonString(value->key);
    freeJsonValue(value->val);
    free(value);
}

void freeJsonValue(JsonValue *value)
{
    switch (value->type)
    {
    case VALUE_NULL:
        break;
    case VALUE_BOOL:
        freeJsonBool((JsonBool *)value->val);
        break;
    case VALUE_NUMBER:
        freeJsonNumber((JsonNumber *)value->val);
        break;
    case VALUE_STRING:
        freeJsonString((JsonString *)value->val);
        break;
    case VALUE_OBJECT:
        freeJsonObject((JsonObject *)value->val);
        break;
    case VALUE_ARRAY:
        freeJsonArray((JsonArray *)value->val);
        break;
    }
    free(value);
}
