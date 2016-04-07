/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   UpnpRegistry.c
*
* @remark
*
*/

#include "UpnpRegistry.h"
#include "tiny_memory.h"
#include "tiny_log.h"
#include "UpnpValidator.h"
#include "UpnpObjectFactory.h"

#define TAG                 "UpnpRegistry"

/**
 * for Ssdp
 */
static void UpnpRegistry_MessageHandler(SsdpMessage *message, void *ctx);
static void UpnpRegistry_OnAlive(UpnpRegistry *thiz, SsdpAlive *alive, const char *ip);
static void UpnpRegistry_OnByebye(UpnpRegistry *thiz, SsdpByebye *byebye, const char *ip);
static void UpnpRegistry_OnRequest(UpnpRegistry *thiz, SsdpRequest *request, const char *localIp, const char *ip, uint16_t port);
static void UpnpRegistry_OnResponse(UpnpRegistry *thiz, SsdpResponse *response, const char *ip);

/**
 * for UpnpProvider
 */
static void OnDeviceAdded(UpnpDevice *device, void *ctx);
static void OnDeviceRemoved(UpnpDevice *device, void *ctx);
static void OnRequestDeviceVisit(UpnpDevice *device, void *ctx);

static bool deviceIsMatched(UpnpDevice *device, const char *st);

UpnpRegistry * UpnpRegistry_New(UpnpProvider *provider)
{
    UpnpRegistry *thiz = NULL;

    do
    {
        TinyRet ret = TINY_RET_OK;

        thiz = (UpnpRegistry *)tiny_malloc(sizeof(UpnpRegistry));
        if (thiz == NULL)
        {
            break;
        }

        ret = UpnpRegistry_Construct(thiz, provider);
        if (RET_FAILED(ret))
        {
            UpnpRegistry_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

TinyRet UpnpRegistry_Construct(UpnpRegistry *thiz, UpnpProvider *provider)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(UpnpRegistry));
        thiz->provider = provider;
        thiz->listener = NULL;
        thiz->ctx = NULL;

        ret = Ssdp_Construct(&thiz->ssdp);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "Ssdp_Construct failed");
            ret = TINY_RET_E_NEW;
            break;
        }

        ret = Ssdp_SetMessageHandler(&thiz->ssdp, UpnpRegistry_MessageHandler, thiz);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "Ssdp_SetMessageHandler failed");
            ret = TINY_RET_E_NEW;
            break;
        }

        ret = UpnpObjectList_Construct(&thiz->foundObjects);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "UpnpObjectList_Construct failed");
            ret = TINY_RET_E_NEW;
            break;
        }

        ret = UpnpValidator_Construct(&thiz->validator);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "UpnpValidator_Construct failed");
            ret = TINY_RET_E_NEW;
            break;
        }
    } while (0);

    return ret;
}

void UpnpRegistry_Dispose(UpnpRegistry *thiz)
{
    RETURN_IF_FAIL(thiz);

    Ssdp_Dispose(&thiz->ssdp);
    UpnpObjectList_Dispose(&thiz->foundObjects);
    UpnpValidator_Dispose(&thiz->validator);
}

void UpnpRegistry_Delete(UpnpRegistry *thiz)
{
    RETURN_IF_FAIL(thiz);

    UpnpRegistry_Dispose(thiz);
    tiny_free(thiz);
}

TinyRet UpnpRegistry_Start(UpnpRegistry *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    UpnpProvider_Lock(thiz->provider);

    do
    {
        ret = Ssdp_Start(&thiz->ssdp);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "Ssdp_Start failed");
            break;
        }

        ret = UpnpProvider_AddObserver(thiz->provider, "Registry", OnDeviceAdded, OnDeviceRemoved, NULL, thiz);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "UpnpProvider_AddObserver failed");
            break;
        }
    } while (0);

    UpnpProvider_Unlock(thiz->provider);

    return ret;
}

