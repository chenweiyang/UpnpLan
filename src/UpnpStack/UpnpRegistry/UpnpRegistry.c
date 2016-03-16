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
#include "UpnpObjectList.h"

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
static UpnpObject * convert_upnp_object(const char *ip, HttpMessage *msg, bool strictedUuid);
static bool validate_usn(UpnpRegistry *thiz, const char *str_usn);
static TinyRet validate_nt(const char *type, bool strict_uuid);

struct _UpnpRegistry
{
    UpnpRegistryCore            core;
    UpnpObjectListener          listener;
    UpnpObjectFilter            filter;
    void                      * ctx;

    bool                        strictedUuid;
    UpnpObjectList            * foundObjects;
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
        thiz->listener = NULL;
        thiz->filter = NULL;
        thiz->ctx = NULL;
        thiz->strictedUuid = false;
        thiz->foundObjects = NULL;

        UpnpRegistryCore_Construct(&thiz->core);

        thiz->foundObjects = UpnpObjectList_New();
        if (thiz->foundObjects == NULL)
        {
            ret = TINY_RET_E_NEW;
            break;
        }
    } while (0);

    return ret;
}

static TinyRet UpnpRegistry_Dispose(UpnpRegistry *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    UpnpRegistryCore_Dispose(&thiz->core);

    if (thiz->foundObjects != NULL)
    {
        UpnpObjectList_Delete(thiz->foundObjects);
    }

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

TinyRet UpnpRegistry_Discover(UpnpRegistry *thiz, bool strictedUuid, UpnpObjectListener listener, UpnpObjectFilter filter, void *ctx)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(listener, TINY_RET_E_ARG_NULL);

    thiz->strictedUuid = strictedUuid;
    thiz->listener = listener;
    thiz->filter = filter;
    thiz->ctx = ctx;

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

    thiz->listener = NULL;

    return TINY_RET_OK;
}

TinyRet UpnpRegistry_Register(UpnpRegistry *thiz, UpnpObject *object)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(object, TINY_RET_E_ARG_NULL);

    return TINY_RET_E_NOT_IMPLEMENTED;
}

TinyRet UpnpRegistry_UnRegister(UpnpRegistry *thiz, UpnpObject *object)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(object, TINY_RET_E_ARG_NULL);

    return TINY_RET_E_NOT_IMPLEMENTED;
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

/*
NOTIFY * HTTP/1.1
Host:239.255.255.250:1900
NT:uuid:9ba32c90-9923-4ec6-81d0-335100229b91
NTS:ssdp:alive
Location:http://10.0.1.8:2869/upnphost/udhisapi.dll?content=uuid:9ba32c90-9923-4ec6-81d0-335100229b91
USN:uuid:9ba32c90-9923-4ec6-81d0-335100229b91
Cache-Control:max-age=1800
Server:Microsoft-Windows-NT/5.1 UPnP/1.0 UPnP-Device-Host/1.0
OPT:"http://schemas.upnp.org/upnp/1/0/"; ns=01
01-NLS:f5662e2e617eb7532dbdedda7a40258a
*/

/*
NOTIFY * HTTP/1.1
Host:239.255.255.250:1900
NT:urn:schemas-upnp-org:service:ConnectionManager:1
NTS:ssdp:byebye
Location:http://10.0.1.8:2869/upnphost/udhisapi.dll?content=uuid:9ba32c90-9923-4ec6-81d0-335100229b91
USN:uuid:9ba32c90-9923-4ec6-81d0-335100229b91::urn:schemas-upnp-org:service:ConnectionManager:1
Cache-Control:max-age=1800
Server:Microsoft-Windows-NT/5.1 UPnP/1.0 UPnP-Device-Host/1.0
OPT:"http://schemas.upnp.org/upnp/1/0/"; ns=01
01-NLS:f5662e2e617eb7532dbdedda7a40258a
*/
static void handle_notify(UpnpRegistry *thiz, HttpMessage *message)
{
    LOG_D(TAG, "handle_notify");

    UpnpObjectList_Lock(thiz->foundObjects);

    do
    {
        TinyRet ret = TINY_RET_OK;
        const char *usn = NULL;
        const char *nts = NULL;
        UpnpObject *obj = NULL;

        if (thiz->listener == NULL)
        {
            break;
        }

        usn = HttpMessage_GetHeaderValue(message, HEAD_USN);
        if (usn == NULL)
        {
            LOG_D(TAG, "NOTIFY invalid: not found USN");
            break;
        }

        if (!validate_usn(thiz, usn))
        {
            break;
        }

        nts = HttpMessage_GetHeaderValue(message, HEAD_NTS);
        if (nts == NULL)
        {
            break;
        }

        obj = UpnpObjectList_GetObject(thiz->foundObjects, usn);

        if (STR_EQUAL(nts, NTS_ALIVE))
        {
            if (obj != NULL)
            {
                UpnpObject_UpdateNextNotify(obj);
                break;
            }

            obj = convert_upnp_object(HttpMessage_GetIp(message), message, thiz->strictedUuid);
            if (obj == NULL)
            {
                break;
            }

            UpnpObjectList_AddObject(thiz->foundObjects, obj);
            thiz->listener(obj, true, thiz->ctx);
        }
        else if (STR_EQUAL(nts, NTS_BYEBYE))
        {
            if (obj == NULL)
            {
                break;
            }

            thiz->listener(obj, false, thiz->ctx);
            UpnpObjectList_RemoveObject(thiz->foundObjects, usn);
        }
    } while (0);

    UpnpObjectList_Unlock(thiz->foundObjects);

    return;
}

