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
#include "TinyList.h"
#include "tiny_memory.h"

static TinyRet UpnpService_Construct(UpnpService *thiz);
static void UpnpService_Dispose(UpnpService *thiz);

static void ActionDeleteListener(void * data, void *ctx)
{
    UpnpAction *a = (UpnpAction *)data;
    UpnpAction_Delete(a);
}

static void UpnpStateVariableDeleteListener(void * data, void *ctx)
{
    UpnpStateVariable * v = (UpnpStateVariable *)data;
    UpnpStateVariable_Delete(v);
}

static void SubscriberDeleteListener(void * data, void *ctx)
{
    UpnpSubscriber * s = (UpnpSubscriber *)data;
    UpnpSubscriber_Delete(s);
}

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
    TinyList actionList;
    TinyList stateVariableTable;
    UpnpServiceChangedListener changedListener;
    void * changedCtx;

    TinyList subscriberList;
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

        ret = TinyList_Construct(&thiz->actionList);
        if (RET_FAILED(ret))
        {
            ret = TINY_RET_E_NEW;
            break;
        }

        TinyList_SetDeleteListener(&thiz->actionList, ActionDeleteListener, thiz);

        ret = TinyList_Construct(&thiz->stateVariableTable);
        if (RET_FAILED(ret))
        {
            ret = TINY_RET_E_NEW;
            break;
        }

        TinyList_SetDeleteListener(&thiz->stateVariableTable, UpnpStateVariableDeleteListener, thiz);

        ret = TinyList_Construct(&thiz->subscriberList);
        if (RET_FAILED(ret))
        {
            ret = TINY_RET_E_NEW;
            break;
        }

        TinyList_SetDeleteListener(&thiz->subscriberList, SubscriberDeleteListener, thiz);
    } while (0);

    return ret;
}

static void UpnpService_Dispose(UpnpService *thiz)
{
    RETURN_IF_FAIL(thiz);

    TinyList_Dispose(&thiz->subscriberList);
    TinyList_Dispose(&thiz->stateVariableTable);
    TinyList_Dispose(&thiz->actionList);
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
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        bool isChanged = false;
        int i = 0;

        if (thiz->changedListener == NULL)
        {
            break;
        }

        for (i = 0; i < TinyList_GetCount(&thiz->stateVariableTable); ++i)
        {
            UpnpStateVariable *v = (UpnpStateVariable *)TinyList_GetAt(&thiz->stateVariableTable, i);
            if (v->sendEvents && v->isChanged)
            {
                isChanged = true;
                break;
            }
        }

        if (isChanged)
        {
            thiz->changedListener(thiz, thiz->changedCtx);
        }
    } while (0);

    return ret;
}

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

TinyRet UpnpService_AddAction(UpnpService *thiz, UpnpAction *action)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(action, TINY_RET_E_ARG_NULL);

    UpnpAction_SetParentService(action, thiz);

    return TinyList_AddTail(&thiz->actionList, action);
}

uint32_t UpnpService_GetActionCount(UpnpService *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, 0);

    return TinyList_GetCount(&thiz->actionList);
}

UpnpAction * UpnpService_GetActionAt(UpnpService *thiz, uint32_t index)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return (UpnpAction *)TinyList_GetAt(&thiz->actionList, index);
}

UpnpAction * UpnpService_GetAction(UpnpService *thiz, const char *actionName)
{
    uint32_t i = 0;
    uint32_t count = 0;

    RETURN_VAL_IF_FAIL(thiz, NULL);
    RETURN_VAL_IF_FAIL(actionName, NULL);

    count = TinyList_GetCount(&thiz->actionList);

    for (i = 0; i < count; ++i)
    {
        UpnpAction *action = (UpnpAction *)TinyList_GetAt(&thiz->actionList, i);
        if (STR_EQUAL(UpnpAction_GetName(action), actionName))
        {
            return action;
        }
    }

    return NULL;
}

TinyRet UpnpService_AddStateVariable(UpnpService *thiz, UpnpStateVariable *stateVariable)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(stateVariable, TINY_RET_E_ARG_NULL);

    stateVariable->service = thiz;

    return TinyList_AddTail(&thiz->stateVariableTable, stateVariable);
}

uint32_t UpnpService_GetStateVariableCount(UpnpService *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, 0);

    return TinyList_GetCount(&thiz->stateVariableTable);
}

UpnpStateVariable * UpnpService_GetStateVariableAt(UpnpService *thiz, uint32_t index)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return (UpnpStateVariable *)TinyList_GetAt(&thiz->stateVariableTable, index);
}

UpnpStateVariable * UpnpService_GetStateVariable(UpnpService *thiz, const char *stateName)
{
    uint32_t i = 0;
    uint32_t count = 0;

    RETURN_VAL_IF_FAIL(thiz, NULL);
    RETURN_VAL_IF_FAIL(stateName, NULL);

    count = TinyList_GetCount(&thiz->stateVariableTable);

    for (i = 0; i < count; ++i)
    {
        UpnpStateVariable *state = (UpnpStateVariable *)TinyList_GetAt(&thiz->stateVariableTable, i);
        if (STR_EQUAL(state->definition.name, stateName))
        {
            return state;
        }
    }

    return NULL;
}

TinyRet UpnpService_AddSubscriber(UpnpService *thiz, UpnpSubscriber *subscriber)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(subscriber, TINY_RET_E_ARG_NULL);

    return TinyList_AddTail(&thiz->subscriberList, subscriber);
}

TinyRet UpnpService_RemoveSubscriber(UpnpService *thiz, const char *sid)
{
    uint32_t i = 0;
    uint32_t count = 0;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(sid, TINY_RET_E_ARG_NULL);

    count = TinyList_GetCount(&thiz->subscriberList);

    for (i = 0; i < count; ++i)
    {
        UpnpSubscriber *s = (UpnpSubscriber *)TinyList_GetAt(&thiz->subscriberList, i);
        if (STR_EQUAL(UpnpSubscriber_GetSid(s), sid))
        {
            return TinyList_RemoveAt(&thiz->subscriberList, i);
        }
    }

    return TINY_RET_E_NOT_FOUND;
}

uint32_t UpnpService_GetSubscriberCount(UpnpService *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, 0);

    return TinyList_GetCount(&thiz->subscriberList);
}

UpnpSubscriber * UpnpService_GetSubscriberAt(UpnpService *thiz, uint32_t index)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return (UpnpSubscriber *)TinyList_GetAt(&thiz->subscriberList, index);
}

UpnpSubscriber * UpnpService_GetSubscriber(UpnpService *thiz, const char *callback)
{
    uint32_t i = 0;
    uint32_t count = 0;

    RETURN_VAL_IF_FAIL(thiz, NULL);
    RETURN_VAL_IF_FAIL(callback, NULL);

    count = TinyList_GetCount(&thiz->subscriberList);

    for (i = 0; i < count; ++i)
    {
        UpnpSubscriber *s = (UpnpSubscriber *)TinyList_GetAt(&thiz->subscriberList, i);
        if (STR_EQUAL(UpnpSubscriber_GetCallback(s), callback))
        {
            return s;
        }
    }

    return NULL;
}
