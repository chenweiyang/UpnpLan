/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   UpnpSoapHelper.c
*
* @remark
*
*/

#include "UpnpSoapHelper.h"
#include "UpnpDevice.h"
#include "UpnpService.h"
#include "UpnpDeviceDefinition.h"
#include "UpnpServiceDefinition.h"
#include "SoapDefinition.h"

TinyRet ActionToSoapMessage(UpnpAction *action, SoapMessage *soap)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(action, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(soap, TINY_RET_E_ARG_NULL);

    do
    {
        PropertyList *actionArguments = NULL;
        PropertyList *soapArguments = NULL;
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

        ctrlUrl = UpnpService_GetPropertyValue(service, UPNP_SERVICE_ControlURL);
        urlBase = UpnpDevice_GetPropertyValue(device, UPNP_DEVICE_URLBase);

        tiny_snprintf(url, TINY_URL_LEN, "%s%s", urlBase, ctrlUrl);

        ret = SoapMessage_SetPropertyValue(soap, SOAP_ServerURL, url);
        if (RET_FAILED(ret))
        {
            break;
        }

        soapArguments = SoapMessage_GetArgumentList(soap);
        actionArguments = UpnpAction_GetArgumentList(action);
        PropertyList_Copy(soapArguments, actionArguments);
    } while (0);

    return ret;
}

TinyRet SoapMessageToActionResult(SoapMessage *soap, UpnpAction *action, UpnpError *error)
{
    TinyRet ret = TINY_RET_OK;

    PropertyList *fault = SoapMessage_GetFault(soap);
    if (fault != NULL)
    {
        const char * errorCode = SoapMessage_GetFaultValue(soap, SOAP_FAULT_ErrorCode);
        const char * errorDesc = SoapMessage_GetFaultValue(soap, SOAP_FAULT_ErrorDescription);
        error->code = atoi(errorCode);
        strncpy(error->description, errorDesc, UPNP_ERR_DESCRIPTION_LEN);
        ret = TINY_RET_E_UPNP_INVOKE_FAILED;
    }
    else
    {
        uint32_t i = 0;
        uint32_t count = 0;

        PropertyList *actionResults = UpnpAction_GetResultList(action);
        PropertyList *soapResults = SoapMessage_GetArgumentList(soap);

        count = PropertyList_GetSize(actionResults);
        for (i = 0; i < count; ++i)
        {
            Property *p = PropertyList_GetPropertyAt(actionResults, i);
            const char *name = p->definition.name;
            ObjectType *type = &p->definition.type;

            Property *result = PropertyList_GetProperty(soapResults, name);
            if (result->definition.type.clazzType != type->clazzType)
            {
                ret = TINY_RET_E_UPNP_INVOKE_FAILED;
                break;
            }
        }
    }

    return ret;
}
