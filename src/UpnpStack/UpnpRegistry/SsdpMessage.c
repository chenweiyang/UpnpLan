/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   SsdpMessage.c
*
* @remark
*
*/

#include "SsdpMessage.h"
#include "HttpMessage.h"
#include "tiny_log.h"

#define TAG     "SsdpMessage"

static TinyRet ssdp_parse_alive(HttpMessage *message, SsdpAlive *alive);
static TinyRet ssdp_parse_byebye(HttpMessage *message, SsdpByebye *byebye);
static TinyRet ssdp_parse_request(HttpMessage *message, SsdpRequest *request);
static TinyRet ssdp_parse_response(HttpMessage *message, SsdpResponse *response);

static TinyRet ssdp_parse_alive(HttpMessage *message, SsdpAlive *alive)
{
    TinyRet ret = TINY_RET_OK;

    const char *host = NULL;
    const char *cache_control = NULL;
    const char *location = NULL;
    const char *nt = NULL;
    const char *nts = NULL;
    const char *server = NULL;
    const char *usn = NULL;

    RETURN_VAL_IF_FAIL(message, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(alive, TINY_RET_E_ARG_NULL);

    do
    {
        host = HttpMessage_GetHeaderValue(message, HEAD_HOST);
        if (host == NULL)
        {
            LOG_D(TAG, "NOT FOUND: %s", HEAD_HOST);
            ret = TINY_RET_E_HTTP_MSG_INVALID;
            break;
        }

        cache_control = HttpMessage_GetHeaderValue(message, HEAD_CACHE_CONTROL);
        if (cache_control == NULL)
        {
            LOG_D(TAG, "NOT FOUND: %s", HEAD_CACHE_CONTROL);
            ret = TINY_RET_E_HTTP_MSG_INVALID;
            break;
        }

        location = HttpMessage_GetHeaderValue(message, HEAD_LOCATION);
        if (location == NULL)
        {
            LOG_D(TAG, "NOT FOUND: %s", HEAD_LOCATION);
            ret = TINY_RET_E_HTTP_MSG_INVALID;
            break;
        }

        nt = HttpMessage_GetHeaderValue(message, HEAD_NT);
        if (nt == NULL)
        {
            LOG_D(TAG, "NOT FOUND: %s", HEAD_NT);
            ret = TINY_RET_E_HTTP_MSG_INVALID;
            break;
        }

        nts = HttpMessage_GetHeaderValue(message, HEAD_NTS);
        if (nt == NULL)
        {
            LOG_D(TAG, "NOT FOUND: %s", HEAD_NTS);
            ret = TINY_RET_E_HTTP_MSG_INVALID;
            break;
        }

        usn = HttpMessage_GetHeaderValue(message, HEAD_USN);
        if (usn == NULL)
        {
            LOG_D(TAG, "NOT FOUND: %s", HEAD_USN);
            ret = TINY_RET_E_HTTP_MSG_INVALID;
            break;
        }

        strncpy(alive->host, host, HEAD_HOST_LEN);
        strncpy(alive->cache_control, cache_control, HEAD_CACHE_CONTROL_LEN);
        strncpy(alive->location, location, HEAD_LOCATION_LEN);
        strncpy(alive->nt, nt, HEAD_NT_LEN);
        strncpy(alive->nts, nts, HEAD_NTS_LEN);
        strncpy(alive->usn, usn, HEAD_USN_LEN);

        /* OPTIONAL */
        server = HttpMessage_GetHeaderValue(message, HEAD_SERVER);
        if (server != NULL)
        {
            strncpy(alive->server, server, HEAD_USN_LEN);
        }
    } while (0);

    return ret;
}

static TinyRet ssdp_parse_byebye(HttpMessage *message, SsdpByebye *byebye)
{
    TinyRet ret = TINY_RET_OK;

    const char *host = NULL;
    const char *nt = NULL;
    const char *nts = NULL;
    const char *usn = NULL;

    RETURN_VAL_IF_FAIL(message, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(byebye, TINY_RET_E_ARG_NULL);

    do
    {
        host = HttpMessage_GetHeaderValue(message, HEAD_HOST);
        if (host == NULL)
        {
            LOG_D(TAG, "NOT FOUND: %s", HEAD_HOST);
            ret = TINY_RET_E_HTTP_MSG_INVALID;
            break;
        }

        nt = HttpMessage_GetHeaderValue(message, HEAD_NT);
        if (nt == NULL)
        {
            LOG_D(TAG, "NOT FOUND: %s", HEAD_NT);
            ret = TINY_RET_E_HTTP_MSG_INVALID;
            break;
        }

        nts = HttpMessage_GetHeaderValue(message, HEAD_NTS);
        if (nt == NULL)
        {
            LOG_D(TAG, "NOT FOUND: %s", HEAD_NTS);
            ret = TINY_RET_E_HTTP_MSG_INVALID;
            break;
        }

        usn = HttpMessage_GetHeaderValue(message, HEAD_USN);
        if (usn == NULL)
        {
            LOG_D(TAG, "NOT FOUND: %s", HEAD_USN);
            ret = TINY_RET_E_HTTP_MSG_INVALID;
            break;
        }

        strncpy(byebye->host, host, HEAD_HOST_LEN);
        strncpy(byebye->nt, nt, HEAD_NT_LEN);
        strncpy(byebye->nts, nts, HEAD_NTS_LEN);
        strncpy(byebye->usn, usn, HEAD_USN_LEN);
    } while (0);

    return ret;
}

static TinyRet ssdp_parse_request(HttpMessage *message, SsdpRequest *request)
{
    TinyRet ret = TINY_RET_OK;

    const char *host = NULL;
    const char *st = NULL;
    const char *man = NULL;
    const char *mx = NULL;

    RETURN_VAL_IF_FAIL(message, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(request, TINY_RET_E_ARG_NULL);

    do
    {
        host = HttpMessage_GetHeaderValue(message, HEAD_HOST);
        if (host == NULL)
        {
            LOG_D(TAG, "NOT FOUND: %s", HEAD_HOST);
            ret = TINY_RET_E_HTTP_MSG_INVALID;
            break;
        }

        st = HttpMessage_GetHeaderValue(message, HEAD_ST);
        if (st == NULL)
        {
            LOG_D(TAG, "NOT FOUND: %s", HEAD_ST);
            ret = TINY_RET_E_HTTP_MSG_INVALID;
            break;
        }

        man = HttpMessage_GetHeaderValue(message, HEAD_MAN);
        if (man == NULL)
        {
            LOG_D(TAG, "NOT FOUND: %s", HEAD_MAN);
            ret = TINY_RET_E_HTTP_MSG_INVALID;
            break;
        }

        mx = HttpMessage_GetHeaderValue(message, HEAD_MX);
        if (mx == NULL)
        {
            LOG_D(TAG, "NOT FOUND: %s", HEAD_MX);
            ret = TINY_RET_E_HTTP_MSG_INVALID;
            break;
        }

        strncpy(request->host, host, HEAD_HOST_LEN);
        strncpy(request->st, st, HEAD_ST_LEN);
        strncpy(request->man, man, HEAD_MAN_LEN);
        request->mx = atoi(mx);
    } while (0);

    return ret;
}

static TinyRet ssdp_parse_response(HttpMessage *message, SsdpResponse *response)
{
    TinyRet ret = TINY_RET_OK;

    const char *cache_control = NULL;
    const char *location = NULL;
    const char *st = NULL;
    const char *server = NULL;
    const char *usn = NULL;

    RETURN_VAL_IF_FAIL(message, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(response, TINY_RET_E_ARG_NULL);

    do
    {
        cache_control = HttpMessage_GetHeaderValue(message, HEAD_CACHE_CONTROL);
        if (cache_control == NULL)
        {
            LOG_D(TAG, "NOT FOUND: %s", HEAD_CACHE_CONTROL);
            ret = TINY_RET_E_HTTP_MSG_INVALID;
            break;
        }

        location = HttpMessage_GetHeaderValue(message, HEAD_LOCATION);
        if (location == NULL)
        {
            LOG_D(TAG, "NOT FOUND: %s", HEAD_LOCATION);
            ret = TINY_RET_E_HTTP_MSG_INVALID;
            break;
        }

        st = HttpMessage_GetHeaderValue(message, HEAD_ST);
        if (st == NULL)
        {
            LOG_D(TAG, "NOT FOUND: %s", HEAD_ST);
            ret = TINY_RET_E_HTTP_MSG_INVALID;
            break;
        }

        usn = HttpMessage_GetHeaderValue(message, HEAD_USN);
        if (usn == NULL)
        {
            LOG_D(TAG, "NOT FOUND: %s", HEAD_USN);
            ret = TINY_RET_E_HTTP_MSG_INVALID;
            break;
        }

        strncpy(response->cache_control, cache_control, HEAD_CACHE_CONTROL_LEN);
        strncpy(response->location, location, HEAD_LOCATION_LEN);
        strncpy(response->st, st, HEAD_NT_LEN);
        strncpy(response->usn, usn, HEAD_USN_LEN);

        /* OPTIONAL */
        server = HttpMessage_GetHeaderValue(message, HEAD_SERVER);
        if (server != NULL)
        {
            strncpy(response->server, server, HEAD_USN_LEN);
        }
    } while (0);

    return ret;
}

TinyRet SsdpMessage_Construct(SsdpMessage *thiz, const char *ip, uint16_t port, const char *buf, uint32_t len)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        HttpMessage msg;

        memset(thiz, 0, sizeof(SsdpMessage));

        ret = HttpMessage_Construct(&msg);
        if (RET_FAILED(ret))
        {
            LOG_D(TAG, "HttpMessage_Construct failed");
            break;
        }

        ret = HttpMessage_Parse(&msg, buf, len);
        if (RET_FAILED(ret))
        {
            LOG_D(TAG, "HttpMessage_Parse: invliad message");
            HttpMessage_Dispose(&msg);
            break;
        }

        strncpy(thiz->ip, ip, TINY_IP_LEN);
        thiz->port = port;

        switch (HttpMessage_GetType(&msg))
        {
        case HTTP_REQUEST:
            if (HttpMessage_IsMethodEqual(&msg, METHOD_NOTIFY))
            {
                const char *nts = HttpMessage_GetHeaderValue(&msg, HEAD_NTS);
                if (nts == NULL)
                {
                    ret = TINY_RET_E_HTTP_MSG_INVALID;
                    break;
                }

                if (STR_EQUAL(nts, NTS_ALIVE))
                {
                    thiz->type = SSDP_ALIVE;
                    ret = ssdp_parse_alive(&msg, &thiz->v.alive);
                }
                else if (STR_EQUAL(nts, NTS_BYEBYE))
                {
                    thiz->type = SSDP_BYEBYE;
                    ret = ssdp_parse_byebye(&msg, &thiz->v.byebye);
                }
            }
            else if (HttpMessage_IsMethodEqual(&msg, METHOD_MSEARCH))
            {
                thiz->type = SSDP_MSEARCH_REQUEST;
                ret = ssdp_parse_request(&msg, &thiz->v.request);
            }
            else
            {
                LOG_D(TAG, "INVALID HTTP METHOD");
            }
            break;

        case HTTP_RESPONSE:
            thiz->type = SSDP_MSEARCH_RESPONSE;
            ret = ssdp_parse_response(&msg, &thiz->v.response);
            break;

        default:
            LOG_D(TAG, "INVALID HTTP MESSAGE TYPE");
            break;
        }

        HttpMessage_Dispose(&msg);
    } while (0);

    return ret;
}

TinyRet SsdpMessage_ConstructAlive_ROOTDEVICE(SsdpMessage *thiz, UpnpDevice *device, const char *location)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(device, TINY_RET_E_ARG_NULL);

    do
    {
        const char *deviceId = UpnpDevice_GetDeviceId(device);
        uint16_t port = UpnpDevice_GetHttpPort(device);
        char usn[HEAD_USN_LEN + 1];

        memset(usn, 0, HEAD_USN_LEN + 1);
        tiny_snprintf(usn, HEAD_USN_LEN, "%s::upnp:rootdevice", deviceId);
        usn[HEAD_USN_LEN] = '\0';

        memset(thiz, 0, sizeof(SsdpMessage));
        thiz->type = SSDP_ALIVE;

        strncpy(thiz->v.alive.host, DEFAULT_HOST, HEAD_HOST_LEN);
        strncpy(thiz->v.alive.cache_control, "max-age=1800", HEAD_CACHE_CONTROL_LEN);
        strncpy(thiz->v.alive.nt, "upnp:rootdevice", HEAD_NT_LEN);
        strncpy(thiz->v.alive.nts, NTS_ALIVE, HEAD_NTS_LEN);
        strncpy(thiz->v.alive.server, "UpnpLan/0.1 UPnP/1.0", HEAD_SERVER_LEN);
        strncpy(thiz->v.alive.usn, usn, HEAD_USN_LEN);
        thiz->v.alive.usn[HEAD_USN_LEN] = '\0';
        strncpy(thiz->v.alive.location, location, HEAD_LOCATION_LEN);
    } while (0);

    return ret;
}

TinyRet SsdpMessage_ConstructAlive_DEVICE_UUID(SsdpMessage *thiz, UpnpDevice *device, const char *location)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(device, TINY_RET_E_ARG_NULL);

    do
    {
        const char *deviceId = UpnpDevice_GetDeviceId(device);

        memset(thiz, 0, sizeof(SsdpMessage));
        thiz->type = SSDP_ALIVE;

        strncpy(thiz->v.alive.host, DEFAULT_HOST, HEAD_HOST_LEN);
        strncpy(thiz->v.alive.cache_control, "max-age=1800", HEAD_CACHE_CONTROL_LEN);
        strncpy(thiz->v.alive.nt, deviceId, HEAD_NT_LEN);
        strncpy(thiz->v.alive.nts, NTS_ALIVE, HEAD_NTS_LEN);
        strncpy(thiz->v.alive.server, "UpnpLan/0.1 UPnP/1.0", HEAD_SERVER_LEN);
        strncpy(thiz->v.alive.usn, deviceId, HEAD_USN_LEN);
        strncpy(thiz->v.alive.location, location, HEAD_LOCATION_LEN);
    } while (0);

    return ret;
}