TinyRet UpnpRegistry_Stop(UpnpRegistry *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    UpnpProvider_Lock(thiz->provider);

    do
    {
        ret = UpnpProvider_RemoveObserver(thiz->provider, "Registry");
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "UpnpProvider_RemoveObserver failed");
            break;
        }

        ret = Ssdp_Stop(&thiz->ssdp);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "Ssdp_Stop failed");
            break;
        }
    } while (0);

    UpnpProvider_Unlock(thiz->provider);

    return ret;
}

TinyRet UpnpRegistry_Discover(UpnpRegistry *thiz, bool strictedUuid, UpnpObjectListener listener, UpnpObjectFilter filter, void *ctx)
{
    TinyRet ret = TINY_RET_OK;
    SsdpMessage message;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(listener, TINY_RET_E_ARG_NULL);

    do
    {
        thiz->validator.strictedUuid = strictedUuid;
        thiz->validator.filter = filter;
        thiz->validator.ctx = ctx;
        thiz->listener = listener;
        thiz->ctx = ctx;

        ret = SsdpMessage_ConstructRequest(&message, DEFAULT_ST);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = Ssdp_SendMessage(&thiz->ssdp, &message);

        SsdpMessage_Dispose(&message);
    } while (0);

    return ret;
}

TinyRet UpnpRegistry_StopDiscovery(UpnpRegistry *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    thiz->listener = NULL;

    return TINY_RET_OK;
}

static void UpnpRegistry_MessageHandler(SsdpMessage *message, void *ctx)
{
    UpnpRegistry *thiz = (UpnpRegistry *)ctx;

    switch (message->type)
    {
    case SSDP_ALIVE:
        UpnpRegistry_OnAlive(thiz, &message->v.alive, message->remote.ip);
        break;

    case SSDP_BYEBYE:
        UpnpRegistry_OnByebye(thiz, &message->v.byebye, message->remote.ip);
        break;

    case SSDP_MSEARCH_REQUEST:
        UpnpRegistry_OnRequest(thiz, &message->v.request, message->local.ip, message->remote.ip, message->remote.port);
        break;

    case SSDP_MSEARCH_RESPONSE:
        UpnpRegistry_OnResponse(thiz, &message->v.response, message->remote.ip);
        break;

    default:
        break;
    }
}

static void UpnpRegistry_OnAlive(UpnpRegistry *thiz, SsdpAlive *alive, const char *ip)
{
    LOG_D(TAG, "OnAlive");

    UpnpObjectList_Lock(&thiz->foundObjects);

    do
    {
        UpnpObject *obj = NULL;

        if (thiz->listener == NULL)
        {
            break;
        }

        if (!UpnpValidator_ValidateUSN(&thiz->validator, alive->usn))
        {
            break;
        }

        obj = UpnpObjectList_GetObject(&thiz->foundObjects, alive->usn);
        if (obj != NULL)
        {
            UpnpObject_UpdateNextNotify(obj);
            break;
        }

        obj = UpnpObjectFactory_CreateByAlive(alive, ip, &thiz->validator);
        if (obj == NULL)
        {
            break;
        }

        UpnpObjectList_AddObject(&thiz->foundObjects, obj);
        thiz->listener(obj, true, thiz->ctx);
    } while (0);

    UpnpObjectList_Unlock(&thiz->foundObjects);
}

static void UpnpRegistry_OnByebye(UpnpRegistry *thiz, SsdpByebye *byebye, const char *ip)
{
    LOG_D(TAG, "OnByebye");

    UpnpObjectList_Lock(&thiz->foundObjects);

    do
    {
        UpnpObject *obj = NULL;

        if (thiz->listener == NULL)
        {
            break;
        }

        if (!UpnpValidator_ValidateUSN(&thiz->validator, byebye->usn))
        {
            break;
        }

        obj = UpnpObjectList_GetObject(&thiz->foundObjects, byebye->usn);
        if (obj == NULL)
        {
            break;
        }

        thiz->listener(obj, false, thiz->ctx);
        UpnpObjectList_RemoveObject(&thiz->foundObjects, byebye->usn);
    } while (0);

    UpnpObjectList_Unlock(&thiz->foundObjects);
}

