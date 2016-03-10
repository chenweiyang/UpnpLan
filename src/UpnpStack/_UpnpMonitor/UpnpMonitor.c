/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   UpnpMonitor.c
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/


#include "UpnpMonitor.h"
#include "upnp_define.h"
#include "ct_memory.h"
#include "ct_log.h"
#include "str_get_value.h"
#include "CtTcpServer.h"
#include "CtWorker.h"
#include "HttpMessage.h"
#include "HttpClient.h"
#include "TinyXml.h"
#include "UpnpEvent.h"
#include "UpnpService.h"
#include "UpnpServiceDefinition.h"
#include "UpnpDevice.h"
#include "UpnpDeviceDefinition.h"

#define TAG                 "UpnpMonitor"

static CtRet UpnpMonitor_Construct(UpnpMonitor *thiz);
static CtRet UpnpMonitor_Dispose(UpnpMonitor *thiz);

static void conn_listener(CtTcpConn *conn, void *ctx);
static CtRet conn_recv_http_msg(UpnpMonitor *thiz, CtTcpConn *conn, HttpMessage *msg, uint32_t timeout);

struct _UpnpMonitor
{
    CtTcpServer                 httpServer;
    UpnpSubscription          * subscription;
};

UpnpMonitor * UpnpMonitor_New(void)
{
    UpnpMonitor *thiz = NULL;

    do
    {
        CtRet ret = CT_RET_OK;

        thiz = (UpnpMonitor *)ct_malloc(sizeof(UpnpMonitor));
        if (thiz == NULL)
        {
            break;
        }

        ret = UpnpMonitor_Construct(thiz);
        if (RET_FAILED(ret))
        {
            UpnpMonitor_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

CtRet UpnpMonitor_Construct(UpnpMonitor *thiz)
{
    CtRet ret = CT_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, CT_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(UpnpMonitor));
        ret = CtTcpServer_Construct(&thiz->httpServer);
        if (RET_FAILED(ret))
        {
            break;
        }
    } while (0);

    return ret;
}

CtRet UpnpMonitor_Dispose(UpnpMonitor *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, CT_RET_E_ARG_NULL);

    CtTcpServer_Dispose(&thiz->httpServer);

    if (thiz->subscription != NULL)
    {
        ct_free(thiz->subscription);
    }

    return CT_RET_OK;
}

void UpnpMonitor_Delete(UpnpMonitor *thiz)
{
    RETURN_IF_FAIL(thiz);
    UpnpMonitor_Dispose(thiz);
    ct_free(thiz);
}

