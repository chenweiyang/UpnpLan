/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   UpnpState.h
*
* @remark
*
*/

#include "UpnpState.h"
#include "tiny_memory.h"

UpnpState * UpnpState_New()
{
    UpnpState *thiz = NULL;

    do
    {
        TinyRet ret = TINY_RET_OK;

        thiz = (UpnpState *)tiny_malloc(sizeof(UpnpState));
        if (thiz == NULL)
        {
            break;
        }

        ret = UpnpState_Construct(thiz);
        if (RET_FAILED(ret))
        {
            UpnpState_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

void UpnpState_Delete(UpnpState *thiz)
{
    RETURN_IF_FAIL(thiz);

    UpnpState_Dispose(thiz);
    tiny_free(thiz);
}

TinyRet UpnpState_Construct(UpnpState *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    thiz->service = NULL;
    thiz->sendEvents = false;
    PropertyDefinition_Construct(&thiz->definition);
    PropertyValue_Construct(&thiz->value);

    return TINY_RET_OK;
}

void UpnpState_Dispose(UpnpState *thiz)
{
    RETURN_IF_FAIL(thiz);

    thiz->sendEvents = false;
    PropertyDefinition_Dispose(&thiz->definition);
    PropertyValue_Dispose(&thiz->value);
}

void UpnpState_Copy(UpnpState *dst, UpnpState *src)
{
    RETURN_IF_FAIL(dst);
    RETURN_IF_FAIL(src);

    UpnpState_Dispose(dst);

    dst->sendEvents = src->sendEvents;
    PropertyDefinition_Copy(&dst->definition, &src->definition);
    PropertyValue_Copy(&dst->value, &src->value);
}

TinyRet UpnpState_Initialize(UpnpState *thiz, const char *name, ObjectType *type, Object *data, bool sendEvents, void *service)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(name, TINY_RET_E_ARG_NULL);

    do
    {
        thiz->service = service;
        thiz->sendEvents = sendEvents;
        PropertyDefinition_Initialize(&thiz->definition, name, type);

        if (data == NULL)
        {
            break;
        }

        if (data->type.clazzType != type->clazzType)
        {
            ret = TINY_RET_E_ARG_INVALID;
            break;
        }

        PropertyValue_Initialize(&thiz->value, data);
    } while (0);

    return ret;
}