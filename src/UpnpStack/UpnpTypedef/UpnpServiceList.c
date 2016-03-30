/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   UpnpServiceList.c
*
* @remark
*
*/

#include "UpnpServiceList.h"
#include "TinyMap.h"
#include "tiny_memory.h"

static TinyRet UpnpServiceList_Construct(UpnpServiceList *thiz);
static void UpnpServiceList_Dispose(UpnpServiceList *thiz);
static void ServiceDeleteListener(void * data, void *ctx);

struct _UpnpServiceList
{
    TinyMap      actions;
};

UpnpServiceList * UpnpServiceList_New(void)
{
    UpnpServiceList *thiz = NULL;

    do
    {
        TinyRet ret = TINY_RET_OK;

        thiz = (UpnpServiceList *)tiny_malloc(sizeof(UpnpServiceList));
        if (thiz == NULL)
        {
            break;
        }

        ret = UpnpServiceList_Construct(thiz);
        if (RET_FAILED(ret))
        {
            UpnpServiceList_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

void UpnpServiceList_Delete(UpnpServiceList * thiz)
{
    RETURN_IF_FAIL(thiz);

    UpnpServiceList_Dispose(thiz);
    tiny_free(thiz);
}

static TinyRet UpnpServiceList_Construct(UpnpServiceList *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(UpnpServiceList));

        ret = TinyMap_Construct(&thiz->actions);
        if (RET_FAILED(ret))
        {
            break;
        }

        TinyMap_SetDeleteListener(&thiz->actions, ServiceDeleteListener, thiz);
    } while (0);

    return ret;
}

static void UpnpServiceList_Dispose(UpnpServiceList *thiz)
{
    RETURN_IF_FAIL(thiz);

    TinyMap_Dispose(&thiz->actions);
}

static void ServiceDeleteListener(void * data, void *ctx)
{
    UpnpService *service = (UpnpService *)data;
    UpnpService_Delete(service);
}

TinyRet UpnpServiceList_AddService(UpnpServiceList * thiz, UpnpService *service)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(service, TINY_RET_E_ARG_NULL);

    return TinyMap_Insert(&thiz->actions, UpnpService_GetServiceId(service), service);
}

uint32_t UpnpServiceList_GetSize(UpnpServiceList *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, 0);

    return TinyMap_GetSize(&thiz->actions);
}

UpnpService * UpnpServiceList_GetServiceAt(UpnpServiceList *thiz, uint32_t index)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return (UpnpService *)TinyMap_GetValueAt(&thiz->actions, index);
}

UpnpService * UpnpServiceList_GetService(UpnpServiceList *thiz, const char *actionName)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return (UpnpService *)TinyMap_GetValue(&thiz->actions, actionName);
}