TinyRet SsdpMessage_ConstructAlive_DEVICE(SsdpMessage *thiz, UpnpDevice *device, const char *location)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(device, TINY_RET_E_ARG_NULL);

    do
    {
        const char *deviceId = UpnpDevice_GetDeviceId(device);
        const char *deviceType = UpnpDevice_GetDeviceType(device);
        char usn[HEAD_USN_LEN + 1];

        memset(usn, 0, HEAD_USN_LEN + 1);
        tiny_snprintf(usn, HEAD_USN_LEN, "%s::%s", deviceId, deviceType);
        usn[HEAD_USN_LEN] = '\0';

        memset(thiz, 0, sizeof(SsdpMessage));
        thiz->type = SSDP_ALIVE;

        strncpy(thiz->v.alive.host, DEFAULT_HOST, HEAD_HOST_LEN);
        strncpy(thiz->v.alive.cache_control, "max-age=1800", HEAD_CACHE_CONTROL_LEN);
        strncpy(thiz->v.alive.nt, deviceType, HEAD_NT_LEN);
        strncpy(thiz->v.alive.nts, NTS_ALIVE, HEAD_NTS_LEN);
        strncpy(thiz->v.alive.server, "UpnpLan/0.1 UPnP/1.0", HEAD_SERVER_LEN);
        strncpy(thiz->v.alive.usn, usn, HEAD_USN_LEN);
        thiz->v.alive.usn[HEAD_USN_LEN] = '\0';
        strncpy(thiz->v.alive.location, location, HEAD_LOCATION_LEN);
    } while (0);

    return ret;
}