typedef struct _OnRequestContext
{
    UpnpRegistry *registry;
    SsdpRequest *request;
    const char *localIp;
    const char *remoteIp;
    uint16_t remotePort;
} OnRequestContext;

static void OnRequestDeviceVisit(UpnpDevice *device, void *ctx)
{
    OnRequestContext *c = (OnRequestContext *)ctx;

    LOG_D(TAG, "OnRequestDeviceVisit");

    if (deviceIsMatched(device, c->request->st))
    {

        do
        {
            SsdpMessage message;
            uint16_t port = UpnpDevice_GetHttpPort(device);
            const char *uri = UpnpDevice_GetURI(device);
            uint32_t count = UpnpDevice_GetServiceCount(device);
            uint32_t i = 0;

#if 0
            char location[TINY_URL_LEN];

            memset(location, 0, TINY_URL_LEN);
            tiny_snprintf(location, TINY_URL_LEN, "http://%s:%d%s", "10.0.1.8", port, uri);
#endif

            /**
             * root
             */
            if (RET_FAILED(SsdpMessage_ConstructResponse_ROOTDEVICE(&message, device, uri, port, c->localIp, c->remoteIp, c->remotePort)))
            {
                break;
            }
            Ssdp_SendMessage(&c->registry->ssdp, &message);
            SsdpMessage_Dispose(&message);

            /**
             * device uuid
             */
            if (RET_FAILED(SsdpMessage_ConstructResponse_DEVICE_UUID(&message, device, uri, port, c->localIp, c->remoteIp, c->remotePort)))
            {
                break;
            }
            Ssdp_SendMessage(&c->registry->ssdp, &message);
            SsdpMessage_Dispose(&message);

            /**
             * device
             */
            if (RET_FAILED(SsdpMessage_ConstructResponse_DEVICE(&message, device, uri, port, c->localIp, c->remoteIp, c->remotePort)))
            {
                break;
            }
            Ssdp_SendMessage(&c->registry->ssdp, &message);
            SsdpMessage_Dispose(&message);

            /**
             * services
             */
            for (i = 0; i < count; ++i)
            {
                UpnpService *service = UpnpDevice_GetServiceAt(device, i);
                if (RET_FAILED(SsdpMessage_ConstructResponse_SERVICE(&message, service, uri, port, c->localIp, c->remoteIp, c->remotePort)))
                {
                    break;
                }
                Ssdp_SendMessage(&c->registry->ssdp, &message);
                SsdpMessage_Dispose(&message);
            }
        } while (0);
    }
}

static bool deviceIsMatched(UpnpDevice *device, const char *st)
{
    if (STR_EQUAL(st, DEFAULT_ST))
    {
        return true;
    }

    // TODO: check deviceType & serviceType

    return false;
}

static void UpnpRegistry_OnRequest(UpnpRegistry *thiz, SsdpRequest *request, const char *localIp, const char *remoteIp, uint16_t remotePort)
{
    LOG_D(TAG, "OnRequest");

    UpnpProvider_Lock(thiz->provider);
    {
        OnRequestContext ctx;
        ctx.registry = thiz;
        ctx.request = request;
        ctx.localIp = localIp;
        ctx.remoteIp = remoteIp;
        ctx.remotePort = remotePort;

        UpnpProvider_Foreach(thiz->provider, request->st, OnRequestDeviceVisit, &ctx);
    }
    UpnpProvider_Unlock(thiz->provider);
}

static void UpnpRegistry_OnResponse(UpnpRegistry *thiz, SsdpResponse *response, const char *ip)
{
    LOG_D(TAG, "OnResponse: %s", response->st);

    UpnpObjectList_Lock(&thiz->foundObjects);

    do
    {
        UpnpObject *obj = NULL;

        if (thiz->listener == NULL)
        {
            break;
        }

        if (!UpnpValidator_ValidateUSN(&thiz->validator, response->usn))
        {
            break;
        }

        obj = UpnpObjectList_GetObject(&thiz->foundObjects, response->usn);
        if (obj != NULL)
        {
            UpnpObject_UpdateNextNotify(obj);
            break;
        }

        obj = UpnpObjectFactory_CreateByResponse(response, ip, &thiz->validator);
        if (obj == NULL)
        {
            break;
        }

        UpnpObjectList_AddObject(&thiz->foundObjects, obj);
        thiz->listener(obj, true, thiz->ctx);
    } while (0);

    UpnpObjectList_Unlock(&thiz->foundObjects);
}

