/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   SwitchPower.c
*
* @remark
*
*/

#include "SwitchPower.h"
#include "tiny_memory.h"
#include "tiny_str_equal.h"
#include "tiny_log.h"

#define TAG             "SwitchPower"

static const char * _SERVICE_TYPE = "urn:schemas-upnp-org:service:SwitchPower:1";

//-------------------------------------------------------
// action names (3)
//-------------------------------------------------------
static const char * action_gettarget = "gettarget";
static const char * _GetTarget_ARG_RetTargetValue = "RetTargetValue";
static const char * ACTION_SetTarget = "SetTarget";
static const char * _SetTarget_ARG_newTargetValue = "newTargetValue";
static const char * ACTION_GetStatus = "GetStatus";
static const char * _GetStatus_ARG_ResultStatus = "ResultStatus";

//-------------------------------------------------------
// Property Name (2)
//-------------------------------------------------------
static const char * PROPERTY_Status = "Status";
static const char * PROPERTY_Target = "Target";

//-------------------------------------------------------
// Property value defined (0)
//-------------------------------------------------------

static TinyRet SwitchPower_Construct(SwitchPower *thiz, UpnpDevice *device, UpnpRuntime *runtime);
static void SwitchPower_Dispose(SwitchPower *thiz);

struct _SwitchPower
{
    UpnpRuntime *runtime;
    UpnpService *service;
    SwitchPower_OnGetTarget OnGetTarget;
    SwitchPower_OnSetTarget OnSetTarget;
    SwitchPower_OnGetStatus OnGetStatus;
    void *ctx;
};

SwitchPower * SwitchPower_Create(UpnpDevice *device, UpnpRuntime *runtime)
{
    SwitchPower * thiz = NULL;

    RETURN_VAL_IF_FAIL(device, NULL);
    RETURN_VAL_IF_FAIL(runtime, NULL);

    do
    {
        thiz = (SwitchPower *)tiny_malloc(sizeof(SwitchPower));
        if (thiz == NULL)
        {
            LOG_E(TAG, "OUT OF MEMORY!");
            break;
        }

        if (RET_FAILED(SwitchPower_Construct(thiz, device, runtime)))
        {
            LOG_E(TAG, "SwitchPower_Construct failed");
            SwitchPower_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

static TinyRet SwitchPower_Construct(SwitchPower *thiz, UpnpDevice *device, UpnpRuntime *runtime)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(SwitchPower));
        thiz->runtime = runtime;
        thiz->service = NULL;
        thiz->OnGetTarget = NULL;
        thiz->OnSetTarget = NULL;
        thiz->OnGetStatus = NULL;
        thiz->ctx = NULL;

        /**
         * create UpnpService
         */
    } while (0);

    return ret;
}

static void SwitchPower_Dispose(SwitchPower *thiz)
{
    RETURN_IF_FAIL(thiz);
}

void SwitchPower_Delete(SwitchPower *thiz)
{
    RETURN_IF_FAIL(thiz);

    SwitchPower_Dispose(thiz);
    tiny_free(thiz);
}

TinyRet SwitchPower_SetHandler(SwitchPower *thiz, SwitchPower_ActionHandler handler, void *ctx)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    thiz->OnGetTarget = handler.OnGetTarget;
    thiz->OnSetTarget = handler.OnSetTarget;
    thiz->OnGetStatus = handler.OnGetStatus;
    thiz->ctx = ctx;

    return ret;  
}

TinyRet SwitchPower_SendEvents(SwitchPower *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    return ret;  
}

TinyRet SwitchPower_SetStatus(SwitchPower *thiz, bool theStatus)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    return ret;  
}

TinyRet SwitchPower_SetTarget(SwitchPower *thiz, bool theTarget)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    return ret;  
}
