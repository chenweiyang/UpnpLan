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
#include "UpnpServiceDefinition.h"
#include "UpnpActionDefinition.h"

#define TAG             "SwitchPower"

static const char * _SERVICE_TYPE = "urn:schemas-upnp-org:service:SwitchPower:1";

//-------------------------------------------------------
// action names (3)
//-------------------------------------------------------
static const char * ACTION_GetTarget = "GetTarget";
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

static UpnpCode handle_GetTarget(SwitchPower *thiz, UpnpAction *action);
static UpnpCode handle_SetTarget(SwitchPower *thiz, UpnpAction *action);
static UpnpCode handle_GetStatus(SwitchPower *thiz, UpnpAction *action);

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

bool SwitchPower_IsImplemented(SwitchPower *thiz, UpnpService *service)
{
    RETURN_VAL_IF_FAIL(thiz, false);
    RETURN_VAL_IF_FAIL(service, false);

    return STR_EQUAL(UpnpService_GetPropertyValue(service, UPNP_SERVICE_ServiceType), _SERVICE_TYPE);
}

TinyRet SwitchPower_SetHandler(SwitchPower *thiz, SwitchPower_ActionHandler *handler, void *ctx)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    thiz->OnGetTarget = handler->OnGetTarget;
    thiz->OnSetTarget = handler->OnSetTarget;
    thiz->OnGetStatus = handler->OnGetStatus;
    thiz->ctx = ctx;

    return ret;  
}

UpnpCode SwitchPower_OnAction(SwitchPower *thiz, UpnpAction *action)
{
    const char *actionName = NULL;

    RETURN_VAL_IF_FAIL(thiz, UPNP_ERR_INVALID_ARGS);
    RETURN_VAL_IF_FAIL(action, UPNP_ERR_INVALID_ARGS);

    actionName = UpnpAction_GetPropertyValue(action, UPNP_ACTION_Name);
    LOG_D(TAG, "SwitchPower_OnAction: %s", actionName);

    if (STR_EQUAL(actionName, ACTION_GetTarget))
    {
        return handle_GetTarget(thiz, action);
    }

    if (STR_EQUAL(actionName, ACTION_SetTarget))
    {
        return handle_SetTarget(thiz, action);
    }

    if (STR_EQUAL(actionName, ACTION_GetStatus))
    {
        return handle_GetStatus(thiz, action);
    }

    return UPNP_ERR_ACTION_FAILED;
}

TinyRet SwitchPower_SendEvents(SwitchPower *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    // UpnpService_SendEvents(thiz->service);
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

static UpnpCode handle_GetTarget(SwitchPower *thiz, UpnpAction *action)
{
    UpnpCode code = UPNP_SUCCESS;

    do
    {
        SwitchPower_GetTargetResult result;

        /**
        * Argument IN (0)
        */

        memset(&result, 0, sizeof(SwitchPower_GetTargetResult));

        code = thiz->OnGetTarget(thiz, &result, thiz->ctx);
        if (code != UPNP_SUCCESS)
        {
            break;
        }

        /**
        * Argument OUT (1)
        */
        PropertyList *_out = UpnpAction_GetResultList(action);
        Property *_RetTargetValue = PropertyList_GetProperty(_out, _GetTarget_ARG_RetTargetValue);
        if (_RetTargetValue == NULL)
        {
            LOG_E(TAG, "Result invalid: %s NOT FOUND!", _GetTarget_ARG_RetTargetValue);
            break;
        }

        _RetTargetValue->value.object.value.boolValue = result.theTargetValue;
    } while (0);

    return code;
}

static UpnpCode handle_SetTarget(SwitchPower *thiz, UpnpAction *action)
{
    UpnpCode code = UPNP_SUCCESS;

    do
    {
        /**
        * Argument IN (1)
        */
        PropertyList *_in = UpnpAction_GetArgumentList(action);
        Property *_newTargetValue = PropertyList_GetProperty(_in, _SetTarget_ARG_newTargetValue);
        if (_newTargetValue == NULL)
        {
            LOG_E(TAG, "argument invalid: %s NOT FOUND!", _SetTarget_ARG_newTargetValue);
            break;
        }

        code = thiz->OnSetTarget(thiz, _newTargetValue->value.object.value.boolValue, thiz->ctx);
        if (code != UPNP_SUCCESS)
        {
            break;
        }

        /**
        * Argument OUT (0)
        */
    } while (0);

    return code;
}

static UpnpCode handle_GetStatus(SwitchPower *thiz, UpnpAction *action)
{
    UpnpCode code = UPNP_SUCCESS;

    do
    {
        SwitchPower_GetStatusResult result;

        /**
        * Argument IN (0)
        */

        memset(&result, 0, sizeof(SwitchPower_GetStatusResult));

        code = thiz->OnGetStatus(thiz, &result, thiz->ctx);
        if (code != UPNP_SUCCESS)
        {
            break;
        }

        /**
        * Argument OUT (1)
        */
        PropertyList *_out = UpnpAction_GetResultList(action);
        Property *_ResultStatus = PropertyList_GetProperty(_out, _GetStatus_ARG_ResultStatus);
        if (_ResultStatus == NULL)
        {
            LOG_E(TAG, "Result invalid: %s NOT FOUND!", _GetStatus_ARG_ResultStatus);
            break;
        }

        _ResultStatus->value.object.value.boolValue = result.theResultStatus;
    } while (0);

    return code;
}