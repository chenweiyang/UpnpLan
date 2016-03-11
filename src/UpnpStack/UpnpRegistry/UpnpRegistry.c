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
#include "UpnpRegistryCore.h"
#include "HttpMessage.h"

#define TAG                 "UpnpRegistry"

/**
* Presence
* <p> NOTIFY * HTTP/1.1 </p>
* <p> HOST: 239.255.255.250:1900 </p>
* <p> CACHE-CONTROL: max-age=[Message lifetime] </p>
* <p> LOCATION: [URL for the Device Description Document] </p>
* <p> AL: <[application-specific URI]><[application-specific URI]> -> option </p>
* <p> NT: [URI indicating the type of presence advertisement] </p>
* <p> NTS: ssdp:alive </p>
* <p> SERVER: [OS and version] UPnP/1.0 </p>
* <p> USN: [URI identifying this message] </p>
*/

/**
* Exit Presence
* <p> NOTIFY * HTTP/1.1 </p>
* <p> HOST: 239.255.255.250:1900 </p>
* <p> NT: [URI indicating the type of exit message] </p>
* <p> NTS: ssdp:byebye </p>
* <p> USN: [URI identifying this message] </p>
*/

/*
* M-SEARCH request
* <p> M-SEARCH * HTTP/1.1 </p>
* <p> HOST: 239.255.255.250:1900 </p>
* <p> ST: [URI for search target] </p>
* <p> MAN: "ssdp:discover" </p>
* <p> MX: [Maximum response delay in seconds] </p>
*/

/* method */
#define METHOD_NOTIFY       "NOTIFY"
#define METHOD_MSEARCH      "M-SEARCH"

/* headers */
#define HEAD_HOST           "HOST"
#define HEAD_CACHE_CONTROL  "CACHE-CONTROL"
#define HEAD_LOCATION       "LOCATION"
#define HEAD_ST             "ST"
#define HEAD_MAN            "MAN"
#define HEAD_MX             "MX"
#define HEAD_AL             "AL"
#define HEAD_NT             "NT"
#define HEAD_NTS            "NTS"
#define HEAD_SERVER         "SERVER"
#define HEAD_USN            "USN"

/* host value */
#define DEFAULT_HOST        "239.255.255.250:1900"

/* MAN value */
#define DEFAULT_MAN         "\"ssdp:discover\""

/* ST value */
#define DEFAULT_ST          "ssdp:all"

/* MX value */
#define DEFAULT_MX          3

/* NTS value */
#define NTS_ALIVE           "ssdp:alive"
#define NTS_BYEBYE          "ssdp:byebye"

static TinyRet UpnpRegistry_Construct(UpnpRegistry *thiz);
static TinyRet UpnpRegistry_Dispose(UpnpRegistry *thiz);
static void handle_http_request(HttpMessage *request, void *ctx);
static void handle_http_response(HttpMessage *response, void *ctx);
static void handle_notify(UpnpRegistry *thiz, HttpMessage *message);
static void handle_search_request(UpnpRegistry *thiz, HttpMessage *message);
static void handle_search_response(UpnpRegistry *thiz, HttpMessage *message);

struct _UpnpRegistry
{
    UpnpRegistryCore            core;
};

UpnpRegistry * UpnpRegistry_New(void)
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

        ret = UpnpRegistry_Construct(thiz);
        if (RET_FAILED(ret))
        {
            UpnpRegistry_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

static TinyRet UpnpRegistry_Construct(UpnpRegistry *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(UpnpRegistry));

        UpnpRegistryCore_Construct(&thiz->core);

    } while (0);

    return ret;
}

static TinyRet UpnpRegistry_Dispose(UpnpRegistry *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    UpnpRegistryCore_Dispose(&thiz->core);

    return TINY_RET_OK;
}

void UpnpRegistry_Delete(UpnpRegistry *thiz)
{
    RETURN_IF_FAIL(thiz);

    UpnpRegistry_Dispose(thiz);
    tiny_free(thiz);
}

TinyRet UpnpRegistry_Start(UpnpRegistry *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    UpnpRegistryCore_Start(&thiz->core, handle_http_request, handle_http_response, thiz);

    return TINY_RET_OK;
}

TinyRet UpnpRegistry_Stop(UpnpRegistry *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    UpnpRegistryCore_Stop(&thiz->core);

    return TINY_RET_OK;
}

TinyRet UpnpRegistry_Discover(UpnpRegistry *thiz, UpnpDeviceListener listener, void *ctx)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
//    RETURN_VAL_IF_FAIL(listener, TINY_RET_E_ARG_NULL);

    do
    {
        HttpMessage *msg = HttpMessage_New();
        if (msg == NULL)
        {
            LOG_D(TAG, "HttpMessage_New: failed");
            ret = TINY_RET_E_NEW;
            break;
        }

        HttpMessage_SetType(msg, HTTP_REQUEST);
        HttpMessage_SetVersion(msg, 1, 1);
        HttpMessage_SetMethod(msg, METHOD_MSEARCH);
        HttpMessage_SetUri(msg, "*");
        HttpMessage_SetHeader(msg, HEAD_HOST, DEFAULT_HOST);
        HttpMessage_SetHeader(msg, HEAD_ST, DEFAULT_ST);
        HttpMessage_SetHeader(msg, HEAD_MAN, DEFAULT_MAN);
        HttpMessage_SetHeaderInteger(msg, HEAD_MX, DEFAULT_MX);

        ret = UpnpRegistryCore_SendRequest(&thiz->core, msg);

        HttpMessage_Delete(msg);
    } while (0);

    return ret;
}

TinyRet UpnpRegistry_StopDiscovery(UpnpRegistry *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    return TINY_RET_OK;
}

TinyRet UpnpRegistry_Register(UpnpRegistry *thiz, UpnpDevice *device)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(device, TINY_RET_E_ARG_NULL);

    return TINY_RET_OK;
}

TinyRet UpnpRegistry_UnRegister(UpnpRegistry *thiz, UpnpDevice *device)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(device, TINY_RET_E_ARG_NULL);

    return TINY_RET_OK;
}

static void handle_http_request(HttpMessage *request, void *ctx)
{
    UpnpRegistry *thiz = (UpnpRegistry *)ctx;

    if (HttpMessage_IsMethodEqual(request, METHOD_NOTIFY))
    {
        handle_notify(thiz, request);
    }
    else if (HttpMessage_IsMethodEqual(request, METHOD_MSEARCH))
    {
        handle_search_request(thiz, request);
    }
    else
    {
        LOG_D(TAG, "parse message: invliad HTTP method");
    }
}

static void handle_http_response(HttpMessage *response, void *ctx)
{
    handle_search_response((UpnpRegistry *)ctx, response);
}

static void handle_notify(UpnpRegistry *thiz, HttpMessage *message)
{
    LOG_D(TAG, "handle_notify");
}

static void handle_search_request(UpnpRegistry *thiz, HttpMessage *message)
{
    LOG_D(TAG, "handle_search_request");
}

static void handle_search_response(UpnpRegistry *thiz, HttpMessage *message)
{
    LOG_D(TAG, "handle_search_request");
}