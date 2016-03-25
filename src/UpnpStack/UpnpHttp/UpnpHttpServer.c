/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   UpnpHttpServer.c
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#include "UpnpHttpServer.h"
#include "tiny_memory.h"
#include "tiny_log.h"
#include "HttpMessage.h"
#include "upnp_define.h"

#define TAG         "UpnpHttpServer"

static void conn_listener(TcpConn *conn, void *ctx);
static TinyRet conn_recv_once(UpnpHttpServer *thiz, TcpConn *conn);
static TinyRet conn_recv_http_msg(UpnpHttpServer *thiz, TcpConn *conn, HttpMessage *msg, uint32_t timeout);
static void doGet(UpnpHttpServer *thiz, UpnpHttpConnection *conn, HttpMessage *request);
static void doPost(UpnpHttpServer *thiz, UpnpHttpConnection *conn, HttpMessage *request);
static void doNotify(UpnpHttpServer *thiz, UpnpHttpConnection *conn, HttpMessage *request);
static void doSubscribe(UpnpHttpServer *thiz, UpnpHttpConnection *conn, HttpMessage *request);
static void doUnsubscribe(UpnpHttpServer *thiz, UpnpHttpConnection *conn, HttpMessage *request);

UpnpHttpServer * UpnpHttpServer_New(void)
{
    UpnpHttpServer *thiz = NULL;

    do
    {
        TinyRet ret = TINY_RET_OK;

        thiz = (UpnpHttpServer *)tiny_malloc(sizeof(UpnpHttpServer));
        if (thiz == NULL)
        {
            break;
        }

        ret = UpnpHttpServer_Construct(thiz);
        if (RET_FAILED(ret))
        {
            UpnpHttpServer_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

TinyRet UpnpHttpServer_Construct(UpnpHttpServer *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(UpnpHttpServer));

        ret = TcpServer_Construct(&thiz->server);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "TcpServer_Construct failed");
            break;
        }

        thiz->OnGet = NULL;
        thiz->OnPost = NULL;
        thiz->OnNotify = NULL;
        thiz->OnSubscribe = NULL;
        thiz->OnUnsubscribe = NULL;
    } while (0);

    return ret;
}

void UpnpHttpServer_Dispose(UpnpHttpServer *thiz)
{
    RETURN_IF_FAIL(thiz);

    UpnpHttpServer_Stop(thiz);
    TcpServer_Dispose(&thiz->server);
}

void UpnpHttpServer_Delete(UpnpHttpServer *thiz)
{
    RETURN_IF_FAIL(thiz);

    UpnpHttpServer_Dispose(thiz);
    tiny_free(thiz);
}

TinyRet UpnpHttpServer_RegisterGetHandler(UpnpHttpServer *thiz, UpnpGetHandler handler, void *ctx)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(handler, TINY_RET_E_ARG_NULL);

    thiz->OnGet = handler;
    thiz->OnGetCtx = ctx;

    return TINY_RET_OK;
}

TinyRet UpnpHttpServer_RegisterPostHandler(UpnpHttpServer *thiz, UpnpPostHandler handler, void *ctx)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(handler, TINY_RET_E_ARG_NULL);

    thiz->OnPost = handler;
    thiz->OnPostCtx = ctx;

    return TINY_RET_OK;
}

TinyRet UpnpHttpServer_RegisterNotifyHandler(UpnpHttpServer *thiz, UpnpNotifyHandler handler, void *ctx)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(handler, TINY_RET_E_ARG_NULL);

    thiz->OnNotify = handler;
    thiz->OnNotifyCtx = ctx;

    return TINY_RET_OK;
}

TinyRet UpnpHttpServer_RegisterSubscribeHandler(UpnpHttpServer *thiz, UpnpSubscribeHandler handler, void *ctx)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(handler, TINY_RET_E_ARG_NULL);

    thiz->OnSubscribe = handler;
    thiz->OnSubscribeCtx = ctx;

    return TINY_RET_OK;
}

TinyRet UpnpHttpServer_RegisterUnsubscribeHandler(UpnpHttpServer *thiz, UpnpUnsubscribeHandler handler, void *ctx)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(handler, TINY_RET_E_ARG_NULL);

    thiz->OnUnsubscribe = handler;
    thiz->OnUnsubscribeCtx = ctx;

    return TINY_RET_OK;
}

