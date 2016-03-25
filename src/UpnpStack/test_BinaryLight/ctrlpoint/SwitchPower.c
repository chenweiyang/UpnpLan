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
#include "UpnpCode.h"
#include "UpnpEvent.h"
#include "UpnpServiceDefinition.h"

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

static void event_listener(UpnpEvent *event, void *ctx);

struct _SwitchPower
{
    UpnpService *service;
    UpnpRuntime *runtime;
    char subscriptionId[UPNP_UUID_LEN];
    SwitchPower_SubscriptionExpired onSubscriptionExpired;
    SwitchPower_StatusChanged onStatusChanged;
    void *ctx;
};

SwitchPower * SwitchPower_Create(UpnpService *service, UpnpRuntime *runtime)
{
    SwitchPower * thiz = NULL;

    RETURN_VAL_IF_FAIL(service, NULL);
    RETURN_VAL_IF_FAIL(runtime, NULL);

    do
    {
        thiz = (SwitchPower *)tiny_malloc(sizeof(SwitchPower));
        if (thiz == NULL)
        {
            LOG_E(TAG, "OUT OF MEMORY!");
            break;
        }

        memset(thiz, 0, sizeof(SwitchPower));
        thiz->service = service;
        thiz->runtime = runtime;

        if (!STR_EQUAL(_SERVICE_TYPE, UpnpService_GetPropertyValue(service, UPNP_SERVICE_ServiceType)))
        {
            LOG_E(TAG, "SERVICE_TYPE invalid: %s", UpnpService_GetPropertyValue(service, UPNP_SERVICE_ServiceType));
            SwitchPower_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

void SwitchPower_Delete(SwitchPower *thiz)
{
    RETURN_IF_FAIL(thiz);

    tiny_free(thiz);
}

TinyRet SwitchPower_GetTarget(SwitchPower *thiz, SwitchPower_GetTargetResult *result, UpnpError *error)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(result, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(error, TINY_RET_E_ARG_NULL);

    do
    {
        UpnpActionList *list = UpnpService_GetActionList(thiz->service);
        UpnpAction *action = UpnpActionList_GetAction(list, ACTION_GetTarget);
        if (action == NULL)
        {
            ret = TINY_RET_E_UPNP_ACTION_NOT_FOUND;
            break;
        }

        /**
         * Argument IN (0)
         */

        ret = UpnpRuntime_Invoke(thiz->runtime, action, error);
        if (ret != TINY_RET_OK)
        {
            break;
        }

        if (error->code != UPNP_SUCCESS)
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

        result->theTargetValue = _RetTargetValue->value.object.value.boolValue;
    } while (0);

    return ret;
}

TinyRet SwitchPower_SetTarget(SwitchPower *thiz, bool newTargetValue, UpnpError *error)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(error, TINY_RET_E_ARG_NULL);

    do
    {
        UpnpActionList *list = UpnpService_GetActionList(thiz->service);
        UpnpAction *action = UpnpActionList_GetAction(list, ACTION_SetTarget);
        if (action == NULL)
        {
            ret = TINY_RET_E_ARG_INVALID;
            break;
        }

        /**
        * Argument IN (1)
        */

        PropertyList *_in = UpnpAction_GetArgumentList(action);
        Property *_newTargetValue = PropertyList_GetProperty(_in, _SetTarget_ARG_newTargetValue);
        if (_newTargetValue == NULL)
        {
            LOG_E(TAG, "Result invalid: %s NOT FOUND!", _SetTarget_ARG_newTargetValue);
            break;
        }

        _newTargetValue->value.object.value.boolValue = newTargetValue;

        ret = UpnpRuntime_Invoke(thiz->runtime, action, error);
        if (ret != TINY_RET_OK)
        {
            break;
        }

        if (error->code != UPNP_SUCCESS)
        {
            break;
        }

        /**
        * Argument OUT (0)
        */
    } while (0);

    return ret;
}

TinyRet SwitchPower_GetStatus(SwitchPower *thiz, SwitchPower_GetStatusResult *result, UpnpError *error)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(result, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(error, TINY_RET_E_ARG_NULL);

    do
    {
        UpnpActionList *list = UpnpService_GetActionList(thiz->service);
        UpnpAction *action = UpnpActionList_GetAction(list, ACTION_GetStatus);
        if (action == NULL)
        {
            ret = TINY_RET_E_ARG_INVALID;
            break;
        }

        /**
        * Argument IN (0)
        */

        ret = UpnpRuntime_Invoke(thiz->runtime, action, error);
        if (ret != TINY_RET_OK)
        {
            break;
        }

        if (error->code != UPNP_SUCCESS)
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

        result->theResultStatus = _ResultStatus->value.object.value.boolValue;
    } while (0);

    return ret;
}

TinyRet SwitchPower_Subscribe(SwitchPower *thiz, 
    SwitchPower_SubscriptionExpired onSubscriptionExpired,
    SwitchPower_StatusChanged onStatusChanged,
    UpnpError *error,
    void *ctx)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(error, TINY_RET_E_ARG_NULL);

    do
    {
        thiz->ctx = ctx;
        thiz->onSubscriptionExpired = onSubscriptionExpired;
        thiz->onStatusChanged = onStatusChanged;
        thiz->ctx = ctx;

        ret = UpnpRuntime_Subscribe(thiz->runtime, thiz->service, 0, event_listener, thiz, error);
        if (RET_FAILED(ret))
        {
            break;
        }
    } while (0);

    return ret;
}

TinyRet SwitchPower_Unsubscribe(SwitchPower *thiz, UpnpError *error)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(error, TINY_RET_E_ARG_NULL);

    ret = UpnpRuntime_Unsubscribe(thiz->runtime, thiz->service, error);
    if (RET_SUCCEEDED(ret))
    {
        thiz->onSubscriptionExpired = NULL;
        thiz->onStatusChanged = NULL;
        thiz->ctx = NULL;
    }

    return ret;
}

static void event_listener(UpnpEvent *event, void *ctx)
{
    SwitchPower *thiz = (SwitchPower *)ctx;

    LOG_D(TAG, "event_listener");

    do
    {
        const char * _status = UpnpEvent_GetArgumentValue(event, PROPERTY_Status);
        if (_status != NULL)
        {
            if (thiz->onStatusChanged != NULL)
            {
                thiz->onStatusChanged(ObjectType_StringToBoolean(_status), thiz->ctx);
            }
        }
    } while (0);
}
