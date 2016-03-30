/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   UpnpDevice.c
*
* @remark
*
*/

#include "UpnpDevice.h"
#include "PropertyList.h"
#include "UpnpServiceList.h"
#include "tiny_memory.h"

static TinyRet UpnpDevice_Construct(UpnpDevice *thiz);
static void UpnpDevice_Dispose(UpnpDevice *thiz);

#define UDN_LEN                 128
#define FriendlyName_LEN        128
#define DeviceType_LEN          128
#define Manufacturer_LEN        128
#define ModelName               128
#define ModelNumber_LEN         128
#define SerialNumber_LEN        128

struct _UpnpDevice
{
    uint16_t port;
    char Address[TINY_IP_LEN];
    char uri[TINY_URI_LEN];
    char deviceId[UDN_LEN];
    char friendlyName[FriendlyName_LEN];
    char deviceType[DeviceType_LEN];
    char manufacturer[Manufacturer_LEN];
    char manufacturerURL[TINY_URL_LEN];
    char modelName[ModelName];
    char modelNumber[ModelNumber_LEN];
    char modelURL[TINY_URL_LEN];
    char serialNumber[SerialNumber_LEN];
    char URLBase[TINY_URL_LEN];

    UpnpServiceList * serviceList;
};

UpnpDevice * UpnpDevice_New(void)
{
    UpnpDevice *thiz = NULL;

    do
    {
        TinyRet ret = TINY_RET_OK;

        thiz = (UpnpDevice *)tiny_malloc(sizeof(UpnpDevice));
        if (thiz == NULL)
        {
            break;
        }

        ret = UpnpDevice_Construct(thiz);
        if (RET_FAILED(ret))
        {
            UpnpDevice_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

void UpnpDevice_Delete(UpnpDevice *thiz)
{
    RETURN_IF_FAIL(thiz);

    UpnpDevice_Dispose(thiz);
    tiny_free(thiz);
}

static TinyRet UpnpDevice_Construct(UpnpDevice *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(UpnpDevice));
        thiz->port = 0;

        thiz->serviceList = UpnpServiceList_New();
        if (thiz->serviceList == NULL)
        {
            ret = TINY_RET_E_NEW;
            break;
        }
    } while (0);

    return ret;
}

static void UpnpDevice_Dispose(UpnpDevice *thiz)
{
    RETURN_IF_FAIL(thiz);

    UpnpServiceList_Delete(thiz->serviceList);
}

UpnpServiceList * UpnpDevice_GetServiceList(UpnpDevice *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->serviceList;
}

UpnpService * UpnpDevice_GetService(UpnpDevice *thiz, const char *serviceId)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);
    RETURN_VAL_IF_FAIL(serviceId, NULL);

    return UpnpServiceList_GetService(thiz->serviceList, serviceId);
}

TinyRet UpnpDevice_SetHttpPort(UpnpDevice *thiz, uint16_t port)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    thiz->port = port;

    return TINY_RET_OK;
}

TinyRet UpnpDevice_SetAddress(UpnpDevice *thiz, const char *Address)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(Address, TINY_RET_E_ARG_NULL);

    strncpy(thiz->Address, Address, TINY_IP_LEN);

    return TINY_RET_OK;
}

TinyRet UpnpDevice_SetURI(UpnpDevice *thiz, const char *URI)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(URI, TINY_RET_E_ARG_NULL);

    strncpy(thiz->uri, URI, TINY_URI_LEN);

    return TINY_RET_OK;
}

TinyRet UpnpDevice_SetDeviceId(UpnpDevice *thiz, const char *deviceId)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(deviceId, TINY_RET_E_ARG_NULL);

    strncpy(thiz->deviceId, deviceId, UDN_LEN);

    return TINY_RET_OK;
}

TinyRet UpnpDevice_SetFriendlyName(UpnpDevice *thiz, const char *friendlyName)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(friendlyName, TINY_RET_E_ARG_NULL);

    strncpy(thiz->friendlyName, friendlyName, FriendlyName_LEN);

    return TINY_RET_OK;
}

TinyRet UpnpDevice_SetDeviceType(UpnpDevice *thiz, const char *deviceType)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(deviceType, TINY_RET_E_ARG_NULL);

    strncpy(thiz->deviceType, deviceType, DeviceType_LEN);

    return TINY_RET_OK;
}

TinyRet UpnpDevice_SetManufacturer(UpnpDevice *thiz, const char *manufacturer)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(manufacturer, TINY_RET_E_ARG_NULL);

    strncpy(thiz->manufacturer, manufacturer, Manufacturer_LEN);

    return TINY_RET_OK;
}

TinyRet UpnpDevice_SetManufacturerURL(UpnpDevice *thiz, const char *manufacturerURL)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(manufacturerURL, TINY_RET_E_ARG_NULL);

    strncpy(thiz->manufacturerURL, manufacturerURL, Manufacturer_LEN);

    return TINY_RET_OK;
}

TinyRet UpnpDevice_SetModelName(UpnpDevice *thiz, const char *modelName)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(modelName, TINY_RET_E_ARG_NULL);

    strncpy(thiz->modelName, modelName, Manufacturer_LEN);

    return TINY_RET_OK;
}

TinyRet UpnpDevice_SetModelNumber(UpnpDevice *thiz, const char *modelNumber)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(modelNumber, TINY_RET_E_ARG_NULL);

    strncpy(thiz->modelNumber, modelNumber, Manufacturer_LEN);

    return TINY_RET_OK;
}

TinyRet UpnpDevice_SetModelURL(UpnpDevice *thiz, const char *modelURL)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(modelURL, TINY_RET_E_ARG_NULL);

    strncpy(thiz->modelURL, modelURL, Manufacturer_LEN);

    return TINY_RET_OK;
}

TinyRet UpnpDevice_SetSerialNumber(UpnpDevice *thiz, const char *serialNumber)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(serialNumber, TINY_RET_E_ARG_NULL);

    strncpy(thiz->serialNumber, serialNumber, Manufacturer_LEN);

    return TINY_RET_OK;
}

TinyRet UpnpDevice_SetURLBase(UpnpDevice *thiz, const char *URLBase)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(URLBase, TINY_RET_E_ARG_NULL);

    strncpy(thiz->URLBase, URLBase, Manufacturer_LEN);

    return TINY_RET_OK;
}

uint16_t UpnpDevice_GetHttpPort(UpnpDevice *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, 0);

    return thiz->port;
}

const char * UpnpDevice_GetAddress(UpnpDevice *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->Address;
}

const char * UpnpDevice_GetURI(UpnpDevice *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->uri;
}

const char * UpnpDevice_GetDeviceId(UpnpDevice *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->deviceId;
}

const char * UpnpDevice_GetFriendlyName(UpnpDevice *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->friendlyName;
}

const char * UpnpDevice_GetDeviceType(UpnpDevice *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->deviceType;
}

const char * UpnpDevice_GetManufacturer(UpnpDevice *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->manufacturer;
}

const char * UpnpDevice_GetManufacturerURL(UpnpDevice *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->manufacturerURL;
}

const char * UpnpDevice_GetModelName(UpnpDevice *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->modelName;
}

const char * UpnpDevice_GetModelNumber(UpnpDevice *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->modelNumber;
}

const char * UpnpDevice_GetModelURL(UpnpDevice *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->modelURL;
}

const char * UpnpDevice_GetSerialNumber(UpnpDevice *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->serialNumber;
}

const char * UpnpDevice_GetURLBase(UpnpDevice *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->URLBase;
}