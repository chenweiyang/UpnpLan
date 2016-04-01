/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   UpnpStateVariableDefinition.c
*
* @remark
*
*/

#include "UpnpStateVariableDefinition.h"

#define TAG     "UpnpStateVariableDefinition"

void UpnpStateVariableDefinition_Construct(UpnpStateVariableDefinition *thiz)
{
    RETURN_IF_FAIL(thiz);

    memset(thiz, 0, sizeof(UpnpStateVariableDefinition));
}

void UpnpStateVariableDefinition_Dispose(UpnpStateVariableDefinition *thiz)
{
    RETURN_IF_FAIL(thiz);

    memset(thiz, 0, sizeof(UpnpStateVariableDefinition));
}

void UpnpStateVariableDefinition_Copy(UpnpStateVariableDefinition *dst, UpnpStateVariableDefinition *src)
{
    RETURN_IF_FAIL(dst);
    RETURN_IF_FAIL(src);

    if (dst != src)
    {
        UpnpStateVariableDefinition_Dispose(dst);

        DataType_Copy(&dst->dataType, &src->dataType);
        strncpy(dst->name, src->name, STATE_VARIABLE_NAME_LEN);
    }
}

void UpnpStateVariableDefinition_Initialize(UpnpStateVariableDefinition *thiz, const char *name, DataType *type)
{
    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(name);

    DataType_Copy(&thiz->dataType, type);
    strncpy(thiz->name, name, STATE_VARIABLE_NAME_LEN);
}