#ifndef PARSER_H_
#define PARSER_H_

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
    int error;
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
void printParseError(parser *p);
void initParser(parser *p, char *str);

void freeJsonString(JsonString *value);
void freeJsonBool(JsonBool *value);
void freeJsonNumber(JsonNumber *value);
void freeJsonArray(JsonArray *value);
void freeJsonObject(JsonObject *value);
void freeJsonObjectEntry(JsonObjectEntry *value);
void freeJsonValue(JsonValue *value);

#define PERR_NO_ERROR 0
#define PERR_OBJECT_NO_END 1
#define PERR_OBJECT_EXPECTED_COMMA 2
#define PERR_OBJECT_EXPECETD_COLON 3
#define PERR_ARRAY_NO_END 4
#define PERR_ARRAY_EXPECTED_COMMA 5
#define PERR_STRING_NO_END 6
#define PERR_UNKNOWN_VALUE 7
#define PERR_NUM_EXPECTED_DIGIT 8

extern char *parserErrMessages[];
#endif