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
#include "UpnpServiceHelper.h"

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

static TinyRet init_service(SwitchPower *thiz, UpnpDevice *device);
static TinyRet init_stateList(UpnpService *service);
static TinyRet init_actionList(UpnpService *service);

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
        thiz->OnGetTarget = NULL;
        thiz->OnSetTarget = NULL;
        thiz->OnGetStatus = NULL;
        thiz->ctx = NULL;

        ret = init_service(thiz, device);
        if (RET_FAILED(ret))
        {
            break;
        }
    } while (0);

    return ret;
}

static void SwitchPower_Dispose(SwitchPower *thiz)
{
    RETURN_IF_FAIL(thiz);

    /**
     * DO NOT delete thiz->service !!!
     */
}

void SwitchPower_Delete(SwitchPower *thiz)
{
    RETURN_IF_FAIL(thiz);

    SwitchPower_Dispose(thiz);
    tiny_free(thiz);
}

static TinyRet init_service(SwitchPower *thiz, UpnpDevice *device)
{
    TinyRet ret = TINY_RET_OK;

    do
    {
        thiz->service = UpnpService_New();
        if (thiz->service == NULL)
        {
            ret = TINY_RET_E_NEW;
            break;
        }

        UpnpService_SetParentDevice(thiz->service, device);
        UpnpService_SetServiceType(thiz->service, _SERVICE_TYPE);

        ret = UpnpServiceHelper_UpdateServiceId(thiz->service);
        if (RET_FAILED(ret))
        {
            UpnpService_Delete(thiz->service);
            thiz->service = NULL;
            break;
        }

        ret = UpnpServiceHelper_UpdateScpdUrl(thiz->service);
        if (RET_FAILED(ret))
        {
            UpnpService_Delete(thiz->service);
            thiz->service = NULL;
            break;
        }

        ret = UpnpServiceHelper_UpdateCtrlUrl(thiz->service);
        if (RET_FAILED(ret))
        {
            UpnpService_Delete(thiz->service);
            thiz->service = NULL;
            break;
        }

        ret = UpnpServiceHelper_UpdateEventUrl(thiz->service);
        if (RET_FAILED(ret))
        {
            UpnpService_Delete(thiz->service);
            thiz->service = NULL;
            break;
        }

        ret = init_stateList(thiz->service);
        if (RET_FAILED(ret))
        {
            UpnpService_Delete(thiz->service);
            thiz->service = NULL;
            break;
        }

        ret = init_actionList(thiz->service);
        if (RET_FAILED(ret))
        {
            UpnpService_Delete(thiz->service);
            thiz->service = NULL;
            break;
        }

        ret = UpnpDevice_AddService(device, thiz->service);
        if (RET_FAILED(ret))
        {
            break;
        }
    } while (0);

    return ret;
}

static TinyRet init_stateList(UpnpService *service)
{
    TinyRet ret = TINY_RET_OK;

    do
    {
        UpnpStateVariable * _Status = UpnpStateVariable_New();
        if (_Status == NULL)
        {
            ret = TINY_RET_E_NEW;
            break;
        }

        ret = UpnpStateVariable_Initialize(_Status, PROPERTY_Status, "string", NULL, "YES");
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = UpnpService_AddStateVariable(service, _Status);
        if (RET_FAILED(ret))
        {
            break;
        }
    } while (0);

    do
    {
        UpnpStateVariable * _Target = UpnpStateVariable_New();
        if (_Target == NULL)
        {
            ret = TINY_RET_E_NEW;
            break;
        }

        ret = UpnpStateVariable_Initialize(_Target, PROPERTY_Target, "string", NULL, "YES");
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = UpnpService_AddStateVariable(service, _Target);
        if (RET_FAILED(ret))
        {
            break;
        }
    } while (0);

    return ret;
}

