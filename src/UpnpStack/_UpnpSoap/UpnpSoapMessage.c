/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   UpnpSoapMessage.c
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#include "UpnpSoapMessage.h"
#include "ct_memory.h"
#include "str_split.h"
#include "str_equal.h"
#include "ct_log.h"
#include "CtList.h"
#include "TinyXml.h"
#include "PropertyList.h"
#include "UpnpSoapMessageDefinition.h"

#define TAG                             "UpnpSoapMessage"

static CtRet UpnpSoapMessage_Construct(UpnpSoapMessage *thiz);
static CtRet UpnpSoapMessage_Dispose(UpnpSoapMessage *thiz);
static CtRet UpnpSoapMessage_InitializeProperty(UpnpSoapMessage *thiz);
static CtRet UpnpSoapMessage_InitializeFault(UpnpSoapMessage *thiz);
static CtRet UpnpSoapMessage_ParseXml(UpnpSoapMessage *thiz, TinyXml *xml);

static bool is_envelope(UpnpSoapMessage *thiz, TinyXmlNode *root);
static CtRet load_body(UpnpSoapMessage *thiz, TinyXmlNode *root);
static CtRet load_soap_fault(UpnpSoapMessage *thiz, TinyXmlNode *fault);
static bool is_soap_fault(UpnpSoapMessage *thiz, const char *name);
//static CtRet get_action_name(UpnpSoapMessage *thiz, const char *name);
//static CtRet get_action_xmlns(UpnpSoapMessage *thiz, CtList *atts);

struct _UpnpSoapMessage
{
    PropertyList *propertyList;
    PropertyList *argumentList;
    PropertyList *fault;
};

UpnpSoapMessage * UpnpSoapMessage_New(void)
{
    UpnpSoapMessage *thiz = NULL;

    do
    {
        CtRet ret = CT_RET_OK;

        thiz = (UpnpSoapMessage *)ct_malloc(sizeof(UpnpSoapMessage));
        if (thiz == NULL)
        {
            break;
        }

        ret = UpnpSoapMessage_Construct(thiz);
        if (RET_FAILED(ret))
        {
            UpnpSoapMessage_Delete(thiz);
            thiz = NULL;
            break;
        }

        ret = UpnpSoapMessage_InitializeProperty(thiz);
        if (RET_FAILED(ret))
        {
            UpnpSoapMessage_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

static CtRet UpnpSoapMessage_Construct(UpnpSoapMessage *thiz)
{
    CtRet ret = CT_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, CT_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(UpnpSoapMessage));

        thiz->propertyList = PropertyList_New();
        if (thiz->propertyList == NULL)
        {
            ret = CT_RET_E_NEW;
            break;
        }

        thiz->argumentList = PropertyList_New();
        if (thiz->argumentList == NULL)
        {
            ret = CT_RET_E_NEW;
            break;
        }
    } while (0);

    return ret;
}

static CtRet UpnpSoapMessage_Dispose(UpnpSoapMessage *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, CT_RET_E_ARG_NULL);

    if (thiz->propertyList != NULL)
    {
        PropertyList_Delete(thiz->propertyList);
    }

    if (thiz->argumentList != NULL)
    {
        PropertyList_Delete(thiz->argumentList);
    }

    if (thiz->fault != NULL)
    {
        PropertyList_Delete(thiz->fault);
    }

    return CT_RET_OK;
}

void UpnpSoapMessage_Delete(UpnpSoapMessage *thiz)
{
    RETURN_IF_FAIL(thiz);
    UpnpSoapMessage_Dispose(thiz);
    ct_free(thiz);
}

static CtRet UpnpSoapMessage_InitializeProperty(UpnpSoapMessage *thiz)
{
    CtRet ret = CT_RET_OK;
    ObjectType type;

    RETURN_VAL_IF_FAIL(thiz, CT_RET_E_ARG_NULL);

    ObjectType_Construct(&type);

    do
    {
        ObjectType_SetType(&type, CLAZZ_STRING);

        ret = PropertyList_InitProperty(thiz->propertyList, UPNP_SOAP_ServerURL, &type);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = PropertyList_InitProperty(thiz->propertyList, UPNP_SOAP_ActionName, &type);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = PropertyList_InitProperty(thiz->propertyList, UPNP_SOAP_ActionXmlns, &type);
        if (RET_FAILED(ret))
        {
            break;
        }
    } while (0);

    ObjectType_Dispose(&type);

    return ret;
}

