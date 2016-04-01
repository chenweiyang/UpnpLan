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
        const char *urlbase = NULL;

        device = UpnpDevice_New();
        if (device == NULL)
        {
            LOG_D(TAG, "UpnpDevice_New failed");
            break;
        }

        UpnpDevice_SetAddress(device, summary->deviceIp);

        ret = UpnpDeviceParser_Parse(summary->deviceUrl, device, UPNP_TIMEOUT);
        if (RET_FAILED(ret))
        {
            LOG_D(TAG, "DDD_Load failed");
            UpnpDevice_Delete(device);
            device = NULL;
            break;
        }

        urlbase = UpnpDevice_GetURLBase(device);
        if (urlbase == NULL)
        {
            LOG_D(TAG, "<URLBase> not found");
            UpnpDevice_Delete(device);
            device = NULL;
            break;
        }

        size = UpnpDevice_GetServiceCount(device);
        for (i = 0; i < size; ++i)
        {
            char url[TINY_URL_LEN];
            UpnpService *service = UpnpDevice_GetServiceAt(device, i);
            const char *scpdUrl = UpnpService_GetSCPDURL(service);
            if (scpdUrl == NULL)
            {
                LOG_D(TAG, "Get <SCPDURL> failed");
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