/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   UpnpEvent.c
*
* @remark
*
*/

#include "UpnpEvent.h"
#include "UpnpEventDefinition.h"
#include "TinyXml.h"
#include "ct_memory.h"
#include "ct_log.h"

#define TAG         "UpnpEvent"

static CtRet UpnpEvent_Construct(UpnpEvent *thiz);
static void UpnpEvent_Dispose(UpnpEvent *thiz);
static CtRet UpnpEvent_Initialize(UpnpEvent *thiz);

static CtRet load_content(UpnpEvent *thiz, const char *bytes, uint32_t len);
static CtRet load_xml(UpnpEvent *thiz, TinyXml *xml);
static CtRet load_propertyset(UpnpEvent *thiz, TinyXmlNode *root);

struct _UpnpEvent
{
    PropertyList * propertyList;
    PropertyList * argumentList;
};

UpnpEvent * UpnpEvent_New(void)
{
    UpnpEvent *thiz = NULL;

    do
    {
        CtRet ret = CT_RET_OK;

        thiz = (UpnpEvent *)ct_malloc(sizeof(UpnpEvent));
        if (thiz == NULL)
        {
            break;
        }

        ret = UpnpEvent_Construct(thiz);
        if (RET_FAILED(ret))
        {
            UpnpEvent_Delete(thiz);
            thiz = NULL;
            break;
        }

        ret = UpnpEvent_Initialize(thiz);
        if (RET_FAILED(ret))
        {
            UpnpEvent_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

void UpnpEvent_Delete(UpnpEvent *thiz)
{
    RETURN_IF_FAIL(thiz);

    UpnpEvent_Dispose(thiz);
    ct_free(thiz);
}

static CtRet UpnpEvent_Construct(UpnpEvent *thiz)
{
    CtRet ret = CT_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, CT_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(UpnpEvent));

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

static void UpnpEvent_Dispose(UpnpEvent *thiz)
{
    RETURN_IF_FAIL(thiz);

    PropertyList_Delete(thiz->propertyList);
    PropertyList_Delete(thiz->argumentList);
}

static CtRet UpnpEvent_Initialize(UpnpEvent *thiz)
{
    CtRet ret = CT_RET_OK;
    ObjectType type;

    RETURN_VAL_IF_FAIL(thiz, CT_RET_E_ARG_NULL);

    ObjectType_Construct(&type);

    do
    {
        ObjectType_SetType(&type, CLAZZ_STRING);

        ret = PropertyList_InitProperty(thiz->propertyList, UPNP_EVENT_Connection, &type);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = PropertyList_InitProperty(thiz->propertyList, UPNP_EVENT_Nt, &type);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = PropertyList_InitProperty(thiz->propertyList, UPNP_EVENT_Nts, &type);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = PropertyList_InitProperty(thiz->propertyList, UPNP_EVENT_Sid, &type);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = PropertyList_InitProperty(thiz->propertyList, UPNP_EVENT_Seq, &type);
        if (RET_FAILED(ret))
        {
            break;
        }
    } while (0);

    ObjectType_Dispose(&type);

    return ret;
}

CtRet UpnpEvent_SetPropertyValue(UpnpEvent *thiz, const char *propertyName, const char *value)
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

const char * UpnpEvent_GetPropertyValue(UpnpEvent *thiz, const char *propertyName)
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

PropertyList* UpnpEvent_GetArgumentList(UpnpEvent *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->argumentList;
}

CtRet UpnpEvent_SetArgumentValue(UpnpEvent *thiz, const char *argumentName, const char *value)
{
    CtRet ret = CT_RET_OK;
    Object data;

    RETURN_VAL_IF_FAIL(thiz, NULL);
    RETURN_VAL_IF_FAIL(propertyName, NULL);
    RETURN_VAL_IF_FAIL(value, NULL);

    Object_Construct(&data);
    {
        Object_setString(&data, value);
        ret = PropertyList_SetPropertyValue(thiz->argumentList, argumentName, &data);
    }
    Object_Dispose(&data);

    return ret;
}

const char * UpnpEvent_GetArgumentValue(UpnpEvent *thiz, const char *argumentName)
{
    const char *value = NULL;
    Object *data = NULL;

    RETURN_VAL_IF_FAIL(thiz, NULL);
    RETURN_VAL_IF_FAIL(propertyName, NULL);

    data = PropertyList_GetPropertyValue(thiz->argumentList, argumentName);
    if (data != NULL)
    {
        value = data->value.stringValue;
    }

    return value;
}

/*
NOTIFY /upnphost/udhisapi.dll?event=uuid:9ba32c90-9923-4ec6-81d0-335100229b91+urn:upnp-org:serviceId:RenderingControl HTTP/1.1
Cache-Control: no-cache
Connection: Close
Pragma: no-cache
Content-Type: text/xml; charset="utf-8"
User-Agent: Microsoft-Windows/6.1 UPnP/1.0
NT: upnp:event
NTS: upnp:propchange
SID: uuid:4db40ba6-b0a3-4014-aa1c-c294c7a129d8
SEQ: 0
Content-Length: 452
Host: 10.0.1.3:1607

<?xml version="1.0"?>
<e:propertyset xmlns:e="urn:schemas-upnp-org:event-1-0">
<e:property>
<LastChange xmlns:dt="urn:schemas-microsoft-com:datatypes" dt:dt="string">
&lt;Event xmlns="urn:schemas-upnp-org:metadata-1-0/RCS/"&gt;&lt;
InstanceID val="0"&gt;&lt;Mute channel="Master" val="0"/&gt;&lt;
Volume channel="Master" val="60"/&gt;&lt;
PresetNameList val="FactoryDefaults"/&gt;&lt;/
InstanceID&gt;&lt;/Event&gt;
</LastChange>
</e:property>
</e:propertyset>
*/
CtRet UpnpEvent_Parse(UpnpEvent *thiz, HttpMessage *request)
{
    CtRet ret = CT_RET_OK;

    do
    {
        const char *connection = NULL;
        const char *content_type = NULL;
        const char *nt = NULL;
        const char *nts = NULL;
        const char *sid = NULL;
        const char *seq = NULL;

        if (HttpMessage_GetType(request) != HTTP_REQUEST)
        {
            ret = CT_RET_E_HTTP_TYPE_INVALID;
            break;
        }

        connection = HttpMessage_GetHeaderValue(request, "Connection");
        content_type = HttpMessage_GetHeaderValue(request, "Content-Type");
        nt = HttpMessage_GetHeaderValue(request, "NT");
        nts = HttpMessage_GetHeaderValue(request, "NTS");
        sid = HttpMessage_GetHeaderValue(request, "SID");
        seq = HttpMessage_GetHeaderValue(request, "SEQ");

        if (connection != NULL)
        {
            UpnpEvent_SetPropertyValue(thiz, UPNP_EVENT_Connection, connection);
        }

        if (nt != NULL)
        {
            UpnpEvent_SetPropertyValue(thiz, UPNP_EVENT_Nt, nt);
        }

        if (nts != NULL)
        {
            UpnpEvent_SetPropertyValue(thiz, UPNP_EVENT_Nts, nts);
        }

        if (sid != NULL)
        {
            UpnpEvent_SetPropertyValue(thiz, UPNP_EVENT_Sid, sid);
        }

        if (seq != NULL)
        {
            UpnpEvent_SetPropertyValue(thiz, UPNP_EVENT_Seq, seq);
        }

        ret = load_content(thiz, HttpMessage_GetContentObject(request), HttpMessage_GetContentSize(request));
        if (RET_FAILED(ret))
        {
            break;
        }
    } while (0);

    return ret;
}

CtRet UpnpEvent_ToString(UpnpEvent *thiz, char *bytes, uint32_t len)
{
    return CT_RET_E_NOT_IMPLEMENTED;
}

static CtRet load_content(UpnpEvent *thiz, const char *bytes, uint32_t len)
{
    CtRet ret = CT_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, CT_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(bytes, CT_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(len, CT_RET_E_ARG_NULL);

    do
    {
        TinyXml * xml;

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

            ret = load_xml(thiz, xml);
            if (RET_FAILED(ret))
            {
                LOG_D(TAG, "parse_event_xml failed: %s", ct_ret_to_str(ret));
                break;
            }
        } while (0);

        TinyXml_Delete(xml);

    } while (0);

    return ret;
}

/*
<?xml version="1.0"?>
<e:propertyset xmlns:e="urn:schemas-upnp-org:event-1-0">
<e:property>
<LastChange xmlns:dt="urn:schemas-microsoft-com:datatypes" dt:dt="string">
&lt;Event xmlns="urn:schemas-upnp-org:metadata-1-0/RCS/"&gt;&lt;
InstanceID val="0"&gt;&lt;Mute channel="Master" val="0"/&gt;&lt;
Volume channel="Master" val="60"/&gt;&lt;
PresetNameList val="FactoryDefaults"/&gt;&lt;/
InstanceID&gt;&lt;/Event&gt;
</LastChange>
</e:property>
</e:propertyset>
*/
static CtRet load_xml(UpnpEvent *thiz, TinyXml *xml)
{
    LOG_TIME_BEGIN(TAG, load_xml);
    CtRet ret = CT_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, CT_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(xml, CT_RET_E_ARG_NULL);

    do
    {
        TinyXmlNode *root = TinyXml_GetRoot(xml);
        if (root == NULL)
        {
            ret = CT_RET_E_XML_INVALID;
            break;
        }

        ret = load_propertyset(thiz, root);
        if (RET_FAILED(ret))
        {
            break;
        }
    } while (0);

    LOG_TIME_END(TAG, load_xml);

    return ret;
}

static CtRet load_propertyset(UpnpEvent *thiz, TinyXmlNode *root)
{
    CtRet ret = CT_RET_OK;

    do
    {
        uint32_t count = 0;
        uint32_t i = 0;

        if (strstr(TinyXmlNode_GetName(root), "propertyset") == NULL)
        {
            ret = CT_RET_E_NOT_FOUND;
            break;
        }

        count = TinyXmlNode_GetChildren(root);
        for (i = 0; i < count; i++)
        {
            TinyXmlNode *property = TinyXmlNode_GetChildAt(root, i);
            TinyXmlNode *child = TinyXmlNode_GetChildAt(property, 0);
            if (child != NULL)
            {
                const char *name = TinyXmlNode_GetName(child);
                const char *value = TinyXmlNode_GetContent(child);

                if (value == NULL)
                {
                    UpnpEvent_SetArgumentValue(thiz, name, "");
                }
                else
                {
                    UpnpEvent_SetArgumentValue(thiz, name, value);
                }
            }
        }

    } while (0);

    return ret;
}