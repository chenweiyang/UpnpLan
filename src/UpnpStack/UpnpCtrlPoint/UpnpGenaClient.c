/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   UpnpGenaClient.c
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/


#include "UpnpGenaClient.h"
#include "tiny_memory.h"
#include "tiny_log.h"
#include "TinyXml.h"
#include "UpnpEvent.h"
#include "UpnpService.h"
#include "UpnpDevice.h"


#define TAG                 "UpnpGenaClient"

static void item_delete_listener(void * data, void *ctx);
static void notify_handler(UpnpHttpConnection *conn,
    const char *uri,
    const char *nt,
    const char *nts,
    const char *sid,
    const char *seq,
    const char *content,
    uint32_t contentLength,
    void *ctx);

UpnpGenaClient * UpnpGenaClient_New(UpnpHttpManager *http)
{
    UpnpGenaClient *thiz = NULL;

    do
    {
        thiz = (UpnpGenaClient *)tiny_malloc(sizeof(UpnpGenaClient));
        if (thiz == NULL)
        {
            break;
        }

        if (RET_FAILED(UpnpGenaClient_Construct(thiz, http)))
        {
            UpnpGenaClient_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

TinyRet UpnpGenaClient_Construct(UpnpGenaClient *thiz, UpnpHttpManager *http)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(UpnpGenaClient));

        ret = TinyMap_Construct(&thiz->map);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "TinyMap_Construct failed");
            break;
        }

        TinyMap_SetDeleteListener(&thiz->map, item_delete_listener, thiz);

        ret = TinyMutex_Construct(&thiz->mutex);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "TinyMutex_Construct failed");
            break;
        }

        thiz->http = http;

        ret = UpnpHttpServer_RegisterNotifyHandler(&thiz->http->server, notify_handler, thiz);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "UpnpHttpServer_RegisterNotifyHandler: failed");
            break;
        }
    } while (0);

    return ret;
}

void UpnpGenaClient_Dispose(UpnpGenaClient *thiz)
{
    RETURN_IF_FAIL(thiz);

    UpnpHttpServer_UnregisterNotifyHandler(&thiz->http->server);
    thiz->http = NULL;

    TinyMutex_Dispose(&thiz->mutex);
    TinyMap_Dispose(&thiz->map);
}

void UpnpGenaClient_Delete(UpnpGenaClient *thiz)
{
    RETURN_IF_FAIL(thiz);
    UpnpGenaClient_Dispose(thiz);
    tiny_free(thiz);
}

static void item_delete_listener(void * data, void *ctx)
{
    UpnpSubscription *subscription = (UpnpSubscription *)data;
    UpnpSubscription_Delete(subscription);
}

static void notify_handler(UpnpHttpConnection *conn,
    const char *uri,
    const char *nt,
    const char *nts,
    const char *sid,
    const char *seq,
    const char *content,
    uint32_t contentLength,
    void *ctx)
{
    UpnpGenaClient *thiz = (UpnpGenaClient *)ctx;
        
    LOG_D(TAG, "notify_handler");

    printf("uri: %s\n", uri);
    printf("nt: %s\n", nt);
    printf("nts: %s\n", nts);
    printf("sid: %s\n", sid);
    printf("seq: %s\n", seq);
    printf("content: %s\n", content);

    TinyMutex_Lock(&thiz->mutex);

    do
    {
        UpnpSubscription *subscription = NULL;
        UpnpEvent * event = NULL;

        subscription = (UpnpSubscription *)TinyMap_GetValue(&thiz->map, uri);
        if (subscription == NULL)
        {
            UpnpHttpConnection_SendError(conn, 404, "NOT FOUND");
            break;
        }

        event = UpnpEvent_New();
        if (event == NULL)
        {
            LOG_E(TAG, "UpnpEvent_Construct failed");
            break;
        }

        do {
            if (RET_FAILED(UpnpEvent_Parse(event, nt, nts, sid, seq, content, contentLength)))
            {
                break;
            }

            subscription->listener(event, subscription->ctx);
        } while (0);

        UpnpEvent_Delete(event);
        UpnpHttpConnection_SendOk(conn);
    } while (0);  

    TinyMutex_Unlock(&thiz->mutex);
}

TinyRet UpnpGenaClient_Subscribe(UpnpGenaClient *thiz, 
    UpnpService *service,
    uint32_t timeout,
    UpnpEventListener listener,
    void *ctx,
    UpnpError *error)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(service, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(listener, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(error, TINY_RET_E_ARG_NULL);

    TinyMutex_Lock(&thiz->mutex);

    do
    {
        UpnpSubscription *subscription = NULL;

        if (!UpnpHttpServer_IsRunning(&thiz->http->server))
        {
            LOG_E(TAG, "UpnpHttpServer_IsRunning: false");
            ret = TINY_RET_E_STOPPED;
            break;
        }

        subscription = UpnpSubscription_New(service,
            UpnpHttpServer_GetListeningPort(&thiz->http->server),
            timeout,
            listener,
            ctx);
        if (subscription == NULL)
        {
            LOG_D(TAG, "UpnpSubscription_New failed");
            break;
        }

        /**
         * Service already subscribed
         */
        if (TinyMap_GetValue(&thiz->map, UpnpSubscription_GetCallBackUri(subscription)) != NULL)
        {
            LOG_D(TAG, "service already subscribed");
            UpnpSubscription_Delete(subscription);
            ret = TINY_RET_E_ITEM_EXIST;
            break;
        }

        /**
         * add subscription to list
         */
        ret = TinyMap_Insert(&thiz->map, UpnpSubscription_GetCallBackUri(subscription), subscription);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "TinyMap_Insert failed: %s", tiny_ret_to_str(ret));
            UpnpSubscription_Delete(subscription);
            break;
        }

        ret = UpnpHttpClient_Subscribe(&thiz->http->client, subscription, error, UPNP_TIMEOUT);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "UpnpHttpClient_Subscribe failed: %s", tiny_ret_to_str(ret));
            TinyMap_Erase(&thiz->map, UpnpSubscription_GetCallBackUri(subscription));
            break;
        }

        UpnpService_SetCallbackURI(service, UpnpSubscription_GetCallBackUri(subscription));
    } while (0);

    TinyMutex_Unlock(&thiz->mutex);

    return ret;
}

TinyRet UpnpGenaClient_Unsubscribe(UpnpGenaClient *thiz, UpnpService *service, UpnpError *error)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(service, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(error, TINY_RET_E_ARG_NULL);

    TinyMutex_Lock(&thiz->mutex);

    do
    {
        const char *callbackUri = UpnpService_GetCallbackURI(service);
        UpnpSubscription *subscription = NULL;

        if (!UpnpHttpServer_IsRunning(&thiz->http->server))
        {
            LOG_E(TAG, "UpnpHttpServer_IsRunning: false");
            ret = TINY_RET_E_STOPPED;
            break;
        }

        subscription = (UpnpSubscription *)TinyMap_GetValue(&thiz->map, callbackUri);
        if (subscription == NULL)
        {
            LOG_D(TAG, "service not subscribed");
            ret = TINY_RET_E_NOT_FOUND;
            break;
        }

        ret = UpnpHttpClient_Unsubscribe(&thiz->http->client, subscription, error, UPNP_TIMEOUT);
    } while (0);

    TinyMutex_Unlock(&thiz->mutex);

    return ret;
}