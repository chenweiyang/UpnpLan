/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   UpnpServiceHelper.c
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#include "UpnpServiceHelper.h"
#include "UpnpDevice.h"
#include "UpnpUri.h"

static TinyRet UpnpServiceHelper_GetServiceName(UpnpService *service, char name[], uint32_t len)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(service, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(name, TINY_RET_E_ARG_NULL);

    do
    {
        UpnpUri uri;
        const char *type = NULL;
        
        type = UpnpService_GetServiceType(service);
        if (type == NULL)
        {
            ret = TINY_RET_E_ARG_INVALID;
            break;
        }

        ret = UpnpUri_Construct(&uri);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = UpnpUri_Parse(&uri, type, false);
        if (RET_SUCCEEDED(ret))
        {
            strncpy(name, uri.service_type, len);
        }
        UpnpUri_Dispose(&uri);
    } while (0);

    return ret;
}

TinyRet UpnpServiceHelper_UpdateScpdUrl(UpnpService *service)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(service, TINY_RET_E_ARG_NULL);

    do
    {
        UpnpDevice *device = (UpnpDevice *)UpnpService_GetParentDevice(service);
        const char *deviceId = UpnpDevice_GetDeviceId(device);
        char name[128];
        char url[TINY_URL_LEN];

        memset(name, 0, 128);
       
        ret = UpnpServiceHelper_GetServiceName(service, name, 128);
        if (RET_FAILED(ret))
        {
            break;
        }

        memset(url, 0, TINY_URL_LEN);
        tiny_snprintf(url, TINY_URL_LEN, "/upnp/%s/%s.xml", deviceId, name);

        UpnpService_SetSCPDURL(service, url);
    } while (0);

    return ret;
}

TinyRet UpnpServiceHelper_UpdateCtrlUrl(UpnpService *service)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(service, TINY_RET_E_ARG_NULL);

    do
    {
        UpnpDevice *device = (UpnpDevice *)UpnpService_GetParentDevice(service);
        const char *deviceId = UpnpDevice_GetDeviceId(device);
        char name[128];
        char url[TINY_URL_LEN];

        memset(name, 0, 128);

        ret = UpnpServiceHelper_GetServiceName(service, name, 128);
        if (RET_FAILED(ret))
        {
            break;
        }

        memset(url, 0, TINY_URL_LEN);
        tiny_snprintf(url, TINY_URL_LEN, "/upnp/%s/ctrl/%s", deviceId, name);
        UpnpService_SetControlURL(service, url);
    } while (0);

    return ret;
}

TinyRet UpnpServiceHelper_UpdateEventUrl(UpnpService *service)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(service, TINY_RET_E_ARG_NULL);

    do
    {
        UpnpDevice *device = (UpnpDevice *)UpnpService_GetParentDevice(service);
        const char *deviceId = UpnpDevice_GetDeviceId(device);
        char name[128];
        char url[TINY_URL_LEN];

        memset(name, 0, 128);

        ret = UpnpServiceHelper_GetServiceName(service, name, 128);
        if (RET_FAILED(ret))
        {
            break;
        }

        memset(url, 0, TINY_URL_LEN);
        tiny_snprintf(url, TINY_URL_LEN, "/upnp/%s/event/%s", deviceId, name);

        UpnpService_SetEventSubURL(service, url);

    } while (0);

    return ret;
}

TinyRet UpnpServiceHelper_UpdateServiceId(UpnpService *service)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(service, TINY_RET_E_ARG_NULL);

    do
    {
        UpnpDevice *device = (UpnpDevice *)UpnpService_GetParentDevice(service);
        const char *deviceId = UpnpDevice_GetDeviceId(device);
        const char *serviceType = UpnpService_GetServiceType(service);
        char serviceId[128];
        UpnpUri uri;

        ret = UpnpUri_Construct(&uri);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = UpnpUri_Parse(&uri, serviceType, true);
        if (RET_FAILED(ret))
        {
            UpnpUri_Dispose(&uri);
            break;
        }

        memset(serviceId, 0, 128);
        tiny_snprintf(serviceId, 128, "urn:upnp-org:serviceId:%s", uri.service_type);

        UpnpService_SetServiceId(service, serviceId);

        UpnpUri_Dispose(&uri);
    } while (0);

    return ret;
}