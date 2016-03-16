/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   BinaryLight.c
*
* @remark
*
*/

#include "BinaryLight.h"
#include "SwitchPower.h"
#include "UpnpDeviceDefinition.h"
#include "tiny_memory.h"
#include "tiny_log.h"

#define TAG             "BinaryLight"

static const char * _ID_SwitchPower = "urn:upnp-org:serviceId:SwitchPower";

static TinyRet BinaryLight_Construct(BinaryLight *thiz, UpnpDevice *device, UpnpRuntime *runtime);
static void BinaryLight_Dispose(BinaryLight *thiz);

struct _BinaryLight
{
    UpnpDevice *device;
    UpnpRuntime *runtime;
    SwitchPower *switchPower;
};

BinaryLight * BinaryLight_Create(UpnpDevice *device, UpnpRuntime *runtime)
{
    BinaryLight * thiz = NULL;

    RETURN_VAL_IF_FAIL(device, NULL);
    RETURN_VAL_IF_FAIL(runtime, NULL);

    do
    {
        thiz = (BinaryLight *)tiny_malloc(sizeof(BinaryLight));
        if (thiz == NULL)
        {
            LOG_E(TAG, "OUT OF MEMORY!");
            break;
        }

        if (RET_FAILED(BinaryLight_Construct(thiz, device, runtime)))
        {
            LOG_E(TAG, "BinaryLight_Construct failed");
            BinaryLight_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

static TinyRet BinaryLight_Construct(BinaryLight *thiz, UpnpDevice *device, UpnpRuntime *runtime)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        UpnpServiceList * _list = NULL;
        UpnpService * _SwitchPower = NULL;

        memset(thiz, 0, sizeof(BinaryLight));
        thiz->device = device;
        thiz->runtime = runtime;

        if (!STR_EQUAL(BINARYLIGHT_DEVICE_TYPE, UpnpDevice_GetPropertyValue(device, UPNP_DEVICE_DeviceType)))
        {
            LOG_E(TAG, "DEVICE_TYPE invalid: %s", UpnpDevice_GetPropertyValue(device, UPNP_DEVICE_DeviceType));
            ret = TINY_RET_E_CONSTRUCT;
            break;
        }

        _list = UpnpDevice_GetServiceList(device);
        _SwitchPower = UpnpServiceList_GetService(_list, _ID_SwitchPower);
        if (_SwitchPower == NULL)
        {
            LOG_E(TAG, "Service not found: %s", _ID_SwitchPower);
            ret = TINY_RET_E_CONSTRUCT;
            break;
        }

        thiz->switchPower = SwitchPower_Create(_SwitchPower, runtime);
        if (thiz->switchPower == NULL)
        {
            LOG_E(TAG, "SwitchPower_Create: failed");
            ret = TINY_RET_E_CONSTRUCT;
            break;
        }
    } while (0);

    return ret;
}

static void BinaryLight_Dispose(BinaryLight *thiz)
{
    RETURN_IF_FAIL(thiz);

    if (thiz->switchPower != NULL)
    {
        SwitchPower_Delete(thiz->switchPower);
    }
}

void BinaryLight_Delete(BinaryLight *thiz)
{
    RETURN_IF_FAIL(thiz);

    BinaryLight_Dispose(thiz);
    tiny_free(thiz);
}

const char * BinaryLight_GetDeviceType(BinaryLight *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return BINARYLIGHT_DEVICE_TYPE;
}

SwitchPower * BinaryLight_GetSwitchPower(BinaryLight *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->switchPower;
}