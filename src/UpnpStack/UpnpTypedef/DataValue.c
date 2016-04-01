/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   DataValue.h
*
* @remark
*
*/

#include "DataValue.h"
#include "DataType.h"

#define TAG     "DataValue"


void DataValue_Construct(DataValue *thiz)
{
    RETURN_IF_FAIL(thiz);

    memset(thiz, 0, sizeof(DataValue));
}

void DataValue_Dispose(DataValue *thiz)
{
    RETURN_IF_FAIL(thiz);

    do
    {
        if (thiz->internalType = INTERNAL_UNDEFINED)
        {
            break;
        }

        if (thiz->internalType == INTERNAL_STRING)
        {
            if (thiz->internalValue.stringValue != NULL)
            {
                free(thiz->internalValue.stringValue);
            }
        }

        memset(thiz, 0, sizeof(DataValue));
    } while (0);
}

void DataValue_Copy(DataValue *dst, DataValue *src)
{
    RETURN_IF_FAIL(dst);
    RETURN_IF_FAIL(src);

    if (dst != src)
    {
        DataValue_Dispose(dst);

        memcpy(dst, src, sizeof(DataValue));
        if (src->internalType == INTERNAL_STRING)
        {
            if (src->internalValue.stringValue != NULL)
            {
                src->internalValue.stringValue = tiny_strdup(src->internalValue.stringValue);
            }
        }
    }
}

TinyRet DataValue_GetValue(DataValue *thiz, char *value, uint32_t len)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(value, TINY_RET_E_ARG_NULL);

    switch (thiz->internalType)
    {
    case INTERNAL_UNDEFINED:
        break;

    case INTERNAL_BYTE:
        tiny_snprintf(value, len, "%d", thiz->internalValue.byteValue);
        break;

    case INTERNAL_WORD:
        tiny_snprintf(value, len, "%d", thiz->internalValue.wordValue);
        break;

    case INTERNAL_INTEGER:
        tiny_snprintf(value, len, "%d", thiz->internalValue.integerValue);
        break;

    case INTERNAL_LONG:
        tiny_snprintf(value, len, "%l", thiz->internalValue.longValue);
        break;

    case INTERNAL_FLOAT:
        tiny_snprintf(value, len, "%f", thiz->internalValue.floatValue);
        break;

    case INTERNAL_DOUBLE:
        tiny_snprintf(value, len, "%f", thiz->internalValue.doubleValue);
        break;

    case INTERNAL_BOOLEAN:
        tiny_snprintf(value, len, "%s", DataType_BooleanToString(thiz->internalValue.boolValue));
        break;

    case INTERNAL_CHAR:
        tiny_snprintf(value, len, "%c", thiz->internalValue.charValue);
        break;

    case INTERNAL_STRING:
        if (strlen(thiz->internalValue.stringValue) < len)
        {
            strncpy(value, thiz->internalValue.stringValue, len);
        }
        else
        {
            ret = TINY_RET_E_ARG_INVALID;
        }
        break;
    }

    return ret;
}

TinyRet DataValue_SetValue(DataValue *thiz, const char *value)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    switch (thiz->internalType)
    {
    case INTERNAL_UNDEFINED:
        ret = TINY_RET_E_ARG_INVALID;
        break;

    case INTERNAL_BYTE:
        DataValue_SetByte(thiz, (int8_t)(atoi(value)));
        break;

    case INTERNAL_WORD:
        DataValue_SetWord(thiz, (int16_t)atoi(value));
        break;

    case INTERNAL_INTEGER:
        DataValue_SetInteger(thiz, atoi(value));
        break;

    case INTERNAL_LONG:
        DataValue_SetLong(thiz, atol(value));
        break;

    case INTERNAL_FLOAT:
        // BUG !!!
        DataValue_SetFloat(thiz, (float)atof(value));
        break;

    case INTERNAL_DOUBLE:
        DataValue_SetDouble(thiz, atof(value));
        break;

    case INTERNAL_BOOLEAN:
        DataValue_SetBool(thiz, DataType_StringToBoolean(value));
        break;

    case INTERNAL_CHAR:
        DataValue_SetChar(thiz, value[0]);
        break;

    case INTERNAL_STRING:
        DataValue_SetString(thiz, value);
        break;

    default:
        ret = TINY_RET_E_ARG_INVALID;
        break;
    }

    return ret;
}

void DataValue_SetByte(DataValue *thiz, int8_t value)
{
    RETURN_IF_FAIL(thiz);

    DataValue_Dispose(thiz);
    thiz->internalType = INTERNAL_BYTE;
    thiz->internalValue.byteValue = value;
}

void DataValue_SetWord(DataValue *thiz, int16_t value)
{
    RETURN_IF_FAIL(thiz);

    DataValue_Dispose(thiz);
    thiz->internalType = INTERNAL_WORD;
    thiz->internalValue.wordValue = value;
}

void DataValue_SetInteger(DataValue *thiz, int32_t value)
{
    RETURN_IF_FAIL(thiz);

    DataValue_Dispose(thiz);
    thiz->internalType = INTERNAL_INTEGER;
    thiz->internalValue.integerValue = value;
}

void DataValue_SetLong(DataValue *thiz, int64_t value)
{
    RETURN_IF_FAIL(thiz);

    DataValue_Dispose(thiz);
    thiz->internalType = INTERNAL_LONG;
    thiz->internalValue.longValue = value;
}

void DataValue_SetFloat(DataValue *thiz, float value)
{
    RETURN_IF_FAIL(thiz);

    DataValue_Dispose(thiz);
    thiz->internalType = INTERNAL_FLOAT;
    thiz->internalValue.floatValue = value;
}

void DataValue_SetDouble(DataValue *thiz, double value)
{
    RETURN_IF_FAIL(thiz);

    DataValue_Dispose(thiz);
    thiz->internalType = INTERNAL_DOUBLE;
    thiz->internalValue.doubleValue = value;
}

void DataValue_SetBool(DataValue *thiz, bool value)
{
    RETURN_IF_FAIL(thiz);

    DataValue_Dispose(thiz);
    thiz->internalType = INTERNAL_BOOLEAN;
    thiz->internalValue.boolValue = value;
}

void DataValue_SetChar(DataValue *thiz, char value)
{
    RETURN_IF_FAIL(thiz);

    DataValue_Dispose(thiz);
    thiz->internalType = INTERNAL_CHAR;
    thiz->internalValue.charValue = value;
}

void DataValue_SetString(DataValue *thiz, const char * value)
{
    RETURN_IF_FAIL(thiz);

    DataValue_Dispose(thiz);
    thiz->internalType = INTERNAL_STRING;
    if (value == NULL)
    {
        thiz->internalValue.stringValue = NULL;
    }
    else
    {
        thiz->internalValue.stringValue = tiny_strdup(value);
    }
}