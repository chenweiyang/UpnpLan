/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   UpnpSubscriber.c
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/


#include "UpnpSubscriber.h"
#include "upnp_define.h"
#include "tiny_memory.h"
#include "tiny_log.h"
#include "tiny_str_get_value.h"
#include "TcpServer.h"
#include "TinyWorker.h"
#include "HttpMessage.h"
#include "HttpClient.h"
#include "TinyXml.h"
#include "UpnpEvent.h"
#include "UpnpService.h"
#include "UpnpServiceDefinition.h"
#include "UpnpDevice.h"
#include "UpnpDeviceDefinition.h"

#define TAG                 "UpnpSubscriber"

static TinyRet UpnpSubscriber_Construct(UpnpSubscriber *thiz);
static TinyRet UpnpSubscriber_Dispose(UpnpSubscriber *thiz);

static void conn_listener(TcpConn *conn, void *ctx);
static TinyRet conn_recv_http_msg(UpnpSubscriber *thiz, TcpConn *conn, HttpMessage *msg, uint32_t timeout);

struct _UpnpSubscriber
{
    TcpServer                   httpServer;
    UpnpSubscription          * subscription;
};

UpnpSubscriber * UpnpSubscriber_New(void)
{
    UpnpSubscriber *thiz = NULL;

    do
    {
        TinyRet ret = TINY_RET_OK;

        thiz = (UpnpSubscriber *)tiny_malloc(sizeof(UpnpSubscriber));
        if (thiz == NULL)
        {
            break;
        }

        ret = UpnpSubscriber_Construct(thiz);
        if (RET_FAILED(ret))
        {
            UpnpSubscriber_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

TinyRet UpnpSubscriber_Construct(UpnpSubscriber *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(UpnpSubscriber));
        ret = TcpServer_Construct(&thiz->httpServer);
        if (RET_FAILED(ret))
        {
            break;
        }
    } while (0);

    return ret;
}

TinyRet UpnpSubscriber_Dispose(UpnpSubscriber *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    TcpServer_Dispose(&thiz->httpServer);

    if (thiz->subscription != NULL)
    {
        tiny_free(thiz->subscription);
    }

    return TINY_RET_OK;
}

void UpnpSubscriber_Delete(UpnpSubscriber *thiz)
{
    RETURN_IF_FAIL(thiz);
    UpnpSubscriber_Dispose(thiz);
    tiny_free(thiz);
}

/*
HTTP/1.1 200 OK
Server: Microsoft-Windows-NT/5.1 UPnP/1.0 UPnP-Device-Host/1.0 Microsoft-HTTPAPI/2.0
Timeout: Second-300
SID: uuid:f8343300-c3e2-41ec-91f0-ceac669d6c7c
Date: Thu, 30 Oct 2014 07:38:06 GMT
Content-Length: 0
*/
TinyRet UpnpSubscriber_Subscribe(UpnpSubscriber *thiz, UpnpSubscription *subscription, UpnpError *error)
{
    LOG_TIME_BEGIN(TAG, UpnpSubscriber_Subscribe);

    TinyRet ret = TINY_RET_OK;
    HttpClient *client = NULL;
    HttpMessage *httpRequest = NULL;
    HttpMessage *httpResponse = NULL;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(subscription, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(error, TINY_RET_E_ARG_NULL);

    do
    {
        UpnpDevice *device = NULL;
        const char *urlbase = NULL;
        const char *eventSubUrl = NULL;
        char sub_url[TINY_URL_LEN];
        char cb_url[TINY_URL_LEN];

        memset(sub_url, 0, TINY_URL_LEN);
        memset(cb_url, 0, TINY_URL_LEN);

        if (thiz->httpServer.running)
        {
            ret = TINY_RET_E_STARTED;
            break;
        }

        ret = TcpServer_Start(&thiz->httpServer, 0, conn_listener, thiz);
        if (RET_FAILED(ret))
        {
            break;
        }

        if (thiz->subscription != NULL)
        {
            ret = TINY_RET_E_UPNP_SUBSCRIBE_FAILED;
            break;
        }

        device = (UpnpDevice *)UpnpService_GetParentDevice(subscription->service);
        urlbase = UpnpDevice_GetPropertyValue(device, UPNP_DEVICE_URLBase);
        eventSubUrl = UpnpService_GetPropertyValue(subscription->service, UPNP_SERVICE_EventSubURL);

        tiny_snprintf(sub_url, TINY_URL_LEN, "%s%s", urlbase, eventSubUrl);

        httpRequest = HttpMessage_New();
        if (httpRequest == NULL)
        {
            ret = TINY_RET_E_NEW;
            break;
        }

        tiny_snprintf(cb_url, TINY_URL_LEN, "<http://%s:%d%s>",
            "10.0.1.3",
            TcpServer_GetListenPort(&thiz->httpServer),
            eventSubUrl);

        HttpMessage_SetRequest(httpRequest, "SUBSCRIBE", sub_url);
        HttpMessage_SetHeader(httpRequest, "User-Agent", UPNP_STACK_INFO);
        HttpMessage_SetHeader(httpRequest, "CALLBACK", cb_url);
        HttpMessage_SetHeader(httpRequest, "NT", "upnp:event");
        HttpMessage_SetHeader(httpRequest, "TIMEOUT", "Second-infinite");

        client = HttpClient_New();
        if (client == NULL)
        {
            ret = TINY_RET_E_NEW;
            break;
        }

        ret = HttpClient_Execute(client, httpRequest, httpResponse, UPNP_TIMEOUT);
        if (RET_FAILED(ret))
        {
            break;
        }

        if (HttpMessage_GetStatusCode(httpResponse) != HTTP_STATUS_OK)
        {
            ret = TINY_RET_E_HTTP_STATUS;
            error->code = HttpMessage_GetStatusCode(httpResponse);
            break;
        }

        const char *value = HttpMessage_GetHeaderValue(httpResponse, "Timeout");
        const char *sid = HttpMessage_GetHeaderValue(httpResponse, "SID");
        if (value != NULL)
        {
            char v[16];
            int ret = 0;

            memset(v, 0, 16);
            ret = str_get_value(value, strlen(value), "Second-", NULL, v, 16);
            if (ret > 0)
            {
                subscription->timeout = atoi(v);
            }
        }

        strncpy(subscription->subscribeId, sid, UPNP_UUID_LEN);

        thiz->subscription = (UpnpSubscription *)tiny_malloc(sizeof(UpnpSubscription));
        if (thiz->subscription == NULL)
        {
            ret = TINY_RET_E_OUT_OF_MEMORY;
            break;
        }

        memcpy(thiz->subscription, subscription, sizeof(UpnpSubscription));
    } while (0);

    if (httpRequest != NULL)
    {
        HttpMessage_Delete(httpRequest);
    }

    if (httpResponse != NULL)
    {
        HttpMessage_Delete(httpResponse);
    }
    
    if (client != NULL)
    {
        HttpClient_Delete(client);
    }

    LOG_TIME_END(TAG, UpnpSubscriber_Subscribe);

    return ret;
}

TinyRet UpnpSubscriber_Unsubscribe(UpnpSubscriber *thiz, UpnpError *error)
{
    LOG_TIME_BEGIN(TAG, UpnpSubscriber_Unsubscribe);

    TinyRet ret = TINY_RET_OK;
    HttpClient *client = NULL;
    HttpMessage *httpRequest = NULL;
    HttpMessage *httpResponse = NULL;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(error, TINY_RET_E_ARG_NULL);

    do
    {
        UpnpDevice *device = NULL;
        const char *urlbase = NULL;
        const char *eventSubUrl = NULL;
        char sub_url[TINY_URL_LEN];
        char cb_url[TINY_URL_LEN];

        memset(sub_url, 0, TINY_URL_LEN);
        memset(cb_url, 0, TINY_URL_LEN);

        if (!thiz->httpServer.running)
        {
            ret = TINY_RET_E_STOPPED;
            break;
        }

        ret = TcpServer_Stop(&thiz->httpServer);
        if (RET_FAILED(ret))
        {
            break;
        }

        if (thiz->subscription == NULL)
        {
            ret = TINY_RET_E_UPNP_UNSUBSCRIBE_FAILED;
            break;
        }

        device = (UpnpDevice *)UpnpService_GetParentDevice(thiz->subscription->service);
        urlbase = UpnpDevice_GetPropertyValue(device, UPNP_DEVICE_URLBase);
        eventSubUrl = UpnpService_GetPropertyValue(thiz->subscription->service, UPNP_SERVICE_EventSubURL);

        tiny_snprintf(sub_url, TINY_URL_LEN, "%s%s", urlbase, eventSubUrl);

        httpRequest = HttpMessage_New();
        if (httpRequest == NULL)
        {
            ret = TINY_RET_E_NEW;
            break;
        }

        tiny_snprintf(cb_url, TINY_URL_LEN, "<http://%s:%d%s>",
            "10.0.1.3",
            TcpServer_GetListenPort(&thiz->httpServer),
            eventSubUrl);

        HttpMessage_SetRequest(httpRequest, "UNSUBSCRIBE", sub_url);
        HttpMessage_SetHeader(httpRequest, "User-Agent", UPNP_STACK_INFO);
        HttpMessage_SetHeader(httpRequest, "SID", thiz->subscription->subscribeId);

        client = HttpClient_New();
        if (client == NULL)
        {
            ret = TINY_RET_E_NEW;
            break;
        }

        ret = HttpClient_Execute(client, httpRequest, httpResponse, UPNP_TIMEOUT);
        if (RET_FAILED(ret))
        {
            break;
        }

        if (HttpMessage_GetStatusCode(httpResponse) != HTTP_STATUS_OK)
        {
            ret = TINY_RET_E_HTTP_STATUS;
            error->code = HttpMessage_GetStatusCode(httpResponse);
            break;
        }

        tiny_free(thiz->subscription);
        thiz->subscription = NULL;
    } while (0);

    if (httpRequest != NULL)
    {
        HttpMessage_Delete(httpRequest);
    }

    if (httpResponse != NULL)
    {
        HttpMessage_Delete(httpResponse);
    }

    if (client != NULL)
    {
        HttpClient_Delete(client);
    }

    LOG_TIME_END(TAG, UpnpSubscriber_Unsubscribe);

    return ret;
}

static void conn_listener(TcpConn *conn, void *ctx)
{
    TinyRet ret = TINY_RET_OK;
    UpnpSubscriber *thiz = (UpnpSubscriber *)ctx;
    HttpMessage *request = NULL;
    UpnpEvent *event = NULL;

    do
    {
        if (thiz->subscription == NULL)
        {
            break;
        }

        request = HttpMessage_New();
        if (request == NULL)
        {
            ret = TINY_RET_E_NEW;
            return;
        }

        ret = conn_recv_http_msg(thiz, conn, request, UPNP_TIMEOUT);
        if (RET_FAILED(ret))
        {
            break;
        }

        event = UpnpEvent_New();
        if (event == NULL)
        {
            ret = TINY_RET_E_NEW;
            break;
        }

        ret = UpnpEvent_Parse(event, request);
        if (RET_FAILED(ret))
        {
            break;
        }

        thiz->subscription->listener(event, thiz->subscription->ctx);
#if 0
        {
            char *bytes = NULL;
            uint32_t size = 0;
            ret = HttpMessage_ToBytes(&msg, &bytes, &size);
            if (RET_SUSCEEDED(ret))
            {
                printf("------------ event -----------\n");
                printf("%s", bytes);
            }

            if (bytes != NULL)
            {
                tiny_free(bytes);
            }
        }
#endif

    } while (0);

    if (request != NULL)
    {
        HttpMessage_Delete(request);
    }
    
    if (event != NULL)
    {
        UpnpEvent_Delete(event);
    }
}

static TinyRet conn_recv_http_msg(UpnpSubscriber *thiz, TcpConn *conn, HttpMessage *msg, uint32_t timeout)
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