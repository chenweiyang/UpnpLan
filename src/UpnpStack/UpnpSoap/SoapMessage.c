/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   SoapMessage.c
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#include "SoapMessage.h"
#include "tiny_memory.h"
#include "tiny_str_split.h"
#include "tiny_str_equal.h"
#include "tiny_log.h"
#include "TinyList.h"
#include "TinyXml.h"
#include "PropertyList.h"
#include "SoapDefinition.h"

#define TAG                             "SoapMessage"

static TinyRet SoapMessage_Construct(SoapMessage *thiz);
static TinyRet SoapMessage_Dispose(SoapMessage *thiz);
static TinyRet SoapMessage_InitializeProperty(SoapMessage *thiz);
static TinyRet SoapMessage_InitializeFault(SoapMessage *thiz);
static TinyRet SoapMessage_ParseXml(SoapMessage *thiz, TinyXml *xml);

static bool is_envelope(SoapMessage *thiz, TinyXmlNode *root);
static TinyRet load_body(SoapMessage *thiz, TinyXmlNode *root);
static TinyRet load_soap_fault(SoapMessage *thiz, TinyXmlNode *fault);
static TinyRet get_action_name(SoapMessage *thiz, const char *name);
static TinyRet get_action_xmlns(SoapMessage *thiz, TinyXmlNode *response);

struct _SoapMessage
{
    PropertyList *propertyList;
    PropertyList *argumentList;
    PropertyList *fault;
};

