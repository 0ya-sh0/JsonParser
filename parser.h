#include <stdlib.h>

// struct parser;
// struct JsonArray;
// struct JsonObject;
// struct JsonObjectEntry;
// struct JsonValue;
// struct JsonNull;
// struct JsonBool;
// struct JsonNumber;
// struct JsonString;

enum JsonValueType
{
    VALUE_NULL,
    VALUE_BOOL,
    VALUE_NUMBER,
    VALUE_STRING,
    VALUE_OBJECT,
    VALUE_ARRAY
};

typedef struct parser
{
    char *text;
    size_t length;
    size_t pos;
} parser;

typedef struct JsonString
{
    char *val;
} JsonString;

typedef struct JsonValue
{
    enum JsonValueType type;
    void *val;
} JsonValue;

typedef struct JsonArray
{
    size_t length;
    JsonValue **val;
} JsonArray;

typedef struct JsonObjectEntry
{
    JsonString *key;
    JsonValue *val;
} JsonObjectEntry;

typedef struct JsonObject
{
    size_t length;
    JsonObjectEntry **val;
} JsonObject;

typedef struct JsonBool
{
    int val;
} JsonBool;

typedef struct JsonNumber
{
    char *val;
} JsonNumber;

void printParser(const parser *p);

void parseWhiteSpace(parser *p);
int parseNull(parser *p);
JsonBool *parseBool(parser *p);
JsonNumber *parseNumber(parser *p);
JsonString *parseString(parser *p);
JsonArray *parseArray(parser *p);
JsonObject *parseObject(parser *p);
JsonObjectEntry *parseObjectEntry(parser *p);
JsonValue *parseJson(parser *p);
void printIndent(size_t count);
void printJsonValue(const JsonValue *value, size_t indent);