TinyRet UpnpHttpServer_UnregisterGetHandler(UpnpHttpServer *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    thiz->OnGet = NULL;
    thiz->OnGetCtx = NULL;

    return TINY_RET_OK;
}

TinyRet UpnpHttpServer_UnregisterPostHandler(UpnpHttpServer *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    thiz->OnPost = NULL;
    thiz->OnPostCtx = NULL;

    return TINY_RET_OK;
}

TinyRet UpnpHttpServer_UnregisterNotifyHandler(UpnpHttpServer *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    thiz->OnNotify = NULL;
    thiz->OnNotifyCtx = NULL;

    return TINY_RET_OK;
}

TinyRet UpnpHttpServer_UnregisterSubscribeHandler(UpnpHttpServer *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    thiz->OnSubscribe = NULL;
    thiz->OnSubscribeCtx = NULL;

    return TINY_RET_OK;
}

TinyRet UpnpHttpServer_UnregisterUnsubscribeHandler(UpnpHttpServer *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    thiz->OnUnsubscribe = NULL;
    thiz->OnUnsubscribeCtx = NULL;

    return TINY_RET_OK;
}

TinyRet UpnpHttpServer_Start(UpnpHttpServer *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    return TcpServer_Start(&thiz->server, 0, conn_listener, thiz);
}

TinyRet UpnpHttpServer_Stop(UpnpHttpServer *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    return TcpServer_Stop(&thiz->server);
}

bool UpnpHttpServer_IsRunning(UpnpHttpServer *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, false);

    return TcpServer_IsRunning(&thiz->server);
}

uint16_t UpnpHttpServer_GetListeningPort(UpnpHttpServer *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, 0);

    return TcpServer_GetListenPort(&thiz->server);
}

static void conn_listener(TcpConn *conn, void *ctx)
{
    UpnpHttpServer *thiz = (UpnpHttpServer *)ctx;

    while (true)
    {
        TinyRet ret = conn_recv_once(thiz, conn);

        if (RET_FAILED(ret))
        {
            break;
        }
    }
}

static TinyRet conn_recv_once(UpnpHttpServer *thiz, TcpConn *conn)
{
    TinyRet ret = TINY_RET_OK;

    do
    {
        HttpMessage *request = HttpMessage_New();
        if (request == NULL)
        {
            LOG_E(TAG, "HttpMessage_New failed");
            ret = TINY_RET_E_NEW;
            break;
        }

        do
        {
            UpnpHttpConnection httpConn;

            ret = conn_recv_http_msg(thiz, conn, request, UPNP_TIMEOUT);
            if (RET_FAILED(ret))
            {
                break;
            }

            if (HttpMessage_GetType(request) == HTTP_RESPONSE)
            {
                ret = TINY_RET_E_HTTP_MSG_INVALID;
                break;
            }

            ret = UpnpHttpConnection_Construct(&httpConn, conn);
            if (RET_FAILED(ret))
            {
                break;
            }

            do
            {
                if (STR_EQUAL(HttpMessage_GetMethod(request), "GET"))
                {
                    doGet(thiz, &httpConn, request);
                    break;
                }

                if (STR_EQUAL(HttpMessage_GetMethod(request), "POST"))
                {
                    doPost(thiz, &httpConn, request);
                    break;
                }

                if (STR_EQUAL(HttpMessage_GetMethod(request), "NOTIFY"))
                {
                    doNotify(thiz, &httpConn, request);
                    break;
                }

                if (STR_EQUAL(HttpMessage_GetMethod(request), "SUBSCRIBE"))
                {
                    doSubscribe(thiz, &httpConn, request);
                    break;
                }

                if (STR_EQUAL(HttpMessage_GetMethod(request), "UNSUBSCRIBE"))
                {
                    doUnsubscribe(thiz, &httpConn, request);
                    break;
                }
            } while (0);

            UpnpHttpConnection_Dispose(&httpConn);
        } while (0);

        HttpMessage_Delete(request);
    } while (0);

    TcpConn_Disconnect(conn);

    return ret;
}

