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
#include "tiny_memory.h"

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
            ret = SoapMessage_ParseResponse(soap, HttpMessage_GetContentObject(response), HttpMessage_GetContentSize(response));
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

static TinyRet ActionToSoapResponse(UpnpAction *action, SoapMessage *soap);
static TinyRet SoapResponseToHttpResponse(SoapMessage *soap, HttpMessage *response);

TinyRet ActionToResponse(UpnpAction *action, HttpMessage *response)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(action, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(response, TINY_RET_E_ARG_NULL);

    do
    {
        SoapMessage * soap = SoapMessage_New();
        if (soap == NULL)
        {
            ret = TINY_RET_E_NEW;
            break;
        }

        ret = ActionToSoapResponse(action, soap);
        if (RET_SUCCEEDED(ret))
        {
            ret = SoapResponseToHttpResponse(soap, response);
        }

        SoapMessage_Delete(soap);
    } while (0);

    return ret;
}

static TinyRet ActionToSoapResponse(UpnpAction *action, SoapMessage *soap)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(action, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(soap, TINY_RET_E_ARG_NULL);

    do
    {
        uint32_t i = 0;
        uint32_t count = 0;
        PropertyList *soapArguments = SoapMessage_GetArgumentList(soap);
        UpnpDevice *device = NULL;
        UpnpService *service = NULL;
        const char *ctrlUrl = NULL;
        const char *urlBase = NULL;
        char url[TINY_URL_LEN];

        memset(url, 0, TINY_URL_LEN);

        service = (UpnpService *)UpnpAction_GetParentService(action);
        if (service == NULL)
        {
            ret = TINY_RET_E_UPNP_SERVICE_NOT_FOUND;
            break;
        }

        device = (UpnpDevice *)UpnpService_GetParentDevice(service);
        if (service == NULL)
        {
            ret = TINY_RET_E_UPNP_DEVICE_NOT_FOUND;
            break;
        }

        ctrlUrl = UpnpService_GetControlURL(service);
        urlBase = UpnpDevice_GetURLBase(device);

        tiny_snprintf(url, TINY_URL_LEN, "%s%s", urlBase, ctrlUrl);

        ret = SoapMessage_SetServerURL(soap, url);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = SoapMessage_SetResponseActionName(soap, UpnpAction_GetName(action));
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = SoapMessage_SetActionXmlns(soap, UpnpService_GetServiceType(service));
        if (RET_FAILED(ret))
        {
            break;
        }

        count = UpnpAction_GetArgumentCount(action);
        for (i = 0; i < count; ++i)
        {
            UpnpArgument * argument = NULL;
            UpnpStateVariable * state = NULL;
            const char *name = NULL;
            const char *value = NULL;
            char buffer[128];

            argument = UpnpAction_GetArgumentAt(action, i);
            if (UpnpArgument_GetDirection(argument) != ARG_OUT)
            {
                continue;
            }

            state = UpnpService_GetStateVariable(service, UpnpArgument_GetRelatedStateVariable(argument));
            if (state == NULL)
            {
                LOG_E(TAG, "RelatedStateVariable NOT FOUND: %s", UpnpArgument_GetRelatedStateVariable(argument));
                break;
            }

            memset(buffer, 0, 128);

            name = UpnpArgument_GetName(argument);
            value = buffer;

            if (state->value.internalType == INTERNAL_STRING)
            {
                value = state->value.internalValue.stringValue;
            }
            else
            {
                ret = DataValue_GetValue(&state->value, buffer, 128);
                if (RET_FAILED(ret))
                {
                    LOG_E(TAG, "value invalid: %s", name);
                    break;
                }
            }

            PropertyList_Add(soapArguments, name, value);
        }
    } while (0);

    return ret;
}

static TinyRet SoapResponseToHttpResponse(SoapMessage *soap, HttpMessage *response)
{
    TinyRet ret = TINY_RET_OK;

    do
    {
        char soap_action[256];
        char *data = NULL;
        uint32_t size = UPNP_SOAP_LEN;
        const char * serverUrl = SoapMessage_GetServerURL(soap);
        const char * actionName = SoapMessage_GetActionName(soap);
        const char * actionXmlns = SoapMessage_GetActionXmlns(soap);

        tiny_snprintf(soap_action, 256, "\"%s#%s\"", actionXmlns, actionName);

        data = (char *)tiny_malloc(size);
        if (data == NULL)
        {
            ret = TINY_RET_E_OUT_OF_MEMORY;
            break;
        }

        memset(data, 0, size);
        ret = SoapMessage_ToString(soap, data, size);
        if (RET_FAILED(ret))
        {
            tiny_free(data);
            break;
        }

        size = strlen(data);

        HttpMessage_SetType(response, HTTP_RESPONSE);
        HttpMessage_SetVersion(response, 1, 1);
        HttpMessage_SetResponse(response, 200, "OK");
        HttpMessage_SetHeader(response, "Content-Type", "text/xml;charset=\"utf-8\"");
        HttpMessage_SetHeader(response, "User-Agent", UPNP_STACK_INFO);
        HttpMessage_SetHeaderInteger(response, "Content-Length", size);
        HttpMessage_SetContentSize(response, size);
        HttpMessage_AddContentObject(response, data, size);

        tiny_free(data);
    } while (0);

    return ret;
}