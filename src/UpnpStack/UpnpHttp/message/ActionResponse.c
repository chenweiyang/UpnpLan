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
#include "UpnpDeviceDefinition.h"
#include "UpnpServiceDefinition.h"
#include "UpnpActionDefinition.h"
#include "soap/SoapDefinition.h"
#include "soap/SoapMessage.h"
#include "tiny_log.h"
#include "UpnpCode.h"
#include "UpnpError.h"

#define TAG             "ActionResponse"

static TinyRet SoapResponseToActionResult(SoapMessage *soap, UpnpAction *action, UpnpError *error);

TinyRet ActionFromResponse(UpnpAction *action, UpnpError *error, HttpMessage *response)
{
    TinyRet ret = TINY_RET_OK;

    do
    {
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

            Property *result = PropertyList_GetProperty(soapResults, name);
            if (result->definition.type.clazzType == CLAZZ_STRING)
            {
                p->value.object.type.clazzType = p->definition.type.clazzType;

                ret = Object_setValue(&p->value.object, result->value.object.value.stringValue);
                if (RET_FAILED(ret))
                {
                    LOG_D(TAG, "Object_setValue failed: %s, Property Value Type is: %d, Property definition type is: %d",
                        result->value.object.value.stringValue,
                        p->value.object.type.clazzType,
                        p->definition.type.clazzType);
                    break;
                }
            }
        }

        error->code = UPNP_SUCCESS;
        strncpy(error->description, "OK", UPNP_ERR_DESCRIPTION_LEN);
    }

    return ret;
}