TinyRet SsdpMessage_ConstructAlive_SERVICE(SsdpMessage *thiz, UpnpService *service, const char *location)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(service, TINY_RET_E_ARG_NULL);

    do
    {
        UpnpDevice *device = (UpnpDevice *) UpnpService_GetParentDevice(service);
        const char *deviceId = UpnpDevice_GetDeviceId(device);
        const char *serviceType = UpnpService_GetServiceType(service);
        char usn[HEAD_USN_LEN + 1];

        memset(usn, 0, HEAD_USN_LEN + 1);
        tiny_snprintf(usn, HEAD_USN_LEN, "%s::%s", deviceId, serviceType);
        usn[HEAD_USN_LEN] = '\0';

        memset(thiz, 0, sizeof(SsdpMessage));
        thiz->type = SSDP_ALIVE;

        strncpy(thiz->v.alive.host, DEFAULT_HOST, HEAD_HOST_LEN);
        strncpy(thiz->v.alive.cache_control, "max-age=1800", HEAD_CACHE_CONTROL_LEN);
        strncpy(thiz->v.alive.nt, serviceType, HEAD_NT_LEN);
        strncpy(thiz->v.alive.nts, NTS_ALIVE, HEAD_NTS_LEN);
        strncpy(thiz->v.alive.server, "UpnpLan/0.1 UPnP/1.0", HEAD_SERVER_LEN);
        strncpy(thiz->v.alive.usn, usn, HEAD_USN_LEN);
        thiz->v.alive.usn[HEAD_USN_LEN] = '\0';
        strncpy(thiz->v.alive.location, location, HEAD_LOCATION_LEN);
    } while (0);

    return ret;
}

