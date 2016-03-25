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
#include "UpnpDeviceDefinition.h"
#include "PropertyList.h"
#include "UpnpServiceList.h"
#include "tiny_memory.h"

static TinyRet UpnpDevice_Construct(UpnpDevice *thiz);
static void UpnpDevice_Dispose(UpnpDevice *thiz);
static TinyRet UpnpDevice_Initialize(UpnpDevice *thiz);

struct _UpnpDevice
{
    PropertyList * propertyList;
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

        ret = UpnpDevice_Initialize(thiz);
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

        thiz->propertyList = PropertyList_New();
        if (thiz->propertyList == NULL)
        {
            ret = TINY_RET_E_NEW;
            break;
        }

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

    PropertyList_Delete(thiz->propertyList);
    UpnpServiceList_Delete(thiz->serviceList);
}

static TinyRet UpnpDevice_Initialize(UpnpDevice *thiz)
{
    TinyRet ret = TINY_RET_OK;
    ObjectType type;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    ObjectType_Construct(&type);

    do
    {
        ObjectType_SetType(&type, CLAZZ_STRING);

        ret = PropertyList_InitProperty(thiz->propertyList, UPNP_DEVICE_UDN, &type);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = PropertyList_InitProperty(thiz->propertyList, UPNP_DEVICE_FriendlyName, &type);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = PropertyList_InitProperty(thiz->propertyList, UPNP_DEVICE_DeviceType, &type);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = PropertyList_InitProperty(thiz->propertyList, UPNP_DEVICE_Manufacturer, &type);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = PropertyList_InitProperty(thiz->propertyList, UPNP_DEVICE_ManufacturerURL, &type);
        if (RET_FAILED(ret))
        {
            break;
        }
        ret = PropertyList_InitProperty(thiz->propertyList, UPNP_DEVICE_ModelName, &type);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = PropertyList_InitProperty(thiz->propertyList, UPNP_DEVICE_ModelNumber, &type);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = PropertyList_InitProperty(thiz->propertyList, UPNP_DEVICE_ModelURL, &type);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = PropertyList_InitProperty(thiz->propertyList, UPNP_DEVICE_SerialNumber, &type);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = PropertyList_InitProperty(thiz->propertyList, UPNP_DEVICE_URLBase, &type);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = PropertyList_InitProperty(thiz->propertyList, UPNP_DEVICE_Address, &type);
        if (RET_FAILED(ret))
        {
            break;
        }
    } while (0);

    ObjectType_Dispose(&type);

    return ret;
}

UpnpServiceList * UpnpDevice_GetServiceList(UpnpDevice *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->serviceList;
}

PropertyList * UpnpDevice_GetPropertyList(UpnpDevice *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->propertyList;
}

TinyRet UpnpDevice_SetPropertyValue(UpnpDevice *thiz, const char *propertyName, const char *value)
{
    TinyRet ret = TINY_RET_OK;
    Object data;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(propertyName, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(value, TINY_RET_E_ARG_NULL);

    Object_Construct(&data); 
    {
        Object_setString(&data, value);
        ret = PropertyList_SetPropertyValue(thiz->propertyList, propertyName, &data);
    }
    Object_Dispose(&data);

    return ret;
}

const char * UpnpDevice_GetPropertyValue(UpnpDevice *thiz, const char *propertyName)
{
    const char *value = NULL;
    Object *data = NULL;

    RETURN_VAL_IF_FAIL(thiz, NULL);
    RETURN_VAL_IF_FAIL(propertyName, NULL);

    data = PropertyList_GetPropertyValue(thiz->propertyList, propertyName);
    if (data != NULL)
    {
        value = data->value.stringValue;
    }

    return value;
}