SoapMessage * SoapMessage_New(void)
{
    SoapMessage *thiz = NULL;

    do
    {
        TinyRet ret = TINY_RET_OK;

        thiz = (SoapMessage *)tiny_malloc(sizeof(SoapMessage));
        if (thiz == NULL)
        {
            break;
        }

        ret = SoapMessage_Construct(thiz);
        if (RET_FAILED(ret))
        {
            SoapMessage_Delete(thiz);
            thiz = NULL;
            break;
        }

        ret = SoapMessage_InitializeProperty(thiz);
        if (RET_FAILED(ret))
        {
            SoapMessage_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

static TinyRet SoapMessage_Construct(SoapMessage *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(SoapMessage));

        thiz->propertyList = PropertyList_New();
        if (thiz->propertyList == NULL)
        {
            ret = TINY_RET_E_NEW;
            break;
        }

        thiz->argumentList = PropertyList_New();
        if (thiz->argumentList == NULL)
        {
            ret = TINY_RET_E_NEW;
            break;
        }
    } while (0);

    return ret;
}

static TinyRet SoapMessage_Dispose(SoapMessage *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

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

    return TINY_RET_OK;
}

void SoapMessage_Delete(SoapMessage *thiz)
{
    RETURN_IF_FAIL(thiz);
    SoapMessage_Dispose(thiz);
    tiny_free(thiz);
}

static TinyRet SoapMessage_InitializeProperty(SoapMessage *thiz)
{
    TinyRet ret = TINY_RET_OK;
    ObjectType type;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    ObjectType_Construct(&type);

    do
    {
        ObjectType_SetType(&type, CLAZZ_STRING);

        ret = PropertyList_InitProperty(thiz->propertyList, SOAP_ServerURL, &type);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = PropertyList_InitProperty(thiz->propertyList, SOAP_ActionName, &type);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = PropertyList_InitProperty(thiz->propertyList, SOAP_ActionXmlns, &type);
        if (RET_FAILED(ret))
        {
            break;
        }
    } while (0);

    ObjectType_Dispose(&type);

    return ret;
}

static TinyRet SoapMessage_InitializeFault(SoapMessage *thiz)
{
    TinyRet ret = TINY_RET_OK;
    ObjectType type;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    ObjectType_Construct(&type);

    do
    {
        ObjectType_SetType(&type, CLAZZ_STRING);

        ret = PropertyList_InitProperty(thiz->fault, SOAP_FAULT_Code, &type);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = PropertyList_InitProperty(thiz->fault, SOAP_FAULT_String, &type);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = PropertyList_InitProperty(thiz->fault, SOAP_FAULT_ErrorCode, &type);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = PropertyList_InitProperty(thiz->fault, SOAP_FAULT_ErrorDescription, &type);
        if (RET_FAILED(ret))
        {
            break;
        }

    } while (0);

    ObjectType_Dispose(&type);

    return ret;
}

PropertyList *SoapMessage_GetArgumentList(SoapMessage *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->argumentList;
}

PropertyList *SoapMessage_GetFault(SoapMessage *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->fault;
}

TinyRet SoapMessage_Parse(SoapMessage *thiz, const char *bytes, uint32_t len)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(bytes, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(len, TINY_RET_E_ARG_NULL);

    do
    {
        TinyXml *xml;

        SoapMessage_Dispose(thiz);
        SoapMessage_Construct(thiz);

        xml = TinyXml_New();
        if (xml == NULL)
        {
            LOG_D(TAG, "Out of memory");
            ret = TINY_RET_E_NEW;
            break;
        }

        do
        {
            ret = TinyXml_Parse(xml, bytes, len);
            if (RET_FAILED(ret))
            {
                LOG_D(TAG, "TinyXml_Parse failed: %s", tiny_ret_to_str(ret));
                break;
            }

            ret = SoapMessage_ParseXml(thiz, xml);
            if (RET_FAILED(ret))
            {
                LOG_D(TAG, "SoapMessage_ParseXml failed: %s", tiny_ret_to_str(ret));
                break;
            }
        } while (0);

        TinyXml_Delete(xml);

    } while (0);

    return ret;
}

static TinyRet SoapMessage_ParseXml(SoapMessage *thiz, TinyXml *xml)
{
    LOG_TIME_BEGIN(TAG, SoapMessage_ParseXml);
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(xml, TINY_RET_E_ARG_NULL);

    LOG_D(TAG, "SoapMessage_ParseXml ...");

    do
    {
        TinyXmlNode *root = TinyXml_GetRoot(xml);
        if (root == NULL)
        {
            ret = TINY_RET_E_XML_INVALID;
            break;
        }

        if (!is_envelope(thiz, root))
        {
            ret = TINY_RET_E_XML_INVALID;
            break;
        }

        ret = load_body(thiz, root);
        if (RET_FAILED(ret))
        {
            break;
        }

    } while (0);

    LOG_TIME_END(TAG, SoapMessage_ParseXml);

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

TinyRet SoapMessage_ToString(SoapMessage *thiz, char *bytes, uint32_t len)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(bytes, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(len, TINY_RET_E_ARG_NULL);

    do
    {
        Object *actionName = NULL;
        Object *actionXmlns = NULL;
        uint32_t i = 0;
        uint32_t count = 0;
        int32_t unused = 0;
        char *p = NULL;

        tiny_snprintf(bytes, len, "%s%s%s%s%s",
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
            ret = TINY_RET_E_ARG_INVALID;
            break;
        }

        actionName = PropertyList_GetPropertyValue(thiz->propertyList, SOAP_ActionName);
        if (actionName == NULL)
        {
            ret = TINY_RET_E_ARG_INVALID;
            break;
        }

        actionXmlns = PropertyList_GetPropertyValue(thiz->propertyList, SOAP_ActionXmlns);
        if (actionXmlns == NULL)
        {
            ret = TINY_RET_E_ARG_INVALID;
            break;
        }

        tiny_snprintf(p, unused, SOAP_ACTION_BEGIN, actionName->value.stringValue, actionXmlns->value.stringValue);
        p[unused - 1] = 0;

        p += strlen(p);
        unused = len - (p - bytes);
        if (unused < 0)
        {
            ret = TINY_RET_E_ARG_INVALID;
            break;
        }

        count = PropertyList_GetSize(thiz->argumentList);
        for (i = 0; i < count; i++)
        {
            Property *property = PropertyList_GetPropertyAt(thiz->argumentList, i);
            const char *name = property->definition.name;
            ClazzType clazzType = property->definition.type.clazzType;
            ObjectValue *v = &(property->value.object.value);

            switch (clazzType)
            {
            case CLAZZ_UNDEFINED:
                LOG_E(TAG, "invalid type");
                break;

            case CLAZZ_BYTE:
                tiny_snprintf(p, unused, "<%s>%d</%s>\n", name, v->byteValue, name);
                break;

            case CLAZZ_WORD:
                tiny_snprintf(p, unused, "<%s>%d</%s>\n", name, v->wordValue, name);
                break;

            case CLAZZ_INTEGER:
                tiny_snprintf(p, unused, "<%s>%d</%s>\n", name, v->integerValue, name);
                break;

            case CLAZZ_LONG:
                tiny_snprintf(p, unused, "<%s>%l</%s>\n", name, v->longValue, name);
                break;

            case CLAZZ_FLOAT:
                tiny_snprintf(p, unused, "<%s>%f</%s>\n", name, v->floatValue, name);
                break;

            case CLAZZ_DOUBLE:
                tiny_snprintf(p, unused, "<%s>%f</%s>\n", name, v->doubleValue, name);
                break;

            case CLAZZ_BOOLEAN:
                tiny_snprintf(p, unused, "<%s>%s</%s>\n", name, ObjectType_BooleanToString(v->boolValue), name);
                break;

            case CLAZZ_CHAR:
                tiny_snprintf(p, unused, "<%s>%c</%s>\n", name, v->charValue, name);
                break;

            case CLAZZ_STRING:
                tiny_snprintf(p, unused, "<%s>%s</%s>\n", name, v->stringValue, name);
                break;

            default:
                LOG_E(TAG, "invalid type");
                break;
            }

            p[unused - 1] = 0;

            p += strlen(p);
            unused = len - (p - bytes);

            if (unused < 0)
            {
                ret = TINY_RET_E_ARG_INVALID;
                break;
            }
        }

        tiny_snprintf(p, unused, SOAP_ACTION_END, actionName->value.stringValue);
        p[unused - 1] = 0;

        p += strlen(p);
        unused = len - (p - bytes);
        if (unused < 0)
        {
            ret = TINY_RET_E_ARG_INVALID;
            break;
        }

        tiny_snprintf(p, unused, "%s%s",
            SOAP_BODY_END,
            SOAP_ENVELOPE_END);
        p[unused - 1] = 0;

    } while (0);

    return ret;
}

TinyRet SoapMessage_SetPropertyValue(SoapMessage *thiz, const char *propertyName, const char *value)
{
    TinyRet ret = TINY_RET_OK;
    Object data;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(propertyName, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(value, TINY_RET_E_ARG_NULL);

    Object_Construct(&data);
    {
        Object_setString(&data, value);
        ret = PropertyList_SetPropertyValue(thiz->propertyList, propertyName, &data);
    }
    Object_Dispose(&data);

    return ret;
}

const char * SoapMessage_GetPropertyValue(SoapMessage *thiz, const char *propertyName)
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

TinyRet SoapMessage_SetFaultValue(SoapMessage *thiz, const char *faultName, const char *value)
{
    TinyRet ret = TINY_RET_OK;
    Object data;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(faultName, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(value, TINY_RET_E_ARG_NULL);

    Object_Construct(&data);
    {
        Object_setString(&data, value);
        ret = PropertyList_SetPropertyValue(thiz->propertyList, faultName, &data);
    }
    Object_Dispose(&data);

    return ret;
}

const char * SoapMessage_GetFaultValue(SoapMessage *thiz, const char *faultName)
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

static bool is_envelope(SoapMessage *thiz, TinyXmlNode *root)
{
    bool result = false;

    if (strstr(TinyXmlNode_GetName(root), "Envelope") != NULL)
    {
        result = true;
    }

    return result;
}

static TinyRet load_body(SoapMessage *thiz, TinyXmlNode *root)
{
    TinyRet ret = TINY_RET_OK;

    do
    {
        TinyXmlNode *body = NULL;
        TinyXmlNode *response = NULL;
        uint32_t count = 0;
        uint32_t i = 0;

        /**
         * <Body>
         */
        body = TinyXmlNode_GetChildByName(root, "Body");
        if (body == NULL)
        {
            ret = TINY_RET_E_NOT_FOUND;
            break;
        }

        response = TinyXmlNode_GetChildAt(body, 0);
        if (response == NULL)
        {
            ret = TINY_RET_E_NOT_FOUND;
            break;
        }

        /**
         * <Fault
         */
        if (str_equal(TinyXmlNode_GetName(response), "Fault", true))
        {
            ret = SoapMessage_InitializeFault(thiz);
            if (RET_FAILED(ret))
            {
                break;
            }

            ret = load_soap_fault(thiz, response);
            break;
        }

        /**
         * Get the action name
         *
         * [action name][Response]
         */
        ret = get_action_name(thiz, TinyXmlNode_GetName(response));
        if (RET_FAILED(ret))
        {
            break;
        }

        /**
         * Get the xmlns
         * xmlns:m="urn:schemas-upnp-org:service:AVTransport:1" or
         * xmlns:u="GetDeviceCapabilities"
         */
        ret = get_action_xmlns(thiz, response);
        if (RET_FAILED(ret))
        {
            break;
        }

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
#define SOAP_FAULT_DETAIL_ERR_CODE      "errorCode"
#define SOAP_FAULT_DETAIL_ERR_DESC      "errorDescription"

static TinyRet load_soap_fault(SoapMessage *thiz, TinyXmlNode *fault)
{
    TinyRet ret = TINY_RET_OK;

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
            ret = TINY_RET_E_XML_INVALID;
            break;
        }

        ret = PropertyList_SetPropertyStringValue(thiz->fault, SOAP_FAULT_Code, faultcode);
        if (RET_FAILED(ret))
        {
            break;
        }

        faultstring = TinyXmlNode_GetChildContent(fault, SOAP_FAULT_STRING);
        if (faultstring == NULL)
        {
            ret = TINY_RET_E_XML_INVALID;
            break;
        }

        ret = PropertyList_SetPropertyStringValue(thiz->fault, SOAP_FAULT_String, faultstring);
        if (RET_FAILED(ret))
        {
            break;
        }

        /* <detail> */
        detail = TinyXmlNode_GetChildByName(fault, SOAP_FAULT_DETAIL);
        if (detail == NULL)
        {
            LOG_D(TAG, "<detail> not found");
            ret = TINY_RET_E_NOT_FOUND;
            break;
        }

        /* <UPnPError> */
        detail_fault = TinyXmlNode_GetChildAt(detail, 0);
        if (fault == NULL)
        {
            LOG_D(TAG, "<detail> has not child");
            ret = TINY_RET_E_NOT_FOUND;
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
                ret = TINY_RET_E_NOT_FOUND;
                break;
            }

            error_description = TinyXmlNode_GetChildContent(detail_fault, SOAP_FAULT_DETAIL_ERR_DESC);
            if (error_description == NULL)
            {
                LOG_D(TAG, "<%s> not found", SOAP_FAULT_DETAIL_ERR_DESC);
                ret = TINY_RET_E_NOT_FOUND;
                break;
            }

            ret = PropertyList_SetPropertyIntegerValue(thiz->fault, SOAP_FAULT_ErrorCode, atoi(error_code));
            if (RET_FAILED(ret))
            {
                break;
            }

            ret = PropertyList_SetPropertyStringValue(thiz->fault, SOAP_FAULT_ErrorDescription, error_description);
            if (RET_FAILED(ret))
            {
                break;
            }
        } while (0);
    } while (0);

    return ret;
}

#define SOAP_ACTION_NAME_LEN    128

static TinyRet get_action_name(SoapMessage *thiz, const char *action_response)
{
    TinyRet ret = TINY_RET_OK;

    LOG_D(TAG, "get_action_name: %s", action_response);

    do
    {
        char action_name[SOAP_ACTION_NAME_LEN];
        char *p = NULL;

        memset(action_name, 0, SOAP_ACTION_NAME_LEN);

        p = strstr(action_response, "Response");
        if (p == NULL)
        {
            ret = TINY_RET_E_NOT_FOUND;
            break;
        }

        if ((p - action_response) > SOAP_ACTION_NAME_LEN)
        {
            LOG_E(TAG, "actionName is too long!");
            ret = TINY_RET_E_NOT_FOUND;
            break;
        }

        strncpy(action_name, action_response, p - action_response);

        LOG_D(TAG, "actionName is: %s", action_name);

        SoapMessage_SetPropertyValue(thiz, SOAP_ActionName, action_name);
    } while (0);

    return ret;
}

static TinyRet get_action_xmlns(SoapMessage *thiz, TinyXmlNode *response)
{
    TinyRet ret = TINY_RET_OK;

    do
    {
        TinyXmlAttr *attr = TinyXmlNode_GetAttr(response, "xmlns");
        if (attr == NULL)
        {
            LOG_D(TAG, "xmlns: not found");
            ret = TINY_RET_E_NOT_FOUND;
            break;
        }

        SoapMessage_SetPropertyValue(thiz, SOAP_ActionXmlns, attr->value);
    } while (0);

    return ret;
}