/*
HTTP/1.1 200 OK
Server: Microsoft-Windows-NT/5.1 UPnP/1.0 UPnP-Device-Host/1.0 Microsoft-HTTPAPI/2.0
Timeout: Second-300
SID: uuid:f8343300-c3e2-41ec-91f0-ceac669d6c7c
Date: Thu, 30 Oct 2014 07:38:06 GMT
Content-Length: 0
*/
CtRet UpnpMonitor_Subscribe(UpnpMonitor *thiz, UpnpSubscription *subscription, UpnpError *error)
{
    LOG_TIME_BEGIN(TAG, UpnpMonitor_Subscribe);

    CtRet ret = CT_RET_OK;
    HttpClient *client = NULL;
    HttpMessage *httpRequest = NULL;
    HttpMessage *httpResponse = NULL;

    RETURN_VAL_IF_FAIL(thiz, CT_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(subscription, CT_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(error, CT_RET_E_ARG_NULL);

    do
    {
        UpnpDevice *device = NULL;
        const char *urlbase = NULL;
        const char *eventSubUrl = NULL;
        char sub_url[CT_URL_LEN];
        char cb_url[CT_URL_LEN];

        memset(sub_url, 0, CT_URL_LEN);
        memset(cb_url, 0, CT_URL_LEN);

        if (thiz->httpServer.running)
        {
            ret = CT_RET_E_STARTED;
            break;
        }

        ret = CtTcpServer_Start(&thiz->httpServer, 0, conn_listener, thiz);
        if (RET_FAILED(ret))
        {
            break;
        }

        if (thiz->subscription != NULL)
        {
            ret = CT_RET_E_UPNP_SUBSCRIBE_FAILED;
            break;
        }

        device = (UpnpDevice *)UpnpService_GetParentDevice(subscription->service);
        urlbase = UpnpDevice_GetPropertyValue(device, UPNP_DEVICE_URLBase);
        eventSubUrl = UpnpService_GetPropertyValue(subscription->service, UPNP_SERVICE_EventSubURL);

        ct_snprintf(sub_url, CT_URL_LEN, "%s%s", urlbase, eventSubUrl);

        httpRequest = HttpMessage_New();
        if (httpRequest == NULL)
        {
            ret = CT_RET_E_NEW;
            break;
        }

        ct_snprintf(cb_url, CT_URL_LEN, "<http://%s:%d%s>",
            "10.0.1.3",
            CtTcpServer_GetListenPort(&thiz->httpServer),
            eventSubUrl);

        HttpMessage_SetRequest(httpRequest, "SUBSCRIBE", sub_url);
        HttpMessage_SetHeader(httpRequest, "User-Agent", UPNP_STACK_INFO);
        HttpMessage_SetHeader(httpRequest, "CALLBACK", cb_url);
        HttpMessage_SetHeader(httpRequest, "NT", "upnp:event");
        HttpMessage_SetHeader(httpRequest, "TIMEOUT", "Second-infinite");

        client = HttpClient_New();
        if (client == NULL)
        {
            ret = CT_RET_E_NEW;
            break;
        }

        ret = HttpClient_Execute(client, httpRequest, httpResponse, UPNP_TIMEOUT);
        if (RET_FAILED(ret))
        {
            break;
        }

        if (HttpMessage_GetStatusCode(httpResponse) != HTTP_STATUS_OK)
        {
            ret = CT_RET_E_HTTP_STATUS;
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

        thiz->subscription = (UpnpSubscription *)ct_malloc(sizeof(UpnpSubscription));
        if (thiz->subscription == NULL)
        {
            ret = CT_RET_E_OUT_OF_MEMORY;
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

    LOG_TIME_END(TAG, UpnpMonitor_Subscribe);

    return ret;
}

CtRet UpnpMonitor_Unsubscribe(UpnpMonitor *thiz, UpnpError *error)
{
    LOG_TIME_BEGIN(TAG, UpnpMonitor_Unsubscribe);

    CtRet ret = CT_RET_OK;
    HttpClient *client = NULL;
    HttpMessage *httpRequest = NULL;
    HttpMessage *httpResponse = NULL;

    RETURN_VAL_IF_FAIL(thiz, CT_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(subscription, CT_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(error, CT_RET_E_ARG_NULL);

    do
    {
        UpnpDevice *device = NULL;
        const char *urlbase = NULL;
        const char *eventSubUrl = NULL;
        char sub_url[CT_URL_LEN];
        char cb_url[CT_URL_LEN];

        memset(sub_url, 0, CT_URL_LEN);
        memset(cb_url, 0, CT_URL_LEN);

        if (!thiz->httpServer.running)
        {
            ret = CT_RET_E_STOPPED;
            break;
        }

        ret = CtTcpServer_Stop(&thiz->httpServer);
        if (RET_FAILED(ret))
        {
            break;
        }

        if (thiz->subscription == NULL)
        {
            ret = CT_RET_E_UPNP_UNSUBSCRIBE_FAILED;
            break;
        }

        device = (UpnpDevice *)UpnpService_GetParentDevice(thiz->subscription->service);
        urlbase = UpnpDevice_GetPropertyValue(device, UPNP_DEVICE_URLBase);
        eventSubUrl = UpnpService_GetPropertyValue(thiz->subscription->service, UPNP_SERVICE_EventSubURL);

        ct_snprintf(sub_url, CT_URL_LEN, "%s%s", urlbase, eventSubUrl);

        httpRequest = HttpMessage_New();
        if (httpRequest == NULL)
        {
            ret = CT_RET_E_NEW;
            break;
        }

        ct_snprintf(cb_url, CT_URL_LEN, "<http://%s:%d%s>",
            "10.0.1.3",
            CtTcpServer_GetListenPort(&thiz->httpServer),
            eventSubUrl);

        HttpMessage_SetRequest(httpRequest, "UNSUBSCRIBE", sub_url);
        HttpMessage_SetHeader(httpRequest, "User-Agent", UPNP_STACK_INFO);
        HttpMessage_SetHeader(httpRequest, "SID", thiz->subscription->subscribeId);

        client = HttpClient_New();
        if (client == NULL)
        {
            ret = CT_RET_E_NEW;
            break;
        }

        ret = HttpClient_Execute(client, httpRequest, httpResponse, UPNP_TIMEOUT);
        if (RET_FAILED(ret))
        {
            break;
        }

        if (HttpMessage_GetStatusCode(httpResponse) != HTTP_STATUS_OK)
        {
            ret = CT_RET_E_HTTP_STATUS;
            error->code = HttpMessage_GetStatusCode(httpResponse);
            break;
        }

        ct_free(thiz->subscription);
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

    LOG_TIME_END(TAG, UpnpMonitor_Unsubscribe);

    return ret;
}

static void conn_listener(CtTcpConn *conn, void *ctx)
{
    CtRet ret = CT_RET_OK;
    UpnpMonitor *thiz = (UpnpMonitor *)ctx;
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
            ret = CT_RET_E_NEW;
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
            ret = CT_RET_E_NEW;
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
                ct_free(bytes);
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

static CtRet conn_recv_http_msg(UpnpMonitor *thiz, CtTcpConn *conn, HttpMessage *msg, uint32_t timeout)
{
    LOG_TIME_BEGIN(TAG, conn_recv_http_msg);
    CtRet ret = CT_RET_OK;
    char *bytes = NULL;
    uint32_t size = 0;

    RETURN_VAL_IF_FAIL(thiz, CT_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(conn, CT_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(msg, CT_RET_E_ARG_NULL);

    do
    {
        ret = CtTcpConn_Recv(conn, &bytes, &size, timeout);
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
            ct_free(bytes);
            bytes = NULL;
            size = 0;

            ret = CtTcpConn_Recv(conn, &bytes, &size, timeout);
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
        ct_free(bytes);
    }

    LOG_TIME_END(TAG, conn_recv_http_msg);

    return ret;
}