TinyRet SsdpMessage_ConstructByebye_ROOTDEVICE(SsdpMessage *thiz, UpnpDevice *device)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(device, TINY_RET_E_ARG_NULL);

    do
    {
        const char *deviceId = UpnpDevice_GetDeviceId(device);
        char usn[HEAD_USN_LEN + 1];

        memset(usn, 0, HEAD_USN_LEN + 1);
        tiny_snprintf(usn, HEAD_USN_LEN, "%s::upnp:rootdevice", deviceId);
        usn[HEAD_USN_LEN] = '\0';

        memset(thiz, 0, sizeof(SsdpMessage));
        thiz->type = SSDP_BYEBYE;

        strncpy(thiz->v.byebye.host, DEFAULT_HOST, HEAD_HOST_LEN);
        strncpy(thiz->v.byebye.nt, "upnp:rootdevice", HEAD_NT_LEN);
        strncpy(thiz->v.byebye.nts, NTS_BYEBYE, HEAD_NTS_LEN);
        strncpy(thiz->v.byebye.usn, usn, HEAD_USN_LEN);
        thiz->v.byebye.usn[HEAD_USN_LEN] = '\0';
    } while (0);

    return ret;
}

TinyRet SsdpMessage_ConstructByebye_DEVICE_UUID(SsdpMessage *thiz, UpnpDevice *device)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(device, TINY_RET_E_ARG_NULL);

    do
    {
        const char *deviceId = UpnpDevice_GetDeviceId(device);

        memset(thiz, 0, sizeof(SsdpMessage));
        thiz->type = SSDP_BYEBYE;

        strncpy(thiz->v.byebye.host, DEFAULT_HOST, HEAD_HOST_LEN);
        strncpy(thiz->v.byebye.nt, deviceId, HEAD_NT_LEN);
        strncpy(thiz->v.byebye.nts, NTS_BYEBYE, HEAD_NTS_LEN);
        strncpy(thiz->v.byebye.usn, deviceId, HEAD_USN_LEN);
    } while (0);

    return ret;
}

