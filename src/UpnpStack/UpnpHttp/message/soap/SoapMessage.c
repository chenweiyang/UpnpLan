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

#define TAG                             "SoapMessage"

static TinyRet SoapMessage_Construct(SoapMessage *thiz);
static TinyRet SoapMessage_Dispose(SoapMessage *thiz);
static TinyRet SoapMessage_ParseXml(SoapMessage *thiz, TinyXml *xml);

static bool is_envelope(SoapMessage *thiz, TinyXmlNode *root);
static TinyRet load_body(SoapMessage *thiz, TinyXmlNode *root);
static TinyRet load_soap_fault(SoapMessage *thiz, TinyXmlNode *fault);
static TinyRet get_action_name(SoapMessage *thiz, const char *name);
static TinyRet get_action_xmlns(SoapMessage *thiz, TinyXmlNode *response);


#define FAULT_STRING_LEN        512
typedef struct _SoapFault
{
    int faultCode;
    char faultString[FAULT_STRING_LEN];
} SoapFault;

#define ERROR_DESC_LEN          512
typedef struct _SoapError
{
    int errorCode;
    char errorDescription[ERROR_DESC_LEN];
} SoapError;

#define ACTION_NAME_LEN         128
#define ACTION_XMLNS_LEN        256

struct _SoapMessage
{
    char serverURL[TINY_URL_LEN];
    char actionName[ACTION_NAME_LEN];
    char actionXmlns[ACTION_XMLNS_LEN];
    bool isFault;
    SoapFault fault;
    SoapError error;
    PropertyList *argumentList;
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
        thiz->isFault = false;

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

    if (thiz->argumentList != NULL)
    {
        PropertyList_Delete(thiz->argumentList);
    }

    return TINY_RET_OK;
}

void SoapMessage_Delete(SoapMessage *thiz)
{
    RETURN_IF_FAIL(thiz);
    SoapMessage_Dispose(thiz);
    tiny_free(thiz);
}

PropertyList *SoapMessage_GetArgumentList(SoapMessage *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->argumentList;
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

        tiny_snprintf(p, unused, SOAP_ACTION_BEGIN, thiz->actionName, thiz->actionXmlns);
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
#if (defined _WIN32) || (defined __MAC_OSX__)

                tiny_snprintf(p, unused, "<%s>%lld</%s>\n", name, v->longValue, name);
#else
                tiny_snprintf(p, unused, "<%s>%ld</%s>\n", name, v->longValue, name);
#endif
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

        tiny_snprintf(p, unused, SOAP_ACTION_END, thiz->actionName);
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

TinyRet SoapMessage_SetServerURL(SoapMessage *thiz, const char *serverURL)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(serverURL, TINY_RET_E_ARG_NULL);

    strncpy(thiz->serverURL, serverURL, TINY_URL_LEN);

    return TINY_RET_OK;
}

TinyRet SoapMessage_SetActionName(SoapMessage *thiz, const char *actionName)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(actionName, TINY_RET_E_ARG_NULL);

    strncpy(thiz->actionName, actionName, ACTION_NAME_LEN);

    return TINY_RET_OK;
}

TinyRet SoapMessage_SetActionXmlns(SoapMessage *thiz, const char *actionXmlns)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(actionXmlns, TINY_RET_E_ARG_NULL);

    strncpy(thiz->actionXmlns, actionXmlns, ACTION_XMLNS_LEN);

    return TINY_RET_OK;
}

const char * SoapMessage_GetServerURL(SoapMessage *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->serverURL;
}

const char * SoapMessage_GetActionName(SoapMessage *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->actionName;
}

const char * SoapMessage_GetActionXmlns(SoapMessage *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->actionXmlns;
}

bool SoapMessage_IsFault(SoapMessage *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, false);

    return thiz->isFault;
}

TinyRet SoapMessage_SetFault(SoapMessage *thiz, int faultcode, const char *faultstring)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    thiz->fault.faultCode = faultcode;
    strncpy(thiz->fault.faultString, faultstring, FAULT_STRING_LEN);

    return TINY_RET_OK;
}

TinyRet SoapMessage_SetError(SoapMessage *thiz, int errorCode, const char *errorDescription)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    thiz->error.errorCode = errorCode;
    strncpy(thiz->error.errorDescription, errorDescription, ERROR_DESC_LEN);

    return TINY_RET_OK;
}

int SoapMessage_GetFaultcode(SoapMessage *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, 0);

    return thiz->fault.faultCode;
}

const char * SoapMessage_GetFaultstring(SoapMessage *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->fault.faultString;
}

int SoapMessage_GetErrorCode(SoapMessage *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, 0);

    return thiz->error.errorCode;
}

const char * SoapMessage_GetErrorDescription(SoapMessage *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->error.errorDescription;
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
            thiz->isFault = true;
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

        faultstring = TinyXmlNode_GetChildContent(fault, SOAP_FAULT_STRING);
        if (faultstring == NULL)
        {
            ret = TINY_RET_E_XML_INVALID;
            break;
        }

        ret = SoapMessage_SetFault(thiz, atoi(faultcode), faultstring);
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

            ret = SoapMessage_SetError(thiz, atoi(error_code), error_description);
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

        SoapMessage_SetActionName(thiz, action_name);
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

        SoapMessage_SetActionXmlns(thiz, attr->value);
    } while (0);

    return ret;
}