static CtRet UpnpSoapMessage_InitializeFault(UpnpSoapMessage *thiz)
{
    CtRet ret = CT_RET_OK;
    ObjectType type;

    RETURN_VAL_IF_FAIL(thiz, CT_RET_E_ARG_NULL);

    ObjectType_Construct(&type);

    do
    {
        ObjectType_SetType(&type, CLAZZ_STRING);

        ret = PropertyList_InitProperty(thiz->fault, UPNP_SOAP_FAULT_Code, &type);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = PropertyList_InitProperty(thiz->fault, UPNP_SOAP_FAULT_String, &type);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = PropertyList_InitProperty(thiz->fault, UPNP_SOAP_FAULT_ErrorCode, &type);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = PropertyList_InitProperty(thiz->fault, UPNP_SOAP_FAULT_ErrorDescription, &type);
        if (RET_FAILED(ret))
        {
            break;
        }

    } while (0);

    ObjectType_Dispose(&type);

    return ret;
}

PropertyList *UpnpSoapMessage_GetArgumentList(UpnpSoapMessage *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->argumentList;
}

PropertyList *UpnpSoapMessage_GetFault(UpnpSoapMessage *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->fault;
}

CtRet UpnpSoapMessage_Parse(UpnpSoapMessage *thiz, const char *bytes, uint32_t len)
{
    CtRet ret = CT_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, CT_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(bytes, CT_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(len, CT_RET_E_ARG_NULL);

    do
    {
        TinyXml *xml;

        UpnpSoapMessage_Dispose(thiz);
        UpnpSoapMessage_Construct(thiz);

        xml = TinyXml_New();
        if (xml == NULL)
        {
            LOG_D(TAG, "Out of memory");
            ret = CT_RET_E_NEW;
            break;
        }

        do
        {
            ret = TinyXml_Parse(xml, bytes, len);
            if (RET_FAILED(ret))
            {
                LOG_D(TAG, "TinyXml_Parse failed: %s", ct_ret_to_str(ret));
                break;
            }

            ret = UpnpSoapMessage_ParseXml(thiz, xml);
            if (RET_FAILED(ret))
            {
                LOG_D(TAG, "UpnpSoapMessage_ParseXml failed: %s", ct_ret_to_str(ret));
                break;
            }
        } while (0);

        TinyXml_Delete(xml);

    } while (0);

    return ret;
}

static CtRet UpnpSoapMessage_ParseXml(UpnpSoapMessage *thiz, TinyXml *xml)
{
    LOG_TIME_BEGIN(TAG, UpnpSoapMessage_ParseXml);
    CtRet ret = CT_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, CT_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(xml, CT_RET_E_ARG_NULL);

    LOG_D(TAG, "UpnpSoapMessage_ParseXml ...");

    do
    {
        TinyXmlNode *root = TinyXml_GetRoot(xml);
        if (root == NULL)
        {
            ret = CT_RET_E_XML_INVALID;
            break;
        }

        if (!is_envelope(thiz, root))
        {
            ret = CT_RET_E_XML_INVALID;
            break;
        }

        ret = load_body(thiz, root);
        if (RET_FAILED(ret))
        {
            break;
        }

    } while (0);

    LOG_TIME_END(TAG, UpnpSoapMessage_ParseXml);

    return ret;
}

/*
<?xml version="1.0"?>
<s:Envelope xmlns:s="http://schemas.xmlsoap.org/soap/envelope/"
s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/">
<s:Body>
<u:actionName xmlns:u="urn:schemas-upnp-org:service:serviceType:v">
<argumentName>in arg value</argumentName>
<!-- other in args and their values go here, if any -->
</u:actionName>
</s:Body>
</s:Envelope>
*/

#define XML_VERSION                     "<?xml version = \"1.0\"?>\n"
#define SOAP_ENVELOPE_BEGIN             "<s:Envelope"
#define SOAP_ENVELOPE_BEGIN_XMLNS       " xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\""
#define SOAP_ENVELOPE_BEGIN_ENCODING    " s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">\n"
#define SOAP_ENVELOPE_END               "</s:Envelope>\n"
#define SOAP_BODY_BEGIN                 "<s:Body>\n"
#define SOAP_BODY_END                   "</s:Body>\n"
#define SOAP_ACTION_BEGIN               "<u:%s xmlns:u=\"%s\">\n"
#define SOAP_ACTION_END                 "</u:%s>\n"

CtRet UpnpSoapMessage_ToString(UpnpSoapMessage *thiz, char *bytes, uint32_t len)
{
    CtRet ret = CT_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, CT_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(bytes, CT_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(len, CT_RET_E_ARG_NULL);

    do
    {
        Object *actionName = NULL;
        Object *actionXmlns = NULL;
        uint32_t i = 0;
        uint32_t count = 0;
        int32_t unused = 0;
        char *p = NULL;

        ct_snprintf(bytes, len, "%s%s%s%s%s",
            XML_VERSION,
            SOAP_ENVELOPE_BEGIN,
            SOAP_ENVELOPE_BEGIN_XMLNS,
            SOAP_ENVELOPE_BEGIN_ENCODING,
            SOAP_BODY_BEGIN);
        bytes[len - 1] = 0;

        p = bytes + strlen(bytes);
        unused = len - (p - bytes);
        if (unused < 0)
        {
            ret = CT_RET_E_ARG_INVALID;
            break;
        }

        actionName = PropertyList_GetPropertyValue(thiz->propertyList, UPNP_SOAP_ActionName);
        if (actionName == NULL)
        {
            ret = CT_RET_E_ARG_INVALID;
            break;
        }

        actionXmlns = PropertyList_GetPropertyValue(thiz->propertyList, UPNP_SOAP_ActionXmlns);
        if (actionXmlns == NULL)
        {
            ret = CT_RET_E_ARG_INVALID;
            break;
        }

        ct_snprintf(p, unused, SOAP_ACTION_BEGIN, actionName->value.stringValue, actionXmlns->value.stringValue);
        p[unused - 1] = 0;

        p += strlen(p);
        unused = len - (p - bytes);
        if (unused < 0)
        {
            ret = CT_RET_E_ARG_INVALID;
            break;
        }

        count = PropertyList_GetSize(thiz->argumentList);
        for (i = 0; i < count; i++)
        {
            Property *property = PropertyList_GetPropertyAt(thiz->propertyList, i);
            const char *name = property->definition.name;
            ClazzType clazzType = property->definition.type.clazzType;
            ObjectValue *v = &(property->value.data.value);

            if (clazzType != CLAZZ_STRING)
            {
                LOG_E(TAG, "%s value must be string", name);
                continue;
            }

            ct_snprintf(p, unused, "<%s>%s</%s>\n", name, v->stringValue, name);

            p[unused - 1] = 0;

            p += strlen(p);
            unused = len - (p - bytes);

            if (unused < 0)
            {
                ret = CT_RET_E_ARG_INVALID;
                break;
            }
        }

        ct_snprintf(p, unused, SOAP_ACTION_END, actionName->value.stringValue);
        p[unused - 1] = 0;

        p += strlen(p);
        unused = len - (p - bytes);
        if (unused < 0)
        {
            ret = CT_RET_E_ARG_INVALID;
            break;
        }

        ct_snprintf(p, unused, "%s%s",
            SOAP_BODY_END,
            SOAP_ENVELOPE_END);
        p[unused - 1] = 0;

    } while (0);

    return ret;
}

CtRet UpnpSoapMessage_SetPropertyValue(UpnpSoapMessage *thiz, const char *propertyName, const char *value)
{
    CtRet ret = CT_RET_OK;
    Object data;

    RETURN_VAL_IF_FAIL(thiz, NULL);
    RETURN_VAL_IF_FAIL(propertyName, NULL);
    RETURN_VAL_IF_FAIL(value, NULL);

    Object_Construct(&data);
    {
        Object_setString(&data, value);
        ret = PropertyList_SetPropertyValue(thiz->propertyList, propertyName, &data);
    }
    Object_Dispose(&data);

    return ret;
}

const char * UpnpSoapMessage_GetPropertyValue(UpnpSoapMessage *thiz, const char *propertyName)
{
    const char *value = NULL;
    Object *data = NULL;

    RETURN_VAL_IF_FAIL(thiz, NULL);
    RETURN_VAL_IF_FAIL(propertyName, NULL);

    data = PropertyList_GetPropertyValue(thiz->propertyList, propertyName);
    if (data != NULL)
    {
        value = data->value.stringValue;
    }

    return value;
}

CtRet UpnpSoapMessage_SetFaultValue(UpnpSoapMessage *thiz, const char *faultName, const char *value)
{
    CtRet ret = CT_RET_OK;
    Object data;

    RETURN_VAL_IF_FAIL(thiz, NULL);
    RETURN_VAL_IF_FAIL(propertyName, NULL);
    RETURN_VAL_IF_FAIL(value, NULL);

    Object_Construct(&data);
    {
        Object_setString(&data, value);
        ret = PropertyList_SetPropertyValue(thiz->propertyList, faultName, &data);
    }
    Object_Dispose(&data);

    return ret;
}

const char * UpnpSoapMessage_GetFaultValue(UpnpSoapMessage *thiz, const char *faultName)
{
    const char *value = NULL;
    Object *data = NULL;

    RETURN_VAL_IF_FAIL(thiz, NULL);
    RETURN_VAL_IF_FAIL(faultName, NULL);

    data = PropertyList_GetPropertyValue(thiz->fault, faultName);
    if (data != NULL)
    {
        value = data->value.stringValue;
    }

    return value;
}

static bool is_envelope(UpnpSoapMessage *thiz, TinyXmlNode *root)
{
    bool result = false;

    if (strstr(TinyXmlNode_GetName(root), "Envelope") != NULL)
    {
        result = true;
    }

    return result;
}

static CtRet load_body(UpnpSoapMessage *thiz, TinyXmlNode *root)
{
    CtRet ret = CT_RET_OK;

    do
    {
        TinyXmlNode *body = NULL;
        TinyXmlNode *response = NULL;
        uint32_t count = 0;
        uint32_t i = 0;

        /* <s:Body> or
        * <SOAP-ENV:Body>
        */
        body = TinyXmlNode_GetChildByName(root, "s:Body");
        if (body == NULL)
        {
            body = TinyXmlNode_GetChildByName(root, "SOAP-ENV:Body");
        }

        if (body == NULL)
        {
            ret = CT_RET_E_NOT_FOUND;
            break;
        }

        response = TinyXmlNode_GetChildAt(body, 0);
        if (response == NULL)
        {
            ret = CT_RET_E_NOT_FOUND;
            break;
        }

        /*
        * SOAP-ENV:Fault or
        * s:Fault
        */
        if (is_soap_fault(thiz, TinyXmlNode_GetName(response)))
        {
            ret = UpnpSoapMessage_InitializeFault(thiz);
            if (RET_FAILED(ret))
            {
                break;
            }

            ret = load_soap_fault(thiz, response);
            break;
        }

#if 0
        /*
        * Get the action name
        *
        * <u:[action name][Response]> or
        * <e:[action name][Response]>
        */
        ret = get_action_name(thiz, response->name);
        if (RET_FAILED(ret))
        {
            break;
        }
#endif

#if 0
        /*
        * Get the xmlns
        * xmlns:m="urn:schemas-upnp-org:service:AVTransport:1" or
        * xmlns:u="GetDeviceCapabilities"
        */
        ret = get_action_xmlns(thiz, response->attributes);
        if (RET_FAILED(ret))
        {
            break;
        }
#endif

        count = TinyXmlNode_GetChildren(response);
        for (i = 0; i < count; i++)
        {
            TinyXmlNode *arg = TinyXmlNode_GetChildAt(response, i);
            const char *name = TinyXmlNode_GetName(arg);
            const char *value = TinyXmlNode_GetContent(arg);

            if (name == NULL)
            {
                LOG_D(TAG, "has empty name");
                continue;
            }

            if (value == NULL)
            {
                LOG_D(TAG, "%s has empty value", name);
                continue;
            }

            ObjectType type;
            ObjectType_Construct(&type);
            ObjectType_SetType(&type, CLAZZ_STRING);
            {
                ret = PropertyList_InitProperty(thiz->argumentList, name, &type);
                if (RET_FAILED(ret))
                {
                    break;
                }
            }
            ObjectType_Dispose(&type);

            ret = PropertyList_SetPropertyStringValue(thiz->argumentList, name, value);
            if (RET_FAILED(ret))
            {
                break;
            }
        }
    } while (0);

    return ret;
}

#define SOAP_FAULT_CODE                 "faultcode"
#define SOAP_FAULT_STRING               "faultstring"
#define SOAP_FAULT_DETAIL               "detail"
#define SOAP_FAULT_DETAIL_ERR_CODE      "u:errorCode"
#define SOAP_FAULT_DETAIL_ERR_DESC      "u:errorDescription"

static CtRet load_soap_fault(UpnpSoapMessage *thiz, TinyXmlNode *fault)
{
    CtRet ret = CT_RET_OK;

    do
    {
        const char *faultcode = NULL;
        const char *faultstring = NULL;
        TinyXmlNode *detail = NULL;
        TinyXmlNode *detail_fault = NULL;
        uint32_t count = 0;
        uint32_t i = 0;

        faultcode = TinyXmlNode_GetChildContent(fault, SOAP_FAULT_CODE);
        if (faultcode == NULL)
        {
            ret = CT_RET_E_XML_INVALID;
            break;
        }

        ret = PropertyList_SetPropertyStringValue(thiz->fault, UPNP_SOAP_FAULT_Code, faultcode);
        if (RET_FAILED(ret))
        {
            break;
        }

        faultstring = TinyXmlNode_GetChildContent(fault, SOAP_FAULT_STRING);
        if (faultstring == NULL)
        {
            ret = CT_RET_E_XML_INVALID;
            break;
        }

        ret = PropertyList_SetPropertyStringValue(thiz->fault, UPNP_SOAP_FAULT_String, faultstring);
        if (RET_FAILED(ret))
        {
            break;
        }

        /* <detail> */
        detail = TinyXmlNode_GetChildByName(fault, SOAP_FAULT_DETAIL);
        if (detail == NULL)
        {
            LOG_D(TAG, "<detail> not found");
            ret = CT_RET_E_NOT_FOUND;
            break;
        }

        /* <u:UPnPError> */
        detail_fault = TinyXmlNode_GetChildAt(detail, 0);
        if (fault == NULL)
        {
            LOG_D(TAG, "<detail> has not child");
            ret = CT_RET_E_NOT_FOUND;
            break;
        }

        do
        {
            const char *error_code = NULL;
            const char *error_description = NULL;

            error_code = TinyXmlNode_GetChildContent(detail_fault, SOAP_FAULT_DETAIL_ERR_CODE);
            if (error_code == NULL)
            {
                LOG_D(TAG, "<%s> not found", SOAP_FAULT_DETAIL_ERR_CODE);
                ret = CT_RET_E_NOT_FOUND;
                break;
            }

            error_description = TinyXmlNode_GetChildContent(detail_fault, SOAP_FAULT_DETAIL_ERR_DESC);
            if (error_description == NULL)
            {
                LOG_D(TAG, "<%s> not found", SOAP_FAULT_DETAIL_ERR_DESC);
                ret = CT_RET_E_NOT_FOUND;
                break;
            }

            ret = PropertyList_SetPropertyIntegerValue(thiz->fault, UPNP_SOAP_FAULT_ErrorCode, atoi(error_code));
            if (RET_FAILED(ret))
            {
                break;
            }

            ret = PropertyList_SetPropertyStringValue(thiz->fault, UPNP_SOAP_FAULT_ErrorDescription, error_description);
            if (RET_FAILED(ret))
            {
                break;
            }
        } while (0);
    } while (0);

    return ret;
}

static bool is_soap_fault(UpnpSoapMessage *thiz, const char *name)
{
    bool ret = false;

    do
    {
        uint32_t count = 0;
        char group[8][128];
        char *p = NULL;

        /*
        * SOAP-ENV:Fault or
        * s:Fault
        */
        memset(group, 0, 8 * 128);
        count = str_split(name, ":", group, 8);
        if (count < 2)
        {
            break;
        }

        if (str_equal(group[1], "Fault", true))
        {
            ret = true;
        }
    } while (0);

    return ret;
}

#if 0
static CtRet get_action_name(UpnpSoapMessage *thiz, const char *name)
{
    CtRet ret = CT_RET_OK;

    do
    {
        uint32_t count = 0;
        char group[8][128];
        char action_response[SOAP_ACTION_NAME_LEN];
        char *p = NULL;

        count = str_split(name, ":", group, 8);
        if (count == 0)
        {
            ret = CT_RET_E_NOT_FOUND;
            break;
        }

        if (count == 1)
        {
            strncpy(action_response, group[0], SOAP_ACTION_NAME_LEN);
            action_response[SOAP_ACTION_NAME_LEN - 1] = 0;
        }
        else
        {
            strncpy(action_response, group[1], SOAP_ACTION_NAME_LEN);
            action_response[SOAP_ACTION_NAME_LEN - 1] = 0;
        }

        p = strstr(action_response, "Response");
        if (p == NULL)
        {
            ret = CT_RET_E_NOT_FOUND;
            break;
        }

        strncpy(thiz->action_name, action_response, p - action_response);
    } while (0);

    return ret;
}
#endif

#if 0
static CtRet get_action_xmlns(UpnpSoapMessage *thiz, CtList *attrs)
{
    CtRet ret = CT_RET_OK;

    do
    {
        TinyXmlAttr *attr = NULL;

        if (attrs == NULL)
        {
            LOG_D(TAG, "Not Found attributes");
            ret = CT_RET_E_NOT_FOUND;
            break;
        }

        attr = (TinyXmlAttr *)CtList_GetAt(attrs, 0);
        if (attr == NULL)
        {
            LOG_D(TAG, "Not Found xmlns");
            ret = CT_RET_E_NOT_FOUND;
            break;
        }

        strncpy(thiz->action_xmlns, attr->value, SOAP_ACTION_XMLNS_LEN);
        thiz->action_xmlns[SOAP_ACTION_XMLNS_LEN - 1] = 0;
    } while (0);

    return ret;
}
#endif