/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   UpnpDeviceFactory.c
*
* @remark
*
*/

#include "UpnpDeviceFactory.h"
#include "UpnpDeviceParser.h"
#include "UpnpServiceParser.h"
#include "UpnpDeviceDefinition.h"
#include "UpnpServiceDefinition.h"
#include "tiny_log.h"

#define TAG         "UpnpDeviceFactory"

UpnpDevice * UpnpDeviceFactory_Create(UpnpDeviceSummary *summary)
{
    UpnpDevice * device = NULL;

    do
    {
        uint32_t i = 0;
        uint32_t size = 0;
        TinyRet ret = TINY_RET_OK;
        UpnpServiceList *services = NULL;
        const char *urlbase = NULL;

        device = UpnpDevice_New();
        if (device == NULL)
        {
            LOG_D(TAG, "UpnpDevice_New failed");
            break;
        }

        ret = UpnpDeviceParser_Parse(summary->deviceUrl, device, UPNP_TIMEOUT);
        if (RET_FAILED(ret))
        {
            LOG_D(TAG, "DDD_Load failed");
            UpnpDevice_Delete(device);
            device = NULL;
            break;
        }

        urlbase = UpnpDevice_GetPropertyValue(device, UPNP_DEVICE_URLBase);
        if (urlbase == NULL)
        {
            LOG_D(TAG, "<%s> not found", UPNP_DEVICE_URLBase);
            UpnpDevice_Delete(device);
            device = NULL;
            break;
        }

        services = UpnpDevice_GetServiceList(device);
        size = UpnpServiceList_GetSize(services);
        for (i = 0; i < size; ++i)
        {
            char url[TINY_URL_LEN];
            UpnpService *service = UpnpServiceList_GetServiceAt(services, i);
            const char *scpdUrl = UpnpService_GetPropertyValue(service, UPNP_SERVICE_SCPDURL);
            if (scpdUrl == NULL)
            {
                LOG_D(TAG, "Get <%s> failed", UPNP_SERVICE_SCPDURL);
                ret = TINY_RET_E_NOT_FOUND;
                break;
            }

            tiny_snprintf(url, TINY_URL_LEN, "%s%s", urlbase, scpdUrl);

            ret = UpnpServiceParser_Parse(url, service, UPNP_TIMEOUT);
            if (RET_FAILED(ret))
            {
                break;
            }
        }

        if (RET_FAILED(ret))
        {
            UpnpDevice_Delete(device);
            device = NULL;
            break;
        }
    } while (0);

    return device;
}