static void OnDeviceAdded(UpnpDevice *device, void *ctx)
{
    UpnpRegistry *thiz = (UpnpRegistry *)ctx;

    LOG_D(TAG, "OnDeviceAdded");

    do
    {
        SsdpMessage message;
        uint16_t port = UpnpDevice_GetHttpPort(device);
        const char *uri = UpnpDevice_GetURI(device);
        uint32_t count = UpnpDevice_GetServiceCount(device);
        uint32_t i = 0;

#if 0
        char location[TINY_URL_LEN];

        memset(location, 0, TINY_URL_LEN);
        tiny_snprintf(location, TINY_URL_LEN, "http://%s:%d%s", "10.0.1.8", port, uri);
#endif

        /**
         * root
         */
        if (RET_FAILED(SsdpMessage_ConstructAlive_ROOTDEVICE(&message, device, uri, port)))
        {
            break;
        }
        Ssdp_SendMessage(&thiz->ssdp, &message);
        SsdpMessage_Dispose(&message);

        /**
         * device uuid
         */
        if (RET_FAILED(SsdpMessage_ConstructAlive_DEVICE_UUID(&message, device, uri, port)))
        {
            break;
        }
        Ssdp_SendMessage(&thiz->ssdp, &message);
        SsdpMessage_Dispose(&message);

        /**
         * device
         */
        if (RET_FAILED(SsdpMessage_ConstructAlive_DEVICE(&message, device, uri, port)))
        {
            break;
        }
        Ssdp_SendMessage(&thiz->ssdp, &message);
        SsdpMessage_Dispose(&message);

        /**
         * services
         */
        for (i = 0; i < count; ++i)
        {
            UpnpService *service = UpnpDevice_GetServiceAt(device, i);
            if (RET_FAILED(SsdpMessage_ConstructAlive_SERVICE(&message, service, uri, port)))
            {
                break;
            }
            Ssdp_SendMessage(&thiz->ssdp, &message);
            SsdpMessage_Dispose(&message);
        }
    } while (0);
}

static void OnDeviceRemoved(UpnpDevice *device, void *ctx)
{
    UpnpRegistry *thiz = (UpnpRegistry *)ctx;

    LOG_D(TAG, "OnDeviceAdded");

    do
    {
        SsdpMessage message;
        uint32_t count = UpnpDevice_GetServiceCount(device);
        uint32_t i = 0;

        /**
         * services
         */
        for (i = 0; i < count; ++i)
        {
            UpnpService *service = UpnpDevice_GetServiceAt(device, i);
            if (RET_FAILED(SsdpMessage_ConstructByebye_SERVICE(&message, service)))
            {
                break;
            }
            Ssdp_SendMessage(&thiz->ssdp, &message);
            SsdpMessage_Dispose(&message);
        }

        /**
         * device
         */
        if (RET_FAILED(SsdpMessage_ConstructByebye_DEVICE(&message, device)))
        {
            break;
        }
        Ssdp_SendMessage(&thiz->ssdp, &message);
        SsdpMessage_Dispose(&message);

        /**
         * device uuid
         */
        if (RET_FAILED(SsdpMessage_ConstructByebye_DEVICE_UUID(&message, device)))
        {
            break;
        }
        Ssdp_SendMessage(&thiz->ssdp, &message);
        SsdpMessage_Dispose(&message);

        /**
         * root
         */
        if (RET_FAILED(SsdpMessage_ConstructByebye_ROOTDEVICE(&message, device)))
        {
            break;
        }
        Ssdp_SendMessage(&thiz->ssdp, &message);
        SsdpMessage_Dispose(&message);
    } while (0);
}
