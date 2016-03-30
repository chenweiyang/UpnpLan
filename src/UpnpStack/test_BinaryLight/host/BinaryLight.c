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
#include "tiny_memory.h"
#include "tiny_log.h"

#define TAG "BinaryLight"

static const char * _ID_SwitchPower = "urn:upnp-org:serviceId:SwitchPower";

static TinyRet BinaryLight_Construct(BinaryLight *thiz, UpnpDeviceConfig *config, UpnpRuntime *runtime);
static void BinaryLight_Dispose(BinaryLight *thiz);
static uint32_t action_handler(UpnpAction *action, void *ctx);

struct _BinaryLight
{
    UpnpDevice *device;
    UpnpRuntime *runtime;
    SwitchPower *switchPower;
};

BinaryLight * BinaryLight_New(UpnpDeviceConfig *config, UpnpRuntime *runtime)
{
    BinaryLight * thiz = NULL;

    RETURN_VAL_IF_FAIL(config, NULL);
    RETURN_VAL_IF_FAIL(runtime, NULL);

    do
    {
        thiz = (BinaryLight *)tiny_malloc(sizeof(BinaryLight));
        if (thiz == NULL)
        {
            LOG_E(TAG, "OUT OF MEMORY!");
            break;
        }

        if (RET_FAILED(BinaryLight_Construct(thiz, config, runtime)))
        {
            LOG_E(TAG, "BinaryLight_Construct failed");
            BinaryLight_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

static TinyRet BinaryLight_Construct(BinaryLight *thiz, UpnpDeviceConfig *config, UpnpRuntime *runtime)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(BinaryLight));
        thiz->runtime = runtime;

        thiz->device = UpnpDeviceConfig_CreateDevice(config, BINARYLIGHT_DEVICE_TYPE);
        if (thiz->device == NULL)
        {
            LOG_E(TAG, "UpnpDeviceConfig_CreateDevice failed");
            ret = TINY_RET_E_CONSTRUCT;
            break;
        }

        thiz->switchPower = SwitchPower_Create(thiz->device, runtime);
        if (thiz->switchPower == NULL)
        {
            LOG_E(TAG, "SwitchPower_Create failed");
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

    if (thiz->device != NULL)
    {
        UpnpDevice_Delete(thiz->device);
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

TinyRet BinaryLight_Start(BinaryLight *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    return UpnpRuntime_Register(thiz->runtime, thiz->device, action_handler, thiz);
}

TinyRet BinaryLight_Stop(BinaryLight *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    return UpnpRuntime_Unregister(thiz->runtime, UpnpDevice_GetDeviceId(thiz->device));
}

static uint32_t action_handler(UpnpAction *action, void *ctx)
{
    BinaryLight *thiz = (BinaryLight *)ctx;

    LOG_D(TAG, "action_handler");

    if (SwitchPower_IsImplemented(thiz->switchPower, (UpnpService *)UpnpAction_GetParentService(action)))
    {
        return SwitchPower_OnAction(thiz->switchPower, action);
    }

    return UPNP_ERR_INVALID_ACTION;
}