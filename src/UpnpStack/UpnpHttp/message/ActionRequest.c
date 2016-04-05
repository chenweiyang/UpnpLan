/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   ActionRequest.c
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#include "ActionRequest.h"
#include "soap/SoapMessage.h"
#include "UpnpDevice.h"
#include "UpnpService.h"
#include "tiny_memory.h"
#include "tiny_log.h"

#define TAG     "ActionRequest"

static TinyRet ActionToSoapRequest(UpnpAction *action, SoapMessage *soap);
static TinyRet SoapRequestToHttpRequest(SoapMessage *soap, HttpMessage *request);

TinyRet ActionToRequest(UpnpAction *action, HttpMessage *request)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(action, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(request, TINY_RET_E_ARG_NULL);

    do
    {
        SoapMessage * soap = SoapMessage_New();
        if (soap == NULL)
        {
            ret = TINY_RET_E_NEW;
            break;
        }

        ret = ActionToSoapRequest(action, soap);
        if (RET_SUCCEEDED(ret))
        {
            ret = SoapRequestToHttpRequest(soap, request);
        }

        SoapMessage_Delete(soap);
    } while (0);

    return ret;
}

static TinyRet ActionToSoapRequest(UpnpAction *action, SoapMessage *soap)
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

        ret = SoapMessage_SetActionName(soap, UpnpAction_GetName(action));
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
            if (UpnpArgument_GetDirection(argument) != ARG_IN)
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

static TinyRet SoapRequestToHttpRequest(SoapMessage *soap, HttpMessage *request)
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

        HttpMessage_SetRequest(request, "POST", serverUrl);
        HttpMessage_SetHeader(request, "Content-Type", "text/xml;charset=\"utf-8\"");
        HttpMessage_SetHeader(request, "Soapaction", soap_action);
        HttpMessage_SetHeader(request, "User-Agent", UPNP_STACK_INFO);
        HttpMessage_SetHeaderInteger(request, "Content-Length", size);
        HttpMessage_SetContentSize(request, size);
        HttpMessage_AddContentObject(request, data, size);

        tiny_free(data);
    } while (0);

    return ret;
}

TinyRet ActionFromRequest(UpnpAction *action, const char *content, uint32_t contentLength)
{
    TinyRet ret = TINY_RET_OK;

    do
    {
        SoapMessage * soap = SoapMessage_New();
        if (soap == NULL)
        {
            ret = TINY_RET_E_NEW;
            break;
        }

        do
        {
            uint32_t i = 0;
            uint32_t count = 0;
            UpnpService * service = UpnpAction_GetParentService(action);

            printf("content: %s\n", content);

            ret = SoapMessage_ParseRequest(soap, content, contentLength);
            if (RET_FAILED(ret))
            {
                break;
            }

            count = UpnpAction_GetArgumentCount(action);
            for (i = 0; i < count; i++)
            {
                UpnpArgument *argument = UpnpAction_GetArgumentAt(action, i);
                if (UpnpArgument_GetDirection(argument) == ARG_IN)
                {
                    const char *name = UpnpArgument_GetName(argument);
                    const char *relatedStateVariable = NULL;
                    UpnpStateVariable *state = NULL;
                    PropertyList *list = NULL;
                    const char *value = NULL;

                    list = SoapMessage_GetArgumentList(soap);
                    value = PropertyList_GetPropertyValue(list, name);
                    if (value == NULL)
                    {
                        ret = TINY_RET_E_UPNP_ARGUMENT_NOT_FOUND;
                        break;
                    }

                    relatedStateVariable = UpnpArgument_GetRelatedStateVariable(argument);
                    state = UpnpService_GetStateVariable(service, relatedStateVariable);
                    if (state == NULL)
                    {
                        ret = TINY_RET_E_UPNP_ARGUMENT_NOT_FOUND;
                        break;
                    }

                    ret = DataValue_SetValue(&state->value, value);
                    if (RET_FAILED(ret))
                    {
                        break;
                    }
                }
            }
        } while (0);

        SoapMessage_Delete(soap);
    } while (0);

    return ret;
}