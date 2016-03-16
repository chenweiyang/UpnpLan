/*
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   ObjectType.c
 *
 * @remark
 *
 */

#include "ObjectType.h"
#include "tiny_log.h"

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
static ClazzType ClazzType_RetrieveType(const char *typeName)
{
    ClazzType t = CLAZZ_INTEGER;

    do
    {
        if (STR_EQUAL(typeName, TYPE_I1))
        {
            t = CLAZZ_BYTE;
            break;
        }

        if (STR_EQUAL(typeName, TYPE_UI1) || STR_EQUAL(typeName, TYPE_I2))
        {
            t = CLAZZ_WORD;
            break;
        }

        if (STR_EQUAL(typeName, TYPE_UI2) || STR_EQUAL(typeName, TYPE_I4) || STR_EQUAL(typeName, TYPE_INT))
        {
            t = CLAZZ_INTEGER;
            break;
        }

        if (STR_EQUAL(typeName, TYPE_UI4) || STR_EQUAL(typeName, TYPE_TIME) || STR_EQUAL(typeName, TYPE_TIME_TZ))
        {
            t = CLAZZ_LONG;
            break;
        }

        if (STR_EQUAL(typeName, TYPE_R4) || STR_EQUAL(typeName, TYPE_FLOAT))
        {
            t = CLAZZ_FLOAT;
            break;
        }

        if (STR_EQUAL(typeName, TYPE_R8) || STR_EQUAL(typeName, TYPE_NUMBER) || STR_EQUAL(typeName, TYPE_FIXED_14_4))
        {
            t = CLAZZ_DOUBLE;
            break;
        }

        if (STR_EQUAL(typeName, TYPE_CHAR))
        {
            t = CLAZZ_CHAR;
            break;
        }

        if (STR_EQUAL(typeName, TYPE_BOOLEAN))
        {
            t = CLAZZ_BOOLEAN;
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
            t = CLAZZ_STRING;
            break;
        }

    } while (0);

    return t;
}

static const char * ClazzType_GetName(ClazzType type)
{
    const char * t = NULL;

    switch (type)
    {
    case CLAZZ_BYTE:
        t = TYPE_CHAR;
        break;

    case CLAZZ_WORD:
        t = TYPE_I2;
        break;

    case CLAZZ_INTEGER:
        t = TYPE_I4;
        break;

    case CLAZZ_LONG:
        t = TYPE_UI4;
        break;

    case CLAZZ_FLOAT:
        t = TYPE_FLOAT;
        break;

    case CLAZZ_DOUBLE:
        t = TYPE_NUMBER;
        break;

    case CLAZZ_BOOLEAN:
        t = TYPE_BOOLEAN;
        break;

    case CLAZZ_CHAR:
        t = TYPE_CHAR;
        break;

    case CLAZZ_STRING:
        t = TYPE_STRING;
        break;

    default:
        break;
    }

    return t;
}

void ObjectType_Construct(ObjectType *thiz)
{
    RETURN_IF_FAIL(thiz);

    memset(thiz, 0, sizeof(ObjectType));
}

void ObjectType_Dispose(ObjectType *thiz)
{
    RETURN_IF_FAIL(thiz);

    memset(thiz, 0, sizeof(ObjectType));
}

void ObjectType_Copy(ObjectType *dst, ObjectType *src)
{
    RETURN_IF_FAIL(dst);
    RETURN_IF_FAIL(src);

    if (dst != src)
    {
        ObjectType_Dispose(dst);

        dst->clazzType = src->clazzType;
        strncpy(dst->clazzName, src->clazzName, CLAZZ_NAME_LEN);
    }
}

void ObjectType_SetName(ObjectType *thiz, const char *clazzName)
{
    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(clazzName);

    strncpy(thiz->clazzName, clazzName, CLAZZ_NAME_LEN);
    thiz->clazzType = ClazzType_RetrieveType(clazzName);
}

void ObjectType_SetType(ObjectType *thiz, ClazzType clazzType)
{
    RETURN_IF_FAIL(thiz);

    thiz->clazzType = clazzType;
    strncpy(thiz->clazzName, ClazzType_GetName(clazzType), CLAZZ_NAME_LEN);
}

bool ObjectType_StringToBoolean(const char *string)
{
    RETURN_VAL_IF_FAIL(string, false);

    if (STR_EQUAL("1", string) || STR_EQUAL("YES", string) || STR_EQUAL("TRUE", string))
    {
        return true;
    }

    if (STR_EQUAL("0", string) || STR_EQUAL("NO", string) || STR_EQUAL("FALSE", string))
    {
        return false;
    }

    LOG_W(TAG, "invalid boolean value: %s", string);

    return false;
}

const char * ObjectType_BooleanToString(bool b)
{
    return b ? "1" : "0";
}