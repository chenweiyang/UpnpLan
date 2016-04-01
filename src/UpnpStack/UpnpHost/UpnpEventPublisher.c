/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   UpnpEventPublisher.c
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#include "UpnpEventPublisher.h"
#include "tiny_memory.h"
#include "tiny_log.h"

#define TAG     "UpnpEventPublisher"


static void OnSubscribe(UpnpHttpConnection *conn, const char *uri, const char *callback, const char *nt, const char *timeout, void *ctx)
{
    LOG_D(TAG, "OnSubscribe: %s", uri);

    UpnpHttpConnection_SendError(conn, 404, "NOT FOUND");
}

static void OnUnsubscribe(UpnpHttpConnection *conn, const char *uri, const char *sid, void *ctx)
{
    LOG_D(TAG, "OnUnsubscribe: %s", uri);

    UpnpHttpConnection_SendError(conn, 404, "NOT FOUND");
}

static void OnServiceChanged(UpnpService *service, void *ctx)
{
    LOG_D(TAG, "OnServiceChanged");

    // Send NOTIFY
}

UpnpEventPublisher * UpnpEventPublisher_New(UpnpHttpManager *http, UpnpProvider *provider)
{
    UpnpEventPublisher *thiz = NULL;

    do
    {
        TinyRet ret = TINY_RET_OK;

        thiz = (UpnpEventPublisher *)tiny_malloc(sizeof(UpnpEventPublisher));
        if (thiz == NULL)
        {
            break;
        }

        ret = UpnpEventPublisher_Construct(thiz, http, provider);
        if (RET_FAILED(ret))
        {
            UpnpEventPublisher_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

TinyRet UpnpEventPublisher_Construct(UpnpEventPublisher *thiz, UpnpHttpManager *http, UpnpProvider *provider)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(UpnpEventPublisher));
        thiz->http = http;
        thiz->provider = provider;

        ret = UpnpHttpServer_RegisterSubscribeHandler(&http->server, OnSubscribe, thiz);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "UpnpHttpServer_RegisterSubscribeHandler: failed");
            break;
        }

        ret = UpnpHttpServer_RegisterUnsubscribeHandler(&http->server, OnUnsubscribe, thiz);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "UpnpHttpServer_RegisterUnsubscribeHandler: failed");
            break;
        }

        /** 
         * monitor provider
         */
        UpnpProvider_Lock(thiz->provider);

        do
        {
            ret = UpnpProvider_AddObserver(thiz->provider, "UpnpEventPublisher", NULL, NULL, OnServiceChanged, thiz);
            if (RET_FAILED(ret))
            {
                LOG_E(TAG, "UpnpProvider_AddObserver failed");
                break;
            }
        } while (0);

        UpnpProvider_Unlock(thiz->provider);
    } while (0);

    return ret;
}

void UpnpEventPublisher_Dispose(UpnpEventPublisher *thiz)
{
    RETURN_IF_FAIL(thiz);

    UpnpProvider_Lock(thiz->provider);

    do
    {
        if (RET_FAILED(UpnpProvider_RemoveObserver(thiz->provider, "UpnpEventPublisher")))
        {
            LOG_E(TAG, "UpnpProvider_RemoveObserver failed");
            break;
        }
    } while (0);

    UpnpProvider_Unlock(thiz->provider);

    if (RET_FAILED(UpnpHttpServer_UnregisterSubscribeHandler(&thiz->http->server)))
    {
        LOG_E(TAG, "UpnpHttpServer_UnregisterSubscriberHandler: failed");
    }

    if (RET_FAILED(UpnpHttpServer_UnregisterUnsubscribeHandler(&thiz->http->server)))
    {
        LOG_E(TAG, "UpnpHttpServer_UnregisterUnsubscriberHandler: failed");
    }

    thiz->http = NULL;
    thiz->provider = NULL;
}

void UpnpEventPublisher_Delete(UpnpEventPublisher *thiz)
{
    RETURN_IF_FAIL(thiz);

    UpnpEventPublisher_Dispose(thiz);
    tiny_free(thiz);
}