static TinyRet conn_recv_http_msg(UpnpHttpServer *thiz, TcpConn *conn, HttpMessage *msg, uint32_t timeout)
{
    LOG_TIME_BEGIN(TAG, conn_recv_http_msg);
    TinyRet ret = TINY_RET_OK;
    char *bytes = NULL;
    uint32_t size = 0;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(conn, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(msg, TINY_RET_E_ARG_NULL);

    do
    {
        ret = TcpConn_Recv(conn, &bytes, &size, timeout);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = HttpMessage_Parse(msg, bytes, size);
        if (RET_FAILED(ret))
        {
            break;
        }

        if (HttpMessage_GetContentSize(msg) == 0)
        {
            break;
        }

        if (HttpMessage_IsContentFull(msg))
        {
            break;
        }

        while (1)
        {
            tiny_free(bytes);
            bytes = NULL;
            size = 0;

            ret = TcpConn_Recv(conn, &bytes, &size, timeout);
            if (RET_FAILED(ret))
            {
                break;
            }

            ret = HttpMessage_AddContentObject(msg, bytes, size);
            if (RET_FAILED(ret))
            {
                break;
            }

            if (HttpMessage_IsContentFull(msg))
            {
                break;
            }
        }
    } while (0);

    if (bytes != NULL)
    {
        tiny_free(bytes);
    }

    LOG_TIME_END(TAG, conn_recv_http_msg);

    return ret;
}

static void doGet(UpnpHttpServer *thiz, UpnpHttpConnection *conn, HttpMessage *request)
{
    do
    {
        if (thiz->OnGet == NULL)
        {
            UpnpHttpConnection_SendError(conn, 404, "FILE NOT FOUND");
            break;
        }

        thiz->OnGet(conn, HttpMessage_GetUri(request), thiz->OnGetCtx);
    } while (0);
}

static void doPost(UpnpHttpServer *thiz, UpnpHttpConnection *conn, HttpMessage *request)
{
    do
    {
        if (thiz->OnPost == NULL)
        {
            UpnpHttpConnection_SendError(conn, 404, "FILE NOT FOUND");
            break;
        }

        thiz->OnPost(conn,
            HttpMessage_GetUri(request),
            HttpMessage_GetHeaderValue(request, "Soapaction"),
            HttpMessage_GetContentObject(request),
            HttpMessage_GetContentSize(request),
            thiz->OnPostCtx);
    } while (0);
}

static void doNotify(UpnpHttpServer *thiz, UpnpHttpConnection *conn, HttpMessage *request)
{
    do
    {
        if (thiz->OnNotify == NULL)
        {
            UpnpHttpConnection_SendError(conn, 404, "FILE NOT FOUND");
            break;
        }

        thiz->OnNotify(conn,
            HttpMessage_GetUri(request),
            HttpMessage_GetHeaderValue(request, "NT"),
            HttpMessage_GetHeaderValue(request, "NTS"),
            HttpMessage_GetHeaderValue(request, "SID"),
            HttpMessage_GetHeaderValue(request, "SEQ"),
            HttpMessage_GetContentObject(request),
            HttpMessage_GetContentSize(request),
            thiz->OnNotifyCtx);
    } while (0);
}

static void doSubscribe(UpnpHttpServer *thiz, UpnpHttpConnection *conn, HttpMessage *request)
{
    do
    {
        if (thiz->OnSubscribe == NULL)
        {
            UpnpHttpConnection_SendError(conn, 404, "FILE NOT FOUND");
            break;
        }

        thiz->OnSubscribe(conn,
            HttpMessage_GetUri(request),
            HttpMessage_GetHeaderValue(request, "CALLBACK"),
            HttpMessage_GetHeaderValue(request, "NT"),
            HttpMessage_GetHeaderValue(request, "TIMEOUT"),
            thiz->OnSubscribeCtx);
    } while (0);
}

static void doUnsubscribe(UpnpHttpServer *thiz, UpnpHttpConnection *conn, HttpMessage *request)
{
    do
    {
        if (thiz->OnUnsubscribe == NULL)
        {
            UpnpHttpConnection_SendError(conn, 404, "FILE NOT FOUND");
            break;
        }

        thiz->OnUnsubscribe(conn,
            HttpMessage_GetUri(request),
            HttpMessage_GetHeaderValue(request, "SID"),
            thiz->OnUnsubscribeCtx);
    } while (0);
}