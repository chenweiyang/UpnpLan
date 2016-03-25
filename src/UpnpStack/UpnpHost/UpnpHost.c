/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   UpnpHost.c
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#include "UpnpHost.h"
#include "tiny_memory.h"
#include "tiny_log.h"

#define TAG     "UpnpHost"


static void OnGet(UpnpHttpConnection *conn, const char *uri, void *ctx);
static void OnPost(UpnpHttpConnection *conn, const char *uri, const char *soapAction, const char *content, uint32_t contentLength, void *ctx);
static void OnSubscribe(UpnpHttpConnection *conn, const char *uri, const char *callback, const char *nt, const char *timeout, void *ctx);
static void OnUnsubscribe(UpnpHttpConnection *conn, const char *uri, const char *sid, void *ctx);
static void OnServiceChanged(UpnpService *service, void *ctx);

UpnpHost * UpnpHost_New(UpnpHttpManager *http, UpnpProvider *provider)
{
    UpnpHost *thiz = NULL;

    do
    {
        TinyRet ret = TINY_RET_OK;

        thiz = (UpnpHost *)tiny_malloc(sizeof(UpnpHost));
        if (thiz == NULL)
        {
            break;
        }

        ret = UpnpHost_Construct(thiz, http, provider);
        if (RET_FAILED(ret))
        {
            UpnpHost_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

TinyRet UpnpHost_Construct(UpnpHost *thiz, UpnpHttpManager *http, UpnpProvider *provider)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(UpnpHost));
        thiz->http = http;
        thiz->provider = provider;

        ret = UpnpHttpServer_RegisterGetHandler(&http->server, OnGet, thiz);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "UpnpHttpServer_RegisterGetHandler: failed");
            break;
        }

        ret = UpnpHttpServer_RegisterPostHandler(&http->server, OnPost, thiz);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "UpnpHttpServer_RegisterPostHandler: failed");
            break;
        }

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
    } while (0);

    return ret;
}

void UpnpHost_Dispose(UpnpHost *thiz)
{
    RETURN_IF_FAIL(thiz);

    UpnpHost_Stop(thiz);
    thiz->http = NULL;
    thiz->provider = NULL;
}

void UpnpHost_Delete(UpnpHost *thiz)
{
    RETURN_IF_FAIL(thiz);

    UpnpHost_Dispose(thiz);
    tiny_free(thiz);
}

TinyRet UpnpHost_Start(UpnpHost *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    UpnpProvider_Lock(thiz->provider);

    do
    {
        ret = UpnpProvider_AddObserver(thiz->provider, "Host", NULL, NULL, OnServiceChanged, thiz);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "UpnpProvider_AddObserver failed");
            break;
        }
    } while (0);

    UpnpProvider_Unlock(thiz->provider);

    return ret;
}

TinyRet UpnpHost_Stop(UpnpHost *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    UpnpProvider_Lock(thiz->provider);

    do
    {
        ret = UpnpProvider_RemoveObserver(thiz->provider, "Host");
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "UpnpProvider_RemoveObserver failed");
            break;
        }
    } while (0);

    UpnpProvider_Unlock(thiz->provider);

    return ret;
}

static void OnGet(UpnpHttpConnection *conn, const char *uri, void *ctx)
{
    LOG_D(TAG, "OnGet: %s", uri);

    UpnpHttpConnection_SendError(conn, 404, "NOT FOUND");
}

static void OnPost(UpnpHttpConnection *conn, const char *uri, const char *soapAction, const char *content, uint32_t contentLength, void *ctx)
{
    LOG_D(TAG, "OnPost: %s", uri);

    UpnpHttpConnection_SendError(conn, 404, "NOT FOUND");
}

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
}