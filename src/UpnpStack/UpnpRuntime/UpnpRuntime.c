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
#include "UpnpHttpManager.h"
#include "UpnpRegistry.h"
#include "UpnpActionInvoker.h"
#include "UpnpGenaClient.h"
#include "UpnpSubscription.h"
#include "UpnpProvider.h"
#include "UpnpHost.h"

#define TAG             "UpnpRuntime"

static TinyRet UpnpRuntime_Construct(UpnpRuntime *thiz);
static void UpnpRuntime_Dispose(UpnpRuntime *thiz);

static bool object_filter(UpnpUsn *usn, void *ctx);
static void object_listener(UpnpObject *object, bool alive, void *ctx);

struct _UpnpRuntime
{
    UpnpHttpManager         http;
    UpnpProvider            provider;
    UpnpHost                host;
    UpnpGenaClient          genaClient;
    UpnpActionInvoker       invoker;
    UpnpRegistry            registry;
    UpnpDeviceListener      deviceListener;
    UpnpDeviceFilter        deviceFilter;
    void                  * discoveryCtx;
};

UpnpRuntime * UpnpRuntime_New(void)
{
    UpnpRuntime *thiz = NULL;

    do
    {
        thiz = (UpnpRuntime *)tiny_malloc(sizeof(UpnpRuntime));
        if (thiz == NULL)
        {
            LOG_E(TAG, "OUT OF MEMORY");
            break;
        }

        if (RET_FAILED(UpnpRuntime_Construct(thiz)))
        {
            LOG_E(TAG, "UpnpRuntime_Construct failed");
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

        ret = UpnpHttpManager_Construct(&thiz->http);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "UpnpHttpManager_Construct failed");
            ret = TINY_RET_E_NEW;
            break;
        }

        ret = UpnpProvider_Construct(&thiz->provider);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "UpnpProvider_Construct failed");
            break;
        }

        ret = UpnpHost_Construct(&thiz->host, &thiz->http, &thiz->provider);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "UpnpHost_Construct failed");
            break;
        }

        ret = UpnpActionInvoker_Construct(&thiz->invoker, &thiz->http);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "UpnpActionInvoker_Construct failed");
            break;
        }

        ret = UpnpGenaClient_Construct(&thiz->genaClient, &thiz->http);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "UpnpSubscriber_Construct failed");
            break;
        }

        ret = UpnpRegistry_Construct(&thiz->registry, &thiz->provider);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "UpnpRegistry_Construct failed");
            break;
        }
    } while (0);

    return ret;
}

static void UpnpRuntime_Dispose(UpnpRuntime *thiz)
{
    RETURN_IF_FAIL(thiz);

    UpnpRegistry_Dispose(&thiz->registry);
    UpnpGenaClient_Dispose(&thiz->genaClient);
    UpnpActionInvoker_Dispose(&thiz->invoker);
    UpnpHost_Dispose(&thiz->host);
    UpnpProvider_Dispose(&thiz->provider);
    UpnpHttpManager_Dispose(&thiz->http);
}

void UpnpRuntime_Delete(UpnpRuntime *thiz)
{
    RETURN_IF_FAIL(thiz);

    UpnpRuntime_Dispose(thiz);
    tiny_free(thiz);
}

TinyRet UpnpRuntime_Start(UpnpRuntime *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        ret = UpnpHttpServer_Start(&thiz->http.server);
        if (RET_FAILED(ret))
        {
            LOG_D(TAG, "UpnpHttpServer_Start failed");
            break;
        }

        ret = UpnpHost_Start(&thiz->host);
        if (RET_FAILED(ret))
        {
            LOG_D(TAG, "UpnpHost_Start failed");
            break;
        }

        ret = UpnpRegistry_Start(&thiz->registry);
        if (RET_FAILED(ret))
        {
            LOG_D(TAG, "UpnpHttpServer_Start failed");
            break;
        }
    } while (0);

    return ret;
}

TinyRet UpnpRuntime_Stop(UpnpRuntime *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        ret = UpnpRegistry_Stop(&thiz->registry);
        if (RET_FAILED(ret))
        {
            LOG_D(TAG, "UpnpRegistry_Stop failed");
            break;
        }

        ret = UpnpHost_Stop(&thiz->host);
        if (RET_FAILED(ret))
        {
            LOG_D(TAG, "UpnpHost_Stop failed");
            break;
        }

        ret = UpnpHttpServer_Stop(&thiz->http.server);
        if (RET_FAILED(ret))
        {
            LOG_D(TAG, "UpnpHttpServer_Stop failed");
            break;
        }
    } while (0);

    return ret;
}

TinyRet UpnpRuntime_StartScan(UpnpRuntime *thiz, UpnpDeviceListener listener, UpnpDeviceFilter filter, void *ctx)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(listener, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(filter, TINY_RET_E_ARG_NULL);

    thiz->deviceListener = listener;
    thiz->deviceFilter = filter;
    thiz->discoveryCtx = ctx;

    return UpnpRegistry_Discover(&thiz->registry, false, object_listener, object_filter, thiz);
}

TinyRet UpnpRuntime_StopScan(UpnpRuntime *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    return UpnpRegistry_StopDiscovery(&thiz->registry);
}

TinyRet UpnpRuntime_Invoke(UpnpRuntime *thiz, UpnpAction *action, UpnpError *error)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(action, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(error, TINY_RET_E_ARG_NULL);

    return UpnpActionInvoker_Invoke(&thiz->invoker, action, error);
}

TinyRet UpnpRuntime_Subscribe(UpnpRuntime *thiz, UpnpService *service, uint32_t timeout, UpnpEventListener listener, void *ctx, UpnpError *error)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(service, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(listener, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(error, TINY_RET_E_ARG_NULL);

    return UpnpGenaClient_Subscribe(&thiz->genaClient, service, timeout, listener, ctx, error);
}

TinyRet UpnpRuntime_Unsubscribe(UpnpRuntime *thiz, UpnpService *service, UpnpError *error)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(service, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(error, TINY_RET_E_ARG_NULL);

    return UpnpGenaClient_Unsubscribe(&thiz->genaClient, service, error);
}

TinyRet UpnpRuntime_Register(UpnpRuntime *thiz, UpnpDevice *device, UpnpActionHandler handler, void *ctx)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(device, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(handler, TINY_RET_E_ARG_NULL);

    UpnpProvider_Lock(&thiz->provider); 
    {
        UpnpDevice_SetHttpPort(device, UpnpHttpServer_GetListeningPort(&thiz->http.server));

        ret = UpnpProvider_Add(&thiz->provider, device, handler, ctx);
    }
    UpnpProvider_Unlock(&thiz->provider);

    return ret;
}

TinyRet UpnpRuntime_Unregister(UpnpRuntime *thiz, const char *deviceId)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(deviceId, TINY_RET_E_ARG_NULL);

    UpnpProvider_Lock(&thiz->provider);
    {
        ret = UpnpProvider_Remove(&thiz->provider, deviceId);
    }
    UpnpProvider_Unlock(&thiz->provider);

    return ret;
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

        result = (thiz->deviceFilter == NULL) ? true : thiz->deviceFilter(&usn->uri, thiz->discoveryCtx);
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
            strncpy(device.deviceVersion,nt->version, UPNP_VERSION_LEN);
            strncpy(device.upnpStackInfo, UpnpObject_GetStackInfo(object), UPNP_STACK_INFO_LEN);

            thiz->deviceListener(&device, alive, thiz->discoveryCtx);
        }
    } while (0);
}