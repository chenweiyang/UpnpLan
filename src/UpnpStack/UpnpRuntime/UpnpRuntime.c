/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   UpnpRuntime.c
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#include "UpnpRuntime.h"
#include "tiny_log.h"
#include "tiny_memory.h"
#include "PropertyList.h"
#include "UpnpObject.h"
#include "UpnpUsn.h"
#include "UpnpRegistry.h"
#include "UpnpActionInvoker.h"
#include "UpnpSubscriber.h"
#include "UpnpSubscription.h"

#define TAG             "UpnpRuntime"

static TinyRet UpnpRuntime_Construct(UpnpRuntime *thiz);
static void UpnpRuntime_Dispose(UpnpRuntime *thiz);

static bool object_filter(UpnpUsn *usn, void *ctx);
static void object_listener(UpnpObject *object, bool alive, void *ctx);


struct _UpnpRuntime
{
    UpnpRegistry          * registry;
    UpnpDeviceListener      deviceListener;
    UpnpDeviceFilter        deviceFilter;
    void                  * discoveryCtx;

    UpnpSubscriber        * subscriber;
    UpnpActionInvoker     * invoker;
};

UpnpRuntime * UpnpRuntime_New(void)
{
    UpnpRuntime *thiz = NULL;

    do
    {
        TinyRet ret = TINY_RET_OK;

        thiz = (UpnpRuntime *)tiny_malloc(sizeof(UpnpRuntime));
        if (thiz == NULL)
        {
            break;
        }

        ret = UpnpRuntime_Construct(thiz);
        if (RET_FAILED(ret))
        {
            UpnpRuntime_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

static TinyRet UpnpRuntime_Construct(UpnpRuntime *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(UpnpRuntime));
        thiz->deviceListener = NULL;
        thiz->deviceFilter = NULL;
        thiz->discoveryCtx = NULL;

        thiz->registry = UpnpRegistry_New();
        if (thiz->registry == NULL)
        {
            ret = TINY_RET_E_NEW;
            break;
        }

        thiz->invoker = UpnpActionInvoker_New();
        if (thiz->invoker == NULL)
        {
            ret = TINY_RET_E_NEW;
            break;
        }

        thiz->subscriber = UpnpSubscriber_New();
        if (thiz->subscriber == NULL)
        {
            ret = TINY_RET_E_NEW;
            break;
        }

    } while (0);

    return ret;
}

static void UpnpRuntime_Dispose(UpnpRuntime *thiz)
{
    RETURN_IF_FAIL(thiz);

    UpnpActionInvoker_Delete(thiz->invoker);
    UpnpSubscriber_Delete(thiz->subscriber);
    UpnpRegistry_Delete(thiz->registry);
}

void UpnpRuntime_Delete(UpnpRuntime *thiz)
{
    RETURN_IF_FAIL(thiz);

    UpnpRuntime_Dispose(thiz);
    tiny_free(thiz);
}

TinyRet UpnpCtrlPoint_Start(UpnpRuntime *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    return UpnpRegistry_Start(thiz->registry);
}

TinyRet UpnpCtrlPoint_Stop(UpnpRuntime *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    return UpnpRegistry_Stop(thiz->registry);
}

TinyRet UpnpRuntime_StartScan(UpnpRuntime *thiz, UpnpDeviceListener listener, UpnpDeviceFilter filter, void *ctx)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(listener, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(filter, TINY_RET_E_ARG_NULL);

    thiz->deviceListener = listener;
    thiz->deviceFilter = filter;
    thiz->discoveryCtx = ctx;

    return UpnpRegistry_Discover(thiz->registry, false, object_listener, object_filter, thiz);
}

TinyRet UpnpRuntime_StopScan(UpnpRuntime *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    return UpnpRegistry_StopDiscovery(thiz->registry);
}

TinyRet UpnpRuntime_Invoke(UpnpRuntime *thiz, UpnpAction *action, UpnpError *error)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(action, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(error, TINY_RET_E_ARG_NULL);

    return UpnpActionInvoker_Invoke(thiz->invoker, action, error);
}

TinyRet UpnpRuntime_Subscribe(UpnpRuntime *thiz, UpnpSubscription *subscription, UpnpError *error)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(subscription, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(error, TINY_RET_E_ARG_NULL);

    return UpnpSubscriber_Subscribe(thiz->subscriber, subscription, error);
}

TinyRet UpnpRuntime_Unsubscribe(UpnpRuntime *thiz, UpnpSubscription *subscription, UpnpError *error)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(error, TINY_RET_E_ARG_NULL);

    return UpnpSubscriber_Unsubscribe(thiz->subscriber, error);
}

TinyRet UpnpRuntime_register(UpnpRuntime *thiz, UpnpDevice *device, UpnpActionHandler *handler, void *ctx)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(device, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(handler, TINY_RET_E_ARG_NULL);

    return TINY_RET_E_NOT_IMPLEMENTED;
}

TinyRet UpnpRuntime_unregister(UpnpRuntime *thiz, UpnpDevice *device)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(device, TINY_RET_E_ARG_NULL);

    return TINY_RET_E_NOT_IMPLEMENTED;
}

TinyRet UpnpRuntime_sendEvents(UpnpRuntime *thiz, UpnpService *service)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(service, TINY_RET_E_ARG_NULL);

    return TINY_RET_E_NOT_IMPLEMENTED;
}

static bool object_filter(UpnpUsn *usn, void *ctx)
{
    bool result = false;

    //LOG_D(TAG, "object_filter");

    do
    {
        UpnpRuntime *thiz = (UpnpRuntime *)ctx;
        UpnpUriType type = UPNP_URI_UNDEFINED;

        type = usn->uri.type;
        if (type != UPNP_URI_UPNP_DEVICE && type != UPNP_URI_NON_UPNP_DEVICE)
        {
            //LOG_D(TAG, "%s is not DEVICE", usn->string);
            break;
        }

        result = (thiz->deviceFilter == NULL) ? true : thiz->deviceFilter(usn->uri.domain_name, usn->uri.device_type, thiz->discoveryCtx);
    } while (0);

    return result;
}

static void object_listener(UpnpObject *object, bool alive, void *ctx)
{
    do
    {
        UpnpRuntime *thiz = (UpnpRuntime *)ctx;
        UpnpUri *nt = NULL;

        if (thiz->deviceListener == NULL)
        {
            break;
        }

        nt = UpnpObject_GetNt(object);
        if (nt->type == UPNP_URI_UPNP_DEVICE || nt->type == UPNP_URI_NON_UPNP_DEVICE)
        {
            UpnpDeviceSummary device;
            memset(&device, 0, sizeof(UpnpDeviceSummary));
            strncpy(device.deviceIp, UpnpObject_GetIp(object), TINY_IP_LEN);
            strncpy(device.deviceId, UpnpObject_GetUsn(object), UPNP_USN_LEN);
            strncpy(device.deviceUrl, UpnpObject_GetLocation(object), TINY_URL_LEN);
            strncpy(device.domainName, nt->domain_name, UPNP_DOMAIN_NAME_LEN);
            strncpy(device.deviceType, nt->device_type, UPNP_TYPE_LEN);
            device.deviceVersion = atoi(nt->version);
            strncpy(device.upnpStackInfo, UpnpObject_GetStackInfo(object), UPNP_STACK_INFO_LEN);

            thiz->deviceListener(&device, alive, thiz->discoveryCtx);
        }
    } while (0);
}
