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
        UpnpServiceList * serviceList = NULL;

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

        serviceList = UpnpDevice_GetServiceList(device);
        ret = UpnpServiceList_AddService(serviceList, thiz->service);
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
        UpnpStateList * stateList = UpnpService_GetStateList(service);
        ObjectType dataType;

        /* Status */
        ObjectType_SetName(&dataType, "string");
        ret = UpnpStateList_InitState(stateList, PROPERTY_Status, &dataType, true, service);
        if (RET_FAILED(ret))
        {
            break;
        }

        /* Target */
        ObjectType_SetName(&dataType, "string");
        ret = UpnpStateList_InitState(stateList, PROPERTY_Target, &dataType, true, service);
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

    do
    {
        UpnpActionList * actionList = UpnpService_GetActionList(service);
        UpnpStateList * stateList = UpnpService_GetStateList(service);

        UpnpAction *GetTarget = NULL;
        UpnpAction *SetTarget = NULL;
        UpnpAction *GetStatus = NULL;

        PropertyList *argList = NULL;
        PropertyList *resultList = NULL;
        UpnpState * relatedState;

        /** 
         * GetTarget
         */
        GetTarget = UpnpAction_New();

        argList = UpnpAction_GetArgumentList(GetTarget);
        resultList = UpnpAction_GetResultList(GetTarget);
        UpnpAction_SetParentService(GetTarget, service);
        UpnpAction_SetName(GetTarget, ACTION_GetTarget);

        relatedState = UpnpStateList_GetState(stateList, PROPERTY_Target);
        if (relatedState == NULL)
        {
            LOG_D(TAG, "UpnpStateList_GetState failed: <%s>", PROPERTY_Target);
        }

        PropertyList_InitProperty(resultList, _GetTarget_ARG_RetTargetValue, &relatedState->definition.type);

        ret = UpnpActionList_AddAction(actionList, GetTarget);
        if (RET_FAILED(ret))
        {
            LOG_D(TAG, "UpnpActionList_AddAction failed: <%s>", ACTION_GetTarget);
        }

        /**
        * SetTarget
        */
        SetTarget = UpnpAction_New();

        argList = UpnpAction_GetArgumentList(SetTarget);
        resultList = UpnpAction_GetResultList(SetTarget);
        UpnpAction_SetParentService(SetTarget, service);
        UpnpAction_SetName(SetTarget, ACTION_SetTarget);

        relatedState = UpnpStateList_GetState(stateList, PROPERTY_Target);
        if (relatedState == NULL)
        {
            LOG_D(TAG, "UpnpStateList_GetState failed: <%s>", PROPERTY_Target);
        }

        PropertyList_InitProperty(argList, _GetTarget_ARG_RetTargetValue, &relatedState->definition.type);

        ret = UpnpActionList_AddAction(actionList, SetTarget);
        if (RET_FAILED(ret))
        {
            LOG_D(TAG, "UpnpActionList_AddAction failed: <%s>", ACTION_SetTarget);
        }

        /**
        * GetStatus
        */
        GetStatus = UpnpAction_New();

        argList = UpnpAction_GetArgumentList(GetStatus);
        resultList = UpnpAction_GetResultList(GetStatus);
        UpnpAction_SetParentService(GetStatus, service);
        UpnpAction_SetName(GetStatus, ACTION_GetStatus);

        relatedState = UpnpStateList_GetState(stateList, PROPERTY_Status);
        if (relatedState == NULL)
        {
            LOG_D(TAG, "UpnpStateList_GetState failed: <%s>", PROPERTY_Status);
        }

        PropertyList_InitProperty(resultList, _GetTarget_ARG_RetTargetValue, &relatedState->definition.type);

        ret = UpnpActionList_AddAction(actionList, GetStatus);
        if (RET_FAILED(ret))
        {
            LOG_D(TAG, "UpnpActionList_AddAction failed: <%s>", ACTION_GetStatus);
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