TinyRet SsdpMessage_ConstructByebye_DEVICE(SsdpMessage *thiz, UpnpDevice *device)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(device, TINY_RET_E_ARG_NULL);

    do
    {
        const char *deviceId = UpnpDevice_GetDeviceId(device);
        const char *deviceType = UpnpDevice_GetDeviceType(device);
        char usn[HEAD_USN_LEN + 1];

        memset(usn, 0, HEAD_USN_LEN + 1);
        tiny_snprintf(usn, HEAD_USN_LEN, "%s::%s", deviceId, deviceType);
        usn[HEAD_USN_LEN] = '\0';

        memset(thiz, 0, sizeof(SsdpMessage));
        thiz->type = SSDP_BYEBYE;

        strncpy(thiz->v.byebye.host, DEFAULT_HOST, HEAD_HOST_LEN);
        strncpy(thiz->v.byebye.nt, deviceType, HEAD_NT_LEN);
        strncpy(thiz->v.byebye.nts, NTS_BYEBYE, HEAD_NTS_LEN);
        strncpy(thiz->v.byebye.usn, usn, HEAD_USN_LEN);
        thiz->v.byebye.usn[HEAD_USN_LEN] = '\0';
    } while (0);

    return ret;
}

TinyRet SsdpMessage_ConstructByebye_SERVICE(SsdpMessage *thiz, UpnpService *service)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(service, TINY_RET_E_ARG_NULL);

    do
    {
        UpnpDevice *device = (UpnpDevice *)UpnpService_GetParentDevice(service);
        const char *deviceId = UpnpDevice_GetDeviceId(device);
        const char *serviceType = UpnpService_GetServiceType(service);
        char usn[HEAD_USN_LEN + 1];

        memset(usn, 0, HEAD_USN_LEN + 1);
        tiny_snprintf(usn, HEAD_USN_LEN, "%s::%s", deviceId, serviceType);
        usn[HEAD_USN_LEN] = '\0';

        memset(thiz, 0, sizeof(SsdpMessage));
        thiz->type = SSDP_BYEBYE;

        strncpy(thiz->v.byebye.host, DEFAULT_HOST, HEAD_HOST_LEN);
        strncpy(thiz->v.byebye.nt, serviceType, HEAD_NT_LEN);
        strncpy(thiz->v.byebye.nts, NTS_BYEBYE, HEAD_NTS_LEN);
        strncpy(thiz->v.byebye.usn, usn, HEAD_USN_LEN);
        thiz->v.byebye.usn[HEAD_USN_LEN] = '\0';
    } while (0);

    return ret;
}

