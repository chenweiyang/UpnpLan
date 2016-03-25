/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   UpnpService.c
*
* @remark
*
*/

#include "UpnpService.h"
#include "UpnpServiceDefinition.h"
#include "PropertyList.h"
#include "UpnpActionList.h"
#include "UpnpStateList.h"
#include "tiny_memory.h"

static TinyRet UpnpService_Construct(UpnpService *thiz);
static void UpnpService_Dispose(UpnpService *thiz);
static TinyRet UpnpService_Initialize(UpnpService *thiz);

struct _UpnpService
{
    void * device;
    PropertyList * propertyList;
    UpnpActionList * actionList;
    UpnpStateList * stateList;
};

UpnpService * UpnpService_New(void)
{
    UpnpService *thiz = NULL;

    do
    {
        TinyRet ret = TINY_RET_OK;

        thiz = (UpnpService *)tiny_malloc(sizeof(UpnpService));
        if (thiz == NULL)
        {
            break;
        }

        ret = UpnpService_Construct(thiz);
        if (RET_FAILED(ret))
        {
            UpnpService_Delete(thiz);
            thiz = NULL;
            break;
        }

        ret = UpnpService_Initialize(thiz);
        if (RET_FAILED(ret))
        {
            UpnpService_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

void UpnpService_Delete(UpnpService *thiz)
{
    RETURN_IF_FAIL(thiz);

    UpnpService_Dispose(thiz);
    tiny_free(thiz);
}

static TinyRet UpnpService_Construct(UpnpService *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(UpnpService));

        thiz->propertyList = PropertyList_New();
        if (thiz->propertyList == NULL)
        {
            ret = TINY_RET_E_NEW;
            break;
        }

        thiz->actionList = UpnpActionList_New();
        if (thiz->actionList == NULL)
        {
            ret = TINY_RET_E_NEW;
            break;
        }

        thiz->stateList = UpnpStateList_New();
        if (thiz->actionList == NULL)
        {
            ret = TINY_RET_E_NEW;
            break;
        }
    } while (0);

    return ret;
}

static void UpnpService_Dispose(UpnpService *thiz)
{
    RETURN_IF_FAIL(thiz);

    PropertyList_Delete(thiz->propertyList);
    UpnpActionList_Delete(thiz->actionList);
    UpnpStateList_Delete(thiz->stateList);
}

static TinyRet UpnpService_Initialize(UpnpService *thiz)
{
    TinyRet ret = TINY_RET_OK;
    ObjectType type;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    ObjectType_Construct(&type);

    do
    {
        ObjectType_SetType(&type, CLAZZ_STRING);

        ret = PropertyList_InitProperty(thiz->propertyList, UPNP_SERVICE_ServiceType, &type);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = PropertyList_InitProperty(thiz->propertyList, UPNP_SERVICE_ServiceId, &type);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = PropertyList_InitProperty(thiz->propertyList, UPNP_SERVICE_ControlURL, &type);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = PropertyList_InitProperty(thiz->propertyList, UPNP_SERVICE_EventSubURL, &type);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = PropertyList_InitProperty(thiz->propertyList, UPNP_SERVICE_SCPDURL, &type);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = PropertyList_InitProperty(thiz->propertyList, UPNP_SERVICE_CallbackURI, &type);
        if (RET_FAILED(ret))
        {
            break;
        }
    } while (0);
    
    ObjectType_Dispose(&type);

    return ret;
}

void UpnpService_SetParentDevice(UpnpService *thiz, void *device)
{
    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(device);

    thiz->device = device;
}

void * UpnpService_GetParentDevice(UpnpService *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->device;
}

UpnpActionList * UpnpService_GetActionList(UpnpService *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->actionList;
}

UpnpStateList * UpnpService_GetStateList(UpnpService *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->stateList;
}

PropertyList * UpnpService_GetPropertyList(UpnpService *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->propertyList;
}

TinyRet UpnpService_SetPropertyValue(UpnpService *thiz, const char *propertyName, const char *value)
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

const char * UpnpService_GetPropertyValue(UpnpService *thiz, const char *propertyName)
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