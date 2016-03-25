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
#include "tiny_memory.h"
#include "tiny_log.h"

#define TAG         "UpnpEvent"

static TinyRet UpnpEvent_Initialize(UpnpEvent *thiz);

static TinyRet load_content(UpnpEvent *thiz, const char *bytes, uint32_t len);
static TinyRet load_xml(UpnpEvent *thiz, TinyXml *xml);
static TinyRet load_propertyset(UpnpEvent *thiz, TinyXmlNode *root);


UpnpEvent * UpnpEvent_New(void)
{
    UpnpEvent *thiz = NULL;

    do
    {
        TinyRet ret = TINY_RET_OK;

        thiz = (UpnpEvent *)tiny_malloc(sizeof(UpnpEvent));
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

TinyRet UpnpEvent_Construct(UpnpEvent *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(UpnpEvent));

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

void UpnpEvent_Dispose(UpnpEvent *thiz)
{
    RETURN_IF_FAIL(thiz);

    PropertyList_Delete(thiz->propertyList);
    PropertyList_Delete(thiz->argumentList);
}

void UpnpEvent_Delete(UpnpEvent *thiz)
{
    RETURN_IF_FAIL(thiz);

    UpnpEvent_Dispose(thiz);
    tiny_free(thiz);
}

static TinyRet UpnpEvent_Initialize(UpnpEvent *thiz)
{
    TinyRet ret = TINY_RET_OK;
    ObjectType type;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

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

TinyRet UpnpEvent_SetPropertyValue(UpnpEvent *thiz, const char *propertyName, const char *value)
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

TinyRet UpnpEvent_SetArgumentValue(UpnpEvent *thiz, const char *argumentName, const char *value)
{
    TinyRet ret = TINY_RET_OK;
    Object data;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(argumentName, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(value, TINY_RET_E_ARG_NULL);

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
    RETURN_VAL_IF_FAIL(argumentName, NULL);

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

TinyRet UpnpEvent_Parse(UpnpEvent *thiz, const char *nt, const char *nts, const char *sid, const char *seq, const char *content, uint32_t contentLength)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(nt, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(nts, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(sid, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(seq, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(content, TINY_RET_E_ARG_NULL);

    do
    {
        UpnpEvent_SetPropertyValue(thiz, UPNP_EVENT_Nt, nt);
        UpnpEvent_SetPropertyValue(thiz, UPNP_EVENT_Nts, nts);
        UpnpEvent_SetPropertyValue(thiz, UPNP_EVENT_Sid, sid);
        UpnpEvent_SetPropertyValue(thiz, UPNP_EVENT_Seq, seq);

        ret = load_content(thiz, content, contentLength);
        if (RET_FAILED(ret))
        {
            break;
        }
    } while (0);

    return ret;
}

TinyRet UpnpEvent_ToString(UpnpEvent *thiz, char *bytes, uint32_t len)
{
    return TINY_RET_E_NOT_IMPLEMENTED;
}

static TinyRet load_content(UpnpEvent *thiz, const char *bytes, uint32_t len)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(bytes, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(len, TINY_RET_E_ARG_NULL);

    do
    {
        TinyXml * xml;

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

            ret = load_xml(thiz, xml);
            if (RET_FAILED(ret))
            {
                LOG_D(TAG, "parse_event_xml failed: %s", tiny_ret_to_str(ret));
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
static TinyRet load_xml(UpnpEvent *thiz, TinyXml *xml)
{
    LOG_TIME_BEGIN(TAG, load_xml);
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(xml, TINY_RET_E_ARG_NULL);

    do
    {
        TinyXmlNode *root = TinyXml_GetRoot(xml);
        if (root == NULL)
        {
            ret = TINY_RET_E_XML_INVALID;
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

static TinyRet load_propertyset(UpnpEvent *thiz, TinyXmlNode *root)
{
    TinyRet ret = TINY_RET_OK;

    do
    {
        uint32_t count = 0;
        uint32_t i = 0;

        if (strstr(TinyXmlNode_GetName(root), "propertyset") == NULL)
        {
            ret = TINY_RET_E_NOT_FOUND;
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

                /**
                 * MUST init property
                 */
                ObjectType type;
                ObjectType_SetType(&type, CLAZZ_STRING);

                PropertyList_InitProperty(thiz->argumentList, name, &type);

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