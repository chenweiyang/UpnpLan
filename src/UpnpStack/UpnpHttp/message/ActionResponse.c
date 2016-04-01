/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   ActionResponse.c
*
* @remark
*
*/

#include "ActionResponse.h"
#include "UpnpDevice.h"
#include "UpnpService.h"
#include "soap/SoapMessage.h"
#include "tiny_log.h"
#include "UpnpCode.h"

#define TAG             "ActionResponse"

static TinyRet SoapResponseToActionResult(SoapMessage *soap, UpnpAction *action, UpnpError *error);

TinyRet ActionFromResponse(UpnpAction *action, UpnpError *error, HttpMessage *response)
{
    TinyRet ret = TINY_RET_OK;

    do
    {
        error->code = HttpMessage_GetStatusCode(response);
        strncpy(error->description, HttpMessage_GetStatus(response), UPNP_ERR_DESCRIPTION_LEN);

        if (error->code != HTTP_STATUS_OK)
        {
            LOG_D(TAG, "Action Execute failed: %d %s", error->code, error->description);
            ret = TINY_RET_E_UPNP_INVOKE_FAILED;
            break;
        }

        SoapMessage *soap = SoapMessage_New();
        if (soap == NULL)
        {
            LOG_E(TAG, "SoapMessage_New failed");
            ret = TINY_RET_E_NEW;
            break;
        }

        do
        {
            ret = SoapMessage_Parse(soap, HttpMessage_GetContentObject(response), HttpMessage_GetContentSize(response));
            if (RET_FAILED(ret))
            {
                LOG_D(TAG, "SoapMessage_Parse failed: %s", tiny_ret_to_str(ret));
                break;
            }

            ret = SoapResponseToActionResult(soap, action, error);
        } while (0);

        SoapMessage_Delete(soap);
    } while (0);

    return ret;
}

TinyRet SoapResponseToActionResult(SoapMessage *soap, UpnpAction *action, UpnpError *error)
{
    TinyRet ret = TINY_RET_OK;

    do
    {
        uint32_t i = 0;
        uint32_t count = 0;
        UpnpService *service = NULL;
        PropertyList *soapResults = NULL;

        if (SoapMessage_IsFault(soap))
        {
            error->code = SoapMessage_GetErrorCode(soap);
            strncpy(error->description, SoapMessage_GetErrorDescription(soap), UPNP_ERR_DESCRIPTION_LEN);
            ret = TINY_RET_E_UPNP_INVOKE_FAILED;
            break;
        }

        service = (UpnpService *)UpnpAction_GetParentService(action);
        if (service == NULL)
        {
            ret = TINY_RET_E_UPNP_SERVICE_NOT_FOUND;
        }

        soapResults = SoapMessage_GetArgumentList(soap);

        count = UpnpAction_GetArgumentCount(action);
        for (i = 0; i < count; ++i)
        {
            const char *value = NULL;
            UpnpArgument * argument = NULL;
            UpnpStateVariable * state = NULL;

            argument = UpnpAction_GetArgumentAt(action, i);
            if (UpnpArgument_GetDirection(argument) != ARG_OUT)
            {
                continue;
            }

            state = UpnpService_GetStateVariable(service, UpnpArgument_GetRelatedStateVariable(argument));
            if (state == NULL)
            {
                LOG_E(TAG, "RelatedStateVariable NOT FOUND: %s", UpnpArgument_GetRelatedStateVariable(argument));
                ret = TINY_RET_E_UPNP_ARGUMENT_NOT_FOUND;
                break;
            }

            value = PropertyList_GetPropertyValue(soapResults, UpnpArgument_GetName(argument));
            if (value == NULL)
            {
                LOG_D(TAG, "result NOT FOUND: %s", UpnpArgument_GetName(argument));
                ret = TINY_RET_E_UPNP_ARGUMENT_NOT_FOUND;
                break;
            }

            ret = DataValue_SetValue(&state->value, value);
            if (RET_FAILED(ret))
            {
                break;
            }
        }

        if (RET_SUCCEEDED(ret))
        {
            error->code = UPNP_SUCCESS;
            strncpy(error->description, "OK", UPNP_ERR_DESCRIPTION_LEN);
        }
    } while (0);

    return ret;
}