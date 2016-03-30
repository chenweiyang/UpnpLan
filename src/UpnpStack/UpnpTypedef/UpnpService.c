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
#include "PropertyList.h"
#include "UpnpActionList.h"
#include "UpnpStateList.h"
#include "tiny_memory.h"

static TinyRet UpnpService_Construct(UpnpService *thiz);
static void UpnpService_Dispose(UpnpService *thiz);

#define SERVICE_TYPE_LEN    128
#define SERVICE_ID_LEN      128

struct _UpnpService
{
    char serviceType[SERVICE_TYPE_LEN];
    char serviceId[SERVICE_ID_LEN];
    char controlURL[TINY_URL_LEN];
    char eventSubURL[TINY_URL_LEN];
    char SCPDURL[TINY_URL_LEN];
    char callbackURI[TINY_URI_LEN];

    void * device;
    UpnpStateList * stateList;
    UpnpActionList * actionList;
    UpnpServiceChangedListener changedListener;
    void * changedCtx;
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
        thiz->device = NULL;
        thiz->changedListener = NULL;
        thiz->changedCtx = NULL;

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

    UpnpActionList_Delete(thiz->actionList);
    UpnpStateList_Delete(thiz->stateList);
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

void UpnpService_SetChangedListener(UpnpService *thiz, UpnpServiceChangedListener listener, void *ctx)
{
    RETURN_IF_FAIL(thiz);

    thiz->changedListener = listener;
    thiz->changedCtx = ctx;
}

TinyRet UpnpService_SendEvents(UpnpService *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        if (thiz->changedListener == NULL)
        {
            break;
        }

        // TODO ...

    } while (0);

    return TINY_RET_E_NOT_IMPLEMENTED;
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

#if 0
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
#endif

TinyRet UpnpService_SetServiceType(UpnpService *thiz, const char *serviceType)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(serviceType, TINY_RET_E_ARG_NULL);

    strncpy(thiz->serviceType, serviceType, SERVICE_TYPE_LEN);

    return TINY_RET_OK;
}

TinyRet UpnpService_SetServiceId(UpnpService *thiz, const char *serviceId)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(serviceId, TINY_RET_E_ARG_NULL);

    strncpy(thiz->serviceId, serviceId, SERVICE_ID_LEN);

    return TINY_RET_OK;
}

TinyRet UpnpService_SetControlURL(UpnpService *thiz, const char *controlURL)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(controlURL, TINY_RET_E_ARG_NULL);

    strncpy(thiz->controlURL, controlURL, TINY_URL_LEN);

    return TINY_RET_OK;
}

TinyRet UpnpService_SetEventSubURL(UpnpService *thiz, const char *eventSubURL)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(eventSubURL, TINY_RET_E_ARG_NULL);

    strncpy(thiz->eventSubURL, eventSubURL, TINY_URL_LEN);

    return TINY_RET_OK;
}

TinyRet UpnpService_SetSCPDURL(UpnpService *thiz, const char *SCPDURL)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(SCPDURL, TINY_RET_E_ARG_NULL);

    strncpy(thiz->SCPDURL, SCPDURL, TINY_URL_LEN);

    return TINY_RET_OK;
}

TinyRet UpnpService_SetCallbackURI(UpnpService *thiz, const char *callbackURI)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(callbackURI, TINY_RET_E_ARG_NULL);

    strncpy(thiz->callbackURI, callbackURI, TINY_URI_LEN);

    return TINY_RET_OK;
}

const char * UpnpService_GetServiceType(UpnpService *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->serviceType;
}

const char * UpnpService_GetServiceId(UpnpService *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->serviceId;
}

const char * UpnpService_GetControlURL(UpnpService *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->controlURL;
}

const char * UpnpService_GetEventSubURL(UpnpService *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->eventSubURL;
}

const char * UpnpService_GetSCPDURL(UpnpService *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->SCPDURL;
}

const char * UpnpService_GetCallbackURI(UpnpService *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->callbackURI;
}