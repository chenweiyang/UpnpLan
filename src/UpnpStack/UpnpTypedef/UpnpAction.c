/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   UpnpAction.c
*
* @remark
*
*/

#include "UpnpAction.h"
#include "UpnpActionDefinition.h"
#include "tiny_memory.h"

static TinyRet UpnpAction_Construct(UpnpAction *thiz);
static void UpnpAction_Dispose(UpnpAction *thiz);
static TinyRet UpnpAction_Initialize(UpnpAction *thiz);

struct _UpnpAction
{
    void * service;
    PropertyList * propertyList;
    PropertyList * argumentList;
    PropertyList * resultList;
};

UpnpAction * UpnpAction_New(void)
{
    UpnpAction *thiz = NULL;

    do
    {
        TinyRet ret = TINY_RET_OK;

        thiz = (UpnpAction *)tiny_malloc(sizeof(UpnpAction));
        if (thiz == NULL)
        {
            break;
        }

        ret = UpnpAction_Construct(thiz);
        if (RET_FAILED(ret))
        {
            UpnpAction_Delete(thiz);
            thiz = NULL;
            break;
        }

        ret = UpnpAction_Initialize(thiz);
        if (RET_FAILED(ret))
        {
            UpnpAction_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

void UpnpAction_Delete(UpnpAction *thiz)
{
    RETURN_IF_FAIL(thiz);

    UpnpAction_Dispose(thiz);
    tiny_free(thiz);
}

static TinyRet UpnpAction_Construct(UpnpAction *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(UpnpAction));

        thiz->propertyList = PropertyList_New();
        if (thiz->propertyList == NULL)
        {
            ret = TINY_RET_E_NEW;
            break;
        }

        thiz->argumentList = PropertyList_New();
        if (thiz->argumentList == NULL)
        {
            ret = TINY_RET_E_NEW;
            break;
        }

        thiz->resultList = PropertyList_New();
        if (thiz->resultList == NULL)
        {
            ret = TINY_RET_E_NEW;
            break;
        }
    } while (0);

    return ret;
}

static void UpnpAction_Dispose(UpnpAction *thiz)
{
    RETURN_IF_FAIL(thiz);

    PropertyList_Delete(thiz->propertyList);
    PropertyList_Delete(thiz->argumentList);
    PropertyList_Delete(thiz->resultList);
}

static TinyRet UpnpAction_Initialize(UpnpAction *thiz)
{
    TinyRet ret = TINY_RET_OK;
    ObjectType type;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    ObjectType_Construct(&type);
    {
        ObjectType_SetType(&type, CLAZZ_STRING);
        ret = PropertyList_InitProperty(thiz->propertyList, UPNP_ACTION_Name, &type);
    }
    ObjectType_Dispose(&type);

    return ret;
}

void UpnpAction_SetParentService(UpnpAction *thiz, void *service)
{
    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(service);

    thiz->service = service;
}

void* UpnpAction_GetParentService(UpnpAction *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->service;
}

PropertyList* UpnpAction_GetArgumentList(UpnpAction *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->argumentList;
}

PropertyList* UpnpAction_GetResultList(UpnpAction *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->resultList;
}

PropertyList* UpnpAction_GetPropertyList(UpnpAction *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->propertyList;
}

TinyRet UpnpAction_SetPropertyValue(UpnpAction *thiz, const char *propertyName, const char *value)
{
    TinyRet ret = TINY_RET_OK;
    Object data;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(propertyName, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(value, TINY_RET_E_ARG_NULL);

    Object_Construct(&data);
    {
        Object_setString(&data, value);
        ret = PropertyList_SetPropertyValue(thiz->propertyList, propertyName, &data);
    }
    Object_Dispose(&data);

    return ret;
}

const char * UpnpAction_GetPropertyValue(UpnpAction *thiz, const char *propertyName)
{
    const char *value = NULL;
    Object *data = NULL;

    RETURN_VAL_IF_FAIL(thiz, NULL);
    RETURN_VAL_IF_FAIL(propertyName, NULL);

    data = PropertyList_GetPropertyValue(thiz->propertyList, propertyName);
    if (data != NULL)
    {
        value = data->value.stringValue;
    }

    return value;
}
