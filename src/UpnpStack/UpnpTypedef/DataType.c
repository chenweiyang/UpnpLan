/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   DataType.c
*
* @remark
*
*/

#include "DataType.h"
#include "tiny_log.h"
#include "tiny_str_equal.h"

#define TAG                     "ObjectType"

#define	TYPE_UI1                "ui1"
#define	TYPE_UI2                "ui2"
#define	TYPE_UI4                "ui4"
#define	TYPE_I1                 "i1"
#define	TYPE_I2                 "i2"
#define	TYPE_I4                 "i4"
#define	TYPE_INT                "int"
#define	TYPE_R4                 "r4"
#define	TYPE_R8                 "r8"
#define	TYPE_NUMBER             "number"
#define	TYPE_FIXED_14_4         "fixed.14.4"
#define	TYPE_FLOAT              "float"
#define	TYPE_CHAR               "char"
#define	TYPE_STRING             "string"
#define	TYPE_DATE               "date"
#define	TYPE_DATETIME           "dateTime"
#define	TYPE_DATETIME_TZ        "dateTime.tz"
#define	TYPE_TIME               "time"
#define	TYPE_TIME_TZ            "time.tz"
#define	TYPE_BOOLEAN            "boolean"
#define	TYPE_BIN_BASE64         "bin.base64"
#define	TYPE_BIN_HEX            "bin.hex"
#define	TYPE_URI                "uri"
#define	TYPE_UUID               "uuid"


/**
*  Byte                 i1
*  Word                 ui1, i2
*  Integer              ui2, i4, int
*  Long                 ui4, time, time.tz
*  Float                r4, float
*  Double               r8, number, fixed.14.4
*  Character            char
*  String               string, uri, uuid, bin.base64, bin.hex, date, dateTime, dateTime.tz
*  Boolean              boolean
*/
static InternalType InternalType_RetrieveType(const char *typeName)
{
    InternalType t = INTERNAL_INTEGER;

    do
    {
        if (STR_EQUAL(typeName, TYPE_I1))
        {
            t = INTERNAL_BYTE;
            break;
        }

        if (STR_EQUAL(typeName, TYPE_UI1) || STR_EQUAL(typeName, TYPE_I2))
        {
            t = INTERNAL_WORD;
            break;
        }

        if (STR_EQUAL(typeName, TYPE_UI2) || STR_EQUAL(typeName, TYPE_I4) || STR_EQUAL(typeName, TYPE_INT))
        {
            t = INTERNAL_INTEGER;
            break;
        }

        if (STR_EQUAL(typeName, TYPE_UI4) || STR_EQUAL(typeName, TYPE_TIME) || STR_EQUAL(typeName, TYPE_TIME_TZ))
        {
            t = INTERNAL_LONG;
            break;
        }

        if (STR_EQUAL(typeName, TYPE_R4) || STR_EQUAL(typeName, TYPE_FLOAT))
        {
            t = INTERNAL_FLOAT;
            break;
        }

        if (STR_EQUAL(typeName, TYPE_R8) || STR_EQUAL(typeName, TYPE_NUMBER) || STR_EQUAL(typeName, TYPE_FIXED_14_4))
        {
            t = INTERNAL_DOUBLE;
            break;
        }

        if (STR_EQUAL(typeName, TYPE_CHAR))
        {
            t = INTERNAL_CHAR;
            break;
        }

        if (STR_EQUAL(typeName, TYPE_BOOLEAN))
        {
            t = INTERNAL_BOOLEAN;
            break;
        }

        if (STR_EQUAL(typeName, TYPE_STRING)
            || STR_EQUAL(typeName, TYPE_URI)
            || STR_EQUAL(typeName, TYPE_UUID)
            || STR_EQUAL(typeName, TYPE_BIN_BASE64)
            || STR_EQUAL(typeName, TYPE_BIN_HEX)
            || STR_EQUAL(typeName, TYPE_DATE)
            || STR_EQUAL(typeName, TYPE_DATETIME)
            || STR_EQUAL(typeName, TYPE_DATETIME_TZ))
        {
            t = INTERNAL_STRING;
            break;
        }

    } while (0);

    return t;
}

static const char * InternalType_GetName(InternalType type)
{
    const char * t = NULL;

    switch (type)
    {
    case INTERNAL_BYTE:
        t = TYPE_CHAR;
        break;

    case INTERNAL_WORD:
        t = TYPE_I2;
        break;

    case INTERNAL_INTEGER:
        t = TYPE_I4;
        break;

    case INTERNAL_LONG:
        t = TYPE_UI4;
        break;

    case INTERNAL_FLOAT:
        t = TYPE_FLOAT;
        break;

    case INTERNAL_DOUBLE:
        t = TYPE_NUMBER;
        break;

    case INTERNAL_BOOLEAN:
        t = TYPE_BOOLEAN;
        break;

    case INTERNAL_CHAR:
        t = TYPE_CHAR;
        break;

    case INTERNAL_STRING:
        t = TYPE_STRING;
        break;

    default:
        break;
    }

    return t;
}

void DataType_Construct(DataType *thiz)
{
    RETURN_IF_FAIL(thiz);

    memset(thiz, 0, sizeof(DataType));
}

void DataType_Dispose(DataType *thiz)
{
    RETURN_IF_FAIL(thiz);

    memset(thiz, 0, sizeof(DataType));
}

void DataType_Copy(DataType *dst, DataType *src)
{
    RETURN_IF_FAIL(dst);
    RETURN_IF_FAIL(src);

    if (dst != src)
    {
        DataType_Dispose(dst);

        dst->internalType = src->internalType;
        strncpy(dst->name, src->name, DATA_TYPE_NAME_LEN);
    }
}

void DataType_SetName(DataType *thiz, const char *name)
{
    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(name);

    strncpy(thiz->name, name, DATA_TYPE_NAME_LEN);
    thiz->internalType = InternalType_RetrieveType(name);
}

void DataType_SetType(DataType *thiz, InternalType type)
{
    RETURN_IF_FAIL(thiz);

    thiz->internalType = type;
    strncpy(thiz->name, InternalType_GetName(type), DATA_TYPE_NAME_LEN);
}

bool DataType_StringToBoolean(const char *string)
{
    RETURN_VAL_IF_FAIL(string, false);

    if (STR_EQUAL("1", string) || str_equal("YES", string, true) || str_equal("TRUE", string, true))
    {
        return true;
    }

    if (STR_EQUAL("0", string) || str_equal("NO", string, true) || str_equal("FALSE", string, true))
    {
        return false;
    }

    LOG_W(TAG, "invalid boolean value: %s", string);

    return false;
}

const char * DataType_BooleanToString(bool b)
{
    return b ? "1" : "0";
}