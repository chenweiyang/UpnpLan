/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   UpnpStateVariable.c
*
* @remark
*
*/

#include "UpnpStateVariable.h"
#include "tiny_memory.h"

UpnpStateVariable * UpnpStateVariable_New()
{
    UpnpStateVariable *thiz = NULL;

    do
    {
        thiz = (UpnpStateVariable *)tiny_malloc(sizeof(UpnpStateVariable));
        if (thiz == NULL)
        {
            break;
        }

        if (RET_FAILED(UpnpStateVariable_Construct(thiz)))
        {
            UpnpStateVariable_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

TinyRet UpnpStateVariable_Construct(UpnpStateVariable *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    UpnpStateVariableDefinition_Construct(&thiz->definition);
    DataValue_Construct(&thiz->value);

    return TINY_RET_OK;
}

void UpnpStateVariable_Dispose(UpnpStateVariable *thiz)
{
    RETURN_IF_FAIL(thiz);

    UpnpStateVariableDefinition_Dispose(&thiz->definition);
    DataValue_Dispose(&thiz->value);
}

void UpnpStateVariable_Delete(UpnpStateVariable *thiz)
{
    RETURN_IF_FAIL(thiz);

    UpnpStateVariable_Dispose(thiz);
    tiny_free(thiz);
}

void UpnpStateVariable_Copy(UpnpStateVariable *dst, UpnpStateVariable *src)
{
    RETURN_IF_FAIL(dst);
    RETURN_IF_FAIL(src);

    if (dst != src)
    {
        UpnpStateVariable_Dispose(dst);

        UpnpStateVariableDefinition_Copy(&dst->definition, &src->definition);
        DataValue_Copy(&dst->value, &src->value);
    }
}

TinyRet UpnpStateVariable_Initialize(UpnpStateVariable *thiz, const char *name, const char *dataType, const char *defaultValue, const char *sendEvents)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(name, TINY_RET_E_ARG_NULL);

    do
    {
        DataType type;
        DataValue value;

        DataType_Construct(&type);
        DataValue_Construct(&value);
        do
        {
            DataType_SetName(&type, dataType);

            value.internalType = type.internalType;
            if (defaultValue != NULL)
            {
                DataValue_SetValue(&value, defaultValue);
            }

            UpnpStateVariableDefinition_Initialize(&thiz->definition, name, &type);

            thiz->sendEvents = DataType_StringToBoolean(sendEvents);

            DataValue_Copy(&thiz->value, &value);
        } while (0);

        DataValue_Dispose(&value);
        DataType_Dispose(&type);
    } while (0);

    return ret;
}
