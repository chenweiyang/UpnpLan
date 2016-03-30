/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   UpnpSubscription.c
*
* @remark
*
*/

#include "tiny_base.h"
#include "UpnpSubscription.h"
#include "UpnpDevice.h"
#include "tiny_log.h"
#include "tiny_md5.h"
#include "tiny_memory.h"
#include "upnp_net_util.h"

#define TAG                 "UpnpSubscription"

UpnpSubscription * UpnpSubscription_New(UpnpService *service, uint16_t port, uint32_t timeout, UpnpEventListener listener, void *ctx)
{
    UpnpSubscription *thiz = NULL;

    do
    {
        thiz = (UpnpSubscription *)tiny_malloc(sizeof(UpnpSubscription));
        if (thiz == NULL)
        {
            break;
        }

        if (RET_FAILED(UpnpSubscription_Construct(thiz, service, port, timeout, listener, ctx)))
        {
            UpnpSubscription_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

TinyRet UpnpSubscription_Construct(UpnpSubscription *thiz, UpnpService *service, uint16_t port, uint32_t timeout, UpnpEventListener listener, void *ctx)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(UpnpSubscription));
        thiz->service = service;
        thiz->port = port;
        thiz->timeout = timeout;
        thiz->listener = listener;
        thiz->ctx = ctx;

    } while (0);

    return ret;
}

void UpnpSubscription_Dispose(UpnpSubscription *thiz)
{
    RETURN_IF_FAIL(thiz);

    thiz->service = NULL;
    thiz->port = 0;
    thiz->timeout = 0;
    thiz->listener = NULL;
    thiz->ctx = NULL;
}

void UpnpSubscription_Delete(UpnpSubscription *thiz)
{
    RETURN_IF_FAIL(thiz);
    UpnpSubscription_Dispose(thiz);
    tiny_free(thiz);
}

const char * UpnpSubscription_GetCallBackUri(UpnpSubscription *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    if (strlen(thiz->callbackUri) == 0)
    {
        char md5[TINY_MD5_LEN];
        UpnpDevice *device = (UpnpDevice *)UpnpService_GetParentDevice(thiz->service);
        const char *deviceId = UpnpDevice_GetDeviceId(device);
        const char *serviceId = UpnpService_GetServiceId(thiz->service);

        memset(md5, 0, TINY_MD5_LEN);

        tiny_md5_encode(serviceId, md5);
        tiny_snprintf(thiz->callbackUri, TINY_URL_LEN, "/upnp/callback/%s/%s", deviceId, md5);
    }

    return thiz->callbackUri;
}

const char * UpnpSubscription_GetFullCallBack(UpnpSubscription *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    if (strlen(thiz->callbackFull) == 0)
    {
        do
        {
            TinyRet ret = TINY_RET_OK;
            UpnpDevice *device = (UpnpDevice *)UpnpService_GetParentDevice(thiz->service);
            const char *deviceAddress = UpnpDevice_GetAddress(device);
            const char *serviceId = UpnpService_GetServiceId(thiz->service);
            char selfAddress[TINY_IP_LEN];

            memset(selfAddress, 0, TINY_IP_LEN);
            ret = upnp_net_get_ip(deviceAddress, selfAddress, TINY_IP_LEN);
            if (RET_FAILED(ret))
            {
                LOG_E(TAG, "upnp_net_get_ip: %s", tiny_ret_to_str(ret));
                break;
            }

            tiny_snprintf(thiz->callbackFull, TINY_URL_LEN, "<http://%s:%d%s>", selfAddress, thiz->port, thiz->callbackUri);
        } while (0);
    }

    return thiz->callbackFull;
}