/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   UpnpDeviceConfig.c
*
* @remark
*
*/

#include "UpnpDeviceConfig.h"
#include "UpnpDeviceDefinition.h"
#include "tiny_memory.h"
#include "tiny_log.h"
#include "TinyUuid.h"

#define TAG     "UpnpDeviceConfig"

static TinyRet UpnpDeviceConfig_Construct(UpnpDeviceConfig *thiz);
static void UpnpDeviceConfig_Dispose(UpnpDeviceConfig *thiz);
static bool generateNewDeviceId(UpnpDevice * device);

#define LEN     128

struct _UpnpDeviceConfig
{
    char deviceName[LEN];
    char modelNumber[LEN];
    char modelName[LEN];
    char modelDescription[LEN];
    char modelUrl[LEN];
    char manufacturer[LEN];
    char manufacturerUrl[LEN];
};

UpnpDeviceConfig * UpnpDeviceConfig_New(void)
{
    UpnpDeviceConfig *thiz = NULL;

    do
    {
        thiz = (UpnpDeviceConfig *)tiny_malloc(sizeof(UpnpDeviceConfig));
        if (thiz == NULL)
        {
            LOG_E(TAG, "OUT OF MEMORY");
            break;
        }

        if (RET_FAILED(UpnpDeviceConfig_Construct(thiz)))
        {
            LOG_E(TAG, "UpnpDeviceConfig_Construct failed");
            UpnpDeviceConfig_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

static TinyRet UpnpDeviceConfig_Construct(UpnpDeviceConfig *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(UpnpDeviceConfig));
    } while (0);

    return ret;
}

static void UpnpDeviceConfig_Dispose(UpnpDeviceConfig *thiz)
{
    RETURN_IF_FAIL(thiz);
}

void UpnpDeviceConfig_Delete(UpnpDeviceConfig *thiz)
{
    RETURN_IF_FAIL(thiz);

    UpnpDeviceConfig_Dispose(thiz);
    tiny_free(thiz);
}

void UpnpDeviceConfig_SetDeviceName(UpnpDeviceConfig *thiz, const char *name)
{
    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(name);

    strncpy(thiz->deviceName, name, LEN);
}

void UpnpDeviceConfig_SetModelName(UpnpDeviceConfig *thiz, const char *name)
{
    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(name);

    strncpy(thiz->modelName, name, LEN);
}

void UpnpDeviceConfig_SetModelNumber(UpnpDeviceConfig *thiz, const char *number)
{
    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(number);

    strncpy(thiz->modelNumber, number, LEN);
}

void UpnpDeviceConfig_SetModelDescription(UpnpDeviceConfig *thiz, const char *description)
{
    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(description);

    strncpy(thiz->modelDescription, description, LEN);
}

void UpnpDeviceConfig_SetModelUrl(UpnpDeviceConfig *thiz, const char *url)
{
    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(url);

    strncpy(thiz->modelUrl, url, LEN);
}

void UpnpDeviceConfig_SetManufacturer(UpnpDeviceConfig *thiz, const char *manufacturer)
{
    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(manufacturer);

    strncpy(thiz->manufacturer, manufacturer, LEN);
}

void UpnpDeviceConfig_SetManufacturerUrl(UpnpDeviceConfig *thiz, const char *url)
{
    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(url);

    strncpy(thiz->manufacturerUrl, url, LEN);
}

UpnpDevice * UpnpDeviceConfig_CreateDevice(UpnpDeviceConfig *thiz)
{
    UpnpDevice * device = NULL;

    RETURN_VAL_IF_FAIL(thiz, NULL);

    do
    {
        device = UpnpDevice_New();
        if (device == NULL)
        {
            LOG_D(TAG, "UpnpDevice_New failed");
            break;
        }
        
        // UpnpDevice_SetPropertyValue(thiz, UPNP_DEVICE_DeviceType, deviceType);
        UpnpDevice_SetPropertyValue(device, UPNP_DEVICE_FriendlyName, thiz->deviceName);
        UpnpDevice_SetPropertyValue(device, UPNP_DEVICE_ModelNumber, thiz->modelNumber);
        UpnpDevice_SetPropertyValue(device, UPNP_DEVICE_ModelName, thiz->modelName);
        UpnpDevice_SetPropertyValue(device, UPNP_DEVICE_ModelURL, thiz->modelUrl);
        UpnpDevice_SetPropertyValue(device, UPNP_DEVICE_Manufacturer, thiz->manufacturer);
        UpnpDevice_SetPropertyValue(device, UPNP_DEVICE_ManufacturerURL, thiz->manufacturerUrl);

        if (!generateNewDeviceId(device))
        {
            LOG_D(TAG, "generateNewDeviceId failed");
            UpnpDevice_Delete(device);
            device = NULL;
            break;
        }
    } while (0);

    return device;
}

static bool generateNewDeviceId(UpnpDevice * device)
{
    TinyUuid uuid;

    if (RET_FAILED(TinyUuid_Construct(&uuid)))
    {
        LOG_E(TAG, "TinyUuid_Construct failed");
        return false;
    }

    if (RET_FAILED(TinyUuid_GenerateRandom(&uuid)))
    {
        LOG_E(TAG, "TinyUuid_GenerateRandom failed");
        TinyUuid_Dispose(&uuid);
        return false;
    }

    UpnpDevice_SetPropertyValue(device, UPNP_DEVICE_UDN, TinyUuid_ToString(&uuid, true));

    TinyUuid_Dispose(&uuid);

    return true;
}