TinyRet SsdpMessage_ConstructRequest(SsdpMessage *thiz, const char *target)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(target, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(SsdpMessage));
        thiz->type = SSDP_MSEARCH_REQUEST;

        strncpy(thiz->v.request.host, DEFAULT_HOST, HEAD_HOST_LEN);
        strncpy(thiz->v.request.st, target, HEAD_ST_LEN);
        strncpy(thiz->v.request.man, DEFAULT_MAN, HEAD_MAN_LEN);
        thiz->v.request.mx = DEFAULT_MX;
    } while (0);

    return ret;
}

TinyRet SsdpMessage_ConstructResponse_ROOTDEVICE(SsdpMessage *thiz, UpnpDevice *device, const char *location, const char *ip, uint16_t port)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(device, TINY_RET_E_ARG_NULL);

    do
    {
        const char *deviceId = UpnpDevice_GetDeviceId(device);
        uint16_t port = UpnpDevice_GetHttpPort(device);
        char usn[HEAD_USN_LEN + 1];

        memset(usn, 0, HEAD_USN_LEN + 1);
        tiny_snprintf(usn, HEAD_USN_LEN, "%s::upnp:rootdevice", deviceId);
        usn[HEAD_USN_LEN] = '\0';

        memset(thiz, 0, sizeof(SsdpMessage));
        thiz->type = SSDP_MSEARCH_RESPONSE;

        thiz->port = port;
        strncpy(thiz->ip, ip, TINY_IP_LEN);
        strncpy(thiz->v.response.cache_control, "max-age=1800", HEAD_CACHE_CONTROL_LEN);
        strncpy(thiz->v.response.st, "upnp:rootdevice", HEAD_NT_LEN);
        strncpy(thiz->v.response.server, "UpnpLan/0.1 UPnP/1.0", HEAD_SERVER_LEN);
        strncpy(thiz->v.response.usn, usn, HEAD_USN_LEN);
        thiz->v.response.usn[HEAD_USN_LEN] = '\0';
        strncpy(thiz->v.response.location, location, HEAD_LOCATION_LEN);
    } while (0);

    return ret;

}