static void handle_search_request(UpnpRegistry *thiz, HttpMessage *message)
{
    LOG_D(TAG, "handle_search_request");
}

static void handle_search_response(UpnpRegistry *thiz, HttpMessage *message)
{
    LOG_D(TAG, "handle_search_request");

    UpnpObjectList_Lock(thiz->foundObjects);

    do
    {
        TinyRet ret = TINY_RET_OK;
        const char *usn = NULL;
        UpnpObject *obj = NULL;

        if (thiz->listener == NULL)
        {
            break;
        }

        usn = HttpMessage_GetHeaderValue(message, HEAD_USN);
        if (usn == NULL)
        {
            break;
        }

        if (!validate_usn(thiz, usn))
        {
            break;
        }

        obj = UpnpObjectList_GetObject(thiz->foundObjects, usn);
        if (obj != NULL)
        {
            UpnpObject_UpdateNextNotify(obj);
            break;
        }

        obj = convert_upnp_object(HttpMessage_GetIp(message), message, thiz->strictedUuid);
        if (obj != NULL)
        {
            UpnpObjectList_AddObject(thiz->foundObjects, obj);
            thiz->listener(obj, true, thiz->ctx);
        }
    } while (0);

    UpnpObjectList_Unlock(thiz->foundObjects);
}

static UpnpObject * convert_upnp_object(const char *ip, HttpMessage *msg, bool strictedUuid)
{
    UpnpObject *obj = NULL;

    do
    {
        TinyRet ret = TINY_RET_OK;
        const char *usn = NULL;
        const char *nt = NULL;
        const char *cache_control = NULL;
        const char *location = NULL;
        const char *server = NULL;

        usn = HttpMessage_GetHeaderValue(msg, HEAD_USN);
        if (usn == NULL)
        {
            break;
        }

        nt = HttpMessage_GetHeaderValue(msg, HEAD_NT);
        if (nt == NULL)
        {
            nt = HttpMessage_GetHeaderValue(msg, HEAD_ST);
            if (nt == NULL)
            {
                break;
            }
        }

        ret = validate_nt(nt, strictedUuid);
        if (RET_FAILED(ret))
        {
            break;
        }

        cache_control = HttpMessage_GetHeaderValue(msg, HEAD_CACHE_CONTROL);
        if (cache_control == NULL)
        {
            break;
        }

        location = HttpMessage_GetHeaderValue(msg, HEAD_LOCATION);
        if (location == NULL)
        {
            break;
        }

        server = HttpMessage_GetHeaderValue(msg, HEAD_SERVER);
        if (server == NULL)
        {
            break;
        }

        obj = UpnpObject_New();
        if (obj == NULL)
        {
            LOG_E(TAG, "UpnpObject_New failed");
            break;
        }

        UpnpObject_SetNt(obj, nt, strictedUuid);
        UpnpObject_SetCacheControl(obj, cache_control);
        UpnpObject_SetUsn(obj, usn);
        UpnpObject_SetIp(obj, ip);
        UpnpObject_SetLocation(obj, location);
        UpnpObject_SetStackInfo(obj, server);
        UpnpObject_UpdateNextNotify(obj);
    } while (0);

    return obj;
}

static bool validate_usn(UpnpRegistry *thiz, const char *str_usn)
{
    bool result = false;

    do
    {
        TinyRet ret = TINY_RET_OK;
        UpnpUsn usn;

        ret = UpnpUsn_Construct(&usn);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = UpnpUsn_Parse(&usn, str_usn, thiz->strictedUuid);
        if (RET_FAILED(ret))
        {
            LOG_D(TAG, "UpnpUsn_Parse failed: %s", tiny_ret_to_str(ret));
            break;
        }

        result = (thiz->filter == NULL) ? true : thiz->filter(&usn, thiz->ctx);

        UpnpUsn_Dispose(&usn);
    } while (0);

    return result;
}

static TinyRet validate_nt(const char *type, bool strict_uuid)
{
    TinyRet ret = TINY_RET_OK;

    do
    {
        UpnpUri uri;

        ret = UpnpUri_Construct(&uri);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = UpnpUri_Parse(&uri, type, strict_uuid);
        if (RET_FAILED(ret))
        {
            LOG_D(TAG, "UpnpUri_Parse failed: %s", tiny_ret_to_str(ret));
        }

        UpnpUri_Dispose(&uri);

    } while (0);

    return ret;
}