static TinyRet init_actionList(UpnpService *service)
{
    TinyRet ret = TINY_RET_OK;

    /**
     * GetTarget
     */
    do
    {
        UpnpAction * _GetTarget = NULL;
        UpnpArgument * _RetTargetValue = NULL;

        _GetTarget = UpnpAction_New();
        if (_GetTarget == NULL)
        {
            ret = TINY_RET_E_NEW;
            break;
        }

        ret = UpnpAction_SetName(_GetTarget, ACTION_GetTarget);
        if (RET_FAILED(ret))
        {
            break;
        }

        _RetTargetValue = UpnpArgument_New(_GetTarget_ARG_RetTargetValue, ARG_OUT, PROPERTY_Target);
        if (_RetTargetValue == NULL)
        {
            ret = TINY_RET_E_NEW;
            break;
        }

        ret = UpnpAction_AddArgument(_GetTarget, _RetTargetValue);
        if (RET_FAILED(ret))
        {
            break;
        }
    } while (0);

    /**
     * SetTarget
     */
    do {
        UpnpAction * _SetTarget = NULL;
        UpnpArgument * _NewTargetValue = NULL;

        _SetTarget = UpnpAction_New();
        if (_SetTarget == NULL)
        {
            ret = TINY_RET_E_NEW;
            break;
        }

        ret = UpnpAction_SetName(_SetTarget, ACTION_SetTarget);
        if (RET_FAILED(ret))
        {
            break;
        }

        _NewTargetValue = UpnpArgument_New(_SetTarget_ARG_newTargetValue, ARG_IN, PROPERTY_Target);
        if (_NewTargetValue == NULL)
        {
            ret = TINY_RET_E_NEW;
            break;
        }

        ret = UpnpAction_AddArgument(_SetTarget, _NewTargetValue);
        if (RET_FAILED(ret))
        {
            break;
        }
    } while (0);

    /**
     * GetStatus
     */
    do
    {
        UpnpAction * _GetStatus = NULL;
        UpnpArgument * _ResultStatus = NULL;

        _GetStatus = UpnpAction_New();
        if (_GetStatus == NULL)
        {
            ret = TINY_RET_E_NEW;
            break;
        }

        ret = UpnpAction_SetName(_GetStatus, ACTION_GetStatus);
        if (RET_FAILED(ret))
        {
            break;
        }

        _ResultStatus = UpnpArgument_New(_GetStatus_ARG_ResultStatus, ARG_OUT, PROPERTY_Status);
        if (_ResultStatus == NULL)
        {
            ret = TINY_RET_E_NEW;
            break;
        }

        ret = UpnpAction_AddArgument(_GetStatus, _ResultStatus);
        if (RET_FAILED(ret))
        {
            break;
        }
    } while (false);

    return ret;
}

bool SwitchPower_IsImplemented(SwitchPower *thiz, UpnpService *service)
{
    RETURN_VAL_IF_FAIL(thiz, false);
    RETURN_VAL_IF_FAIL(service, false);

    return STR_EQUAL(UpnpService_GetServiceType(service), _SERVICE_TYPE);
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

    actionName = UpnpAction_GetName(action);
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

    return UpnpService_SendEvents(thiz->service);
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
        UpnpStateVariable * _RetTargetValue = UpnpService_GetStateVariable(thiz->service, UpnpAction_GetArgumentRelatedStateVariable(action, _GetTarget_ARG_RetTargetValue));
        if (_RetTargetValue == NULL)
        {
            LOG_E(TAG, "Result invalid: %s NOT FOUND!", _GetTarget_ARG_RetTargetValue);
            break;
        }

        _RetTargetValue->value.internalValue.boolValue = result.theTargetValue;
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
        UpnpStateVariable * _newTargetValue = UpnpService_GetStateVariable(thiz->service, UpnpAction_GetArgumentRelatedStateVariable(action, _SetTarget_ARG_newTargetValue));
        if (_newTargetValue == NULL)
        {
            LOG_E(TAG, "argument invalid: %s NOT FOUND!", _SetTarget_ARG_newTargetValue);
            break;
        }

        code = thiz->OnSetTarget(thiz, _newTargetValue->value.internalValue.boolValue, thiz->ctx);
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
        UpnpStateVariable * _ResultStatus = UpnpService_GetStateVariable(thiz->service, UpnpAction_GetArgumentRelatedStateVariable(action, _GetStatus_ARG_ResultStatus));
        if (_ResultStatus == NULL)
        {
            LOG_E(TAG, "Result invalid: %s NOT FOUND!", _GetStatus_ARG_ResultStatus);
            break;
        }

        _ResultStatus->value.internalValue.boolValue = result.theResultStatus;
    } while (0);

    return code;
}