TinyRet SsdpMessage_ConstructResponse_DEVICE_UUID(SsdpMessage *thiz, UpnpDevice *device, const char *location, const char *ip, uint16_t port)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(device, TINY_RET_E_ARG_NULL);

    do
    {
        const char *deviceId = UpnpDevice_GetDeviceId(device);
        const char *deviceType = UpnpDevice_GetDeviceType(device);
        char usn[HEAD_USN_LEN + 1];

        memset(usn, 0, HEAD_USN_LEN + 1);
        tiny_snprintf(usn, HEAD_USN_LEN, "%s::%s", deviceId, deviceType);
        usn[HEAD_USN_LEN] = '\0';

        memset(thiz, 0, sizeof(SsdpMessage));
        thiz->type = SSDP_MSEARCH_RESPONSE;

        thiz->port = port;
        strncpy(thiz->ip, ip, TINY_IP_LEN);
        strncpy(thiz->v.response.cache_control, "max-age=1800", HEAD_CACHE_CONTROL_LEN);
        strncpy(thiz->v.response.st, deviceType, HEAD_NT_LEN);
        strncpy(thiz->v.response.server, "UpnpLan/0.1 UPnP/1.0", HEAD_SERVER_LEN);
        strncpy(thiz->v.response.usn, usn, HEAD_USN_LEN);
        thiz->v.response.usn[HEAD_USN_LEN] = '\0';
        strncpy(thiz->v.response.location, location, HEAD_LOCATION_LEN);
    } while (0);

    return ret;
}

TinyRet SsdpMessage_ConstructResponse_DEVICE(SsdpMessage *thiz, UpnpDevice *device, const char *location, const char *ip, uint16_t port)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(device, TINY_RET_E_ARG_NULL);

    do
    {
        const char *deviceId = UpnpDevice_GetDeviceId(device);
        const char *deviceType = UpnpDevice_GetDeviceType(device);
        char usn[HEAD_USN_LEN + 1];

        memset(usn, 0, HEAD_USN_LEN + 1);
        tiny_snprintf(usn, HEAD_USN_LEN, "%s::%s", deviceId, deviceType);
        usn[HEAD_USN_LEN] = '\0';

        memset(thiz, 0, sizeof(SsdpMessage));
        thiz->type = SSDP_MSEARCH_RESPONSE;

        thiz->port = port;
        strncpy(thiz->ip, ip, TINY_IP_LEN);
        strncpy(thiz->v.response.cache_control, "max-age=1800", HEAD_CACHE_CONTROL_LEN);
        strncpy(thiz->v.response.st, deviceType, HEAD_NT_LEN);
        strncpy(thiz->v.response.server, "UpnpLan/0.1 UPnP/1.0", HEAD_SERVER_LEN);
        strncpy(thiz->v.response.usn, usn, HEAD_USN_LEN);
        thiz->v.response.usn[HEAD_USN_LEN] = '\0';
        strncpy(thiz->v.response.location, location, HEAD_LOCATION_LEN);
    } while (0);

    return ret;
}

TinyRet SsdpMessage_ConstructResponse_SERVICE(SsdpMessage *thiz, UpnpService *service, const char *location, const char *ip, uint16_t port)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(service, TINY_RET_E_ARG_NULL);

    do
    {
        UpnpDevice *device = (UpnpDevice *)UpnpService_GetParentDevice(service);
        const char *deviceId = UpnpDevice_GetDeviceId(device);
        const char *serviceType = UpnpService_GetServiceType(service);
        char usn[HEAD_USN_LEN + 1];

        memset(usn, 0, HEAD_USN_LEN + 1);
        tiny_snprintf(usn, HEAD_USN_LEN, "%s::%s", deviceId, serviceType);
        usn[HEAD_USN_LEN] = '\0';

        memset(thiz, 0, sizeof(SsdpMessage));
        thiz->type = SSDP_MSEARCH_RESPONSE;

        thiz->port = port;
        strncpy(thiz->ip, ip, TINY_IP_LEN);
        strncpy(thiz->v.response.cache_control, "max-age=1800", HEAD_CACHE_CONTROL_LEN);
        strncpy(thiz->v.response.st, serviceType, HEAD_NT_LEN);
        strncpy(thiz->v.response.server, "UpnpLan/0.1 UPnP/1.0", HEAD_SERVER_LEN);
        strncpy(thiz->v.response.usn, usn, HEAD_USN_LEN);
        thiz->v.response.usn[HEAD_USN_LEN] = '\0';
        strncpy(thiz->v.response.location, location, HEAD_LOCATION_LEN);
    } while (0);

    return ret;
}

void SsdpMessage_Dispose(SsdpMessage *thiz)
{
    RETURN_IF_FAIL(thiz);
}

uint32_t SsdpMessage_ToString(SsdpMessage *thiz, char string[], uint32_t len)
{
    uint32_t ret = 0;

    do
    {
        HttpMessage msg;
        if (RET_FAILED(HttpMessage_Construct(&msg)))
        {
            LOG_D(TAG, "HttpMessage_Construct: failed");
            break;
        }

        switch (thiz->type)
        {
        case SSDP_ALIVE:
            HttpMessage_SetType(&msg, HTTP_REQUEST);
            HttpMessage_SetVersion(&msg, 1, 1);
            HttpMessage_SetMethod(&msg, METHOD_NOTIFY);
            HttpMessage_SetUri(&msg, "*");
            HttpMessage_SetHeader(&msg, HEAD_HOST, thiz->v.alive.host);
            HttpMessage_SetHeader(&msg, HEAD_CACHE_CONTROL, thiz->v.alive.cache_control);
            HttpMessage_SetHeader(&msg, HEAD_NT, thiz->v.alive.nt);
            HttpMessage_SetHeader(&msg, HEAD_NTS, thiz->v.alive.nts);
            HttpMessage_SetHeader(&msg, HEAD_SERVER, thiz->v.alive.server);
            HttpMessage_SetHeader(&msg, HEAD_USN, thiz->v.alive.usn);
            HttpMessage_SetHeader(&msg, HEAD_LOCATION, thiz->v.alive.location);
            break;

        case SSDP_BYEBYE:
            HttpMessage_SetType(&msg, HTTP_REQUEST);
            HttpMessage_SetVersion(&msg, 1, 1);
            HttpMessage_SetMethod(&msg, METHOD_NOTIFY);
            HttpMessage_SetUri(&msg, "*");
            HttpMessage_SetHeader(&msg, HEAD_HOST, thiz->v.byebye.host);
            HttpMessage_SetHeader(&msg, HEAD_NT, thiz->v.byebye.nt);
            HttpMessage_SetHeader(&msg, HEAD_NTS, thiz->v.byebye.nts);
            break;

        case SSDP_MSEARCH_REQUEST:
            HttpMessage_SetType(&msg, HTTP_REQUEST);
            HttpMessage_SetVersion(&msg, 1, 1);
            HttpMessage_SetMethod(&msg, METHOD_MSEARCH);
            HttpMessage_SetUri(&msg, "*");
            HttpMessage_SetHeader(&msg, HEAD_HOST, thiz->v.request.host);
            HttpMessage_SetHeader(&msg, HEAD_ST, thiz->v.request.st);
            HttpMessage_SetHeader(&msg, HEAD_MAN, thiz->v.request.man);
            HttpMessage_SetHeaderInteger(&msg, HEAD_MX, thiz->v.request.mx);
            break;

        case SSDP_MSEARCH_RESPONSE:
            break;

        default:
            break;
        }

        ret = HttpMessage_ToString(&msg, string, len);

        HttpMessage_Dispose(&msg);
    } while (0);

    return ret;
}