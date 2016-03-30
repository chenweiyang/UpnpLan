/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   UpnpDeviceParser.c
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#include "UpnpDeviceParser.h"
#include "tiny_log.h"
#include "tiny_str_equal.h"
#include "tiny_url_split.h"
#include "TinyXml.h"
#include "HttpClient.h"

#define TAG         "UpnpDeviceParser"


#define URL_BASE_LEN                         256
#define DDD_FRIENDLY_NAME_LEN                128
#define DDD_MODEL_NAME_LEN                   128
#define DDD_MODEL_URL_LEN                    128
#define DDD_MANUFACTURER_LEN                 128
#define DDD_MANUFACTURER_URL_LEN             128
#define DDD_UDN_LEN                          128

/* <root> */
#define DDD_ROOT                    "root"

/* <specVersion> */
#define DDD_SEPCVERSION             "specVersion"
#define DDD_SEPCVERSION_MAJOR       "major"
#define DDD_SEPCVERSION_MINOR       "minor"

/* <UrlBase> */
#define DDD_URLBASE                 "URLBase"

/* <device> */
#define DDD_DEVICE                  "device"
#define DDD_DEVICE_UDN              "UDN"
#define DDD_DEVICE_FRIENDLYNAME     "friendlyName"
#define DDD_DEVICE_DEVICETYPE       "deviceType"
#define DDD_DEVICE_MANUFACTURER     "manufacturer"
#define DDD_DEVICE_MANUFACTURERURL  "manufacturerURL"
#define DDD_DEVICE_MODELNAME        "modelName"
#define DDD_DEVICE_MODELNUMBER      "modelNumber"
#define DDD_DEVICE_MODELURL         "modelURL"
#define DDD_DEVICE_SERIALNUMBER     "serialNumber"

/* <iconList> */
#define DDD_ICONLIST                "iconList"
#define DDD_ICON                    "icon"
#define DDD_ICON_MIMETYPE           "mimetype"
#define DDD_ICON_WIDTH              "width"
#define DDD_ICON_HEIGHT             "height"
#define DDD_ICON_DEPTH              "depth"
#define DDD_ICON_URL                "url"

/* <serviceList> */
#define DDD_SERVICELIST             "serviceList"
#define DDD_SERVICE                 "service"
#define DDD_SERVICE_TYPE            "serviceType"
#define DDD_SERVICE_ID              "serviceId"
#define DDD_SERVICE_CONTROLURL      "controlURL"
#define DDD_SERVICE_EVENTSUBURL     "eventSubURL"
#define DDD_SERVICE_SCPDURL         "SCPDURL"

static TinyRet DDD_ParseXml(UpnpDevice *thiz, TinyXml *xml);
static TinyRet DDD_LoadSpecVersion(UpnpDevice *thiz, TinyXmlNode *root);
static TinyRet DDD_LoadDevice(UpnpDevice *thiz, TinyXmlNode *root);
static TinyRet DDD_LoadUrlBase(UpnpDevice *thiz, TinyXmlNode *root);
static TinyRet DDD_LoadIconList(UpnpDevice *thiz, TinyXmlNode *iconList);
static TinyRet DDD_LoadServiceList(UpnpDevice *thiz, TinyXmlNode *serviceList);
static TinyRet UpnpDeviceParser_SetURLBase(UpnpDevice *thiz, const char *deviceUrl);

TinyRet UpnpDeviceParser_Parse(const char *url, UpnpDevice *device, uint32_t timeout)
{
    LOG_TIME_BEGIN(TAG, DDD_Parse);
    TinyRet ret = TINY_RET_OK;
    HttpClient *client = NULL;
    HttpMessage *request = NULL;
    HttpMessage *response = NULL;

    RETURN_VAL_IF_FAIL(device, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(url, TINY_RET_E_ARG_NULL);

    do
    {
        ret = UpnpDevice_SetURLBase(device, url);
        if (RET_FAILED(ret))
        {
            LOG_D(TAG, "UpnpDevice_SetUrlBase failed: %s", tiny_ret_to_str(ret));
            break;
        }

        client = HttpClient_New();
        if (client == NULL)
        {
            ret = TINY_RET_E_NEW;
            break;
        }

        request = HttpMessage_New();
        if (request == NULL)
        {
            ret = TINY_RET_E_NEW;
            break;
        }

        response = HttpMessage_New();
        if (response == NULL)
        {
            ret = TINY_RET_E_NEW;
            break;
        }

        do
        {
            TinyXml *xml;

            ret = HttpMessage_SetRequest(request, "GET", url);
            if (RET_FAILED(ret))
            {
                LOG_D(TAG, "HttpMessage_Set_GET failed: %s", tiny_ret_to_str(ret));
                break;
            }

            ret = HttpClient_Execute(client, request, response, timeout);
            if (RET_FAILED(ret))
            {
                LOG_D(TAG, "HttpClient_Execute failed: %s", tiny_ret_to_str(ret));
                break;
            }

            if (HttpMessage_GetStatusCode(response) != HTTP_STATUS_OK)
            {
                LOG_D(TAG, "HttpMessage_GetStatusCode failed: %d %s",
                    HttpMessage_GetStatusCode(response),
                    HttpMessage_GetStatus(response));
                break;
            }

            xml = TinyXml_New();
            if (xml == NULL)
            {
                LOG_D(TAG, "Out of memory");
                ret = TINY_RET_E_NEW;
                break;
            }

            do
            {
                ret = TinyXml_Parse(xml, HttpMessage_GetContentObject(response), HttpMessage_GetContentSize(response));
                if (RET_FAILED(ret))
                {
                    LOG_D(TAG, "TinyXml_Parse failed: %s", tiny_ret_to_str(ret));
                    break;
                }

                ret = DDD_ParseXml(device, xml);
                if (RET_FAILED(ret))
                {
                    LOG_D(TAG, "DDD_ParseXml failed: %s", tiny_ret_to_str(ret));
                    break;
                }
            } while (0);

            TinyXml_Delete(xml);
        } while (0);
    } while (0);

    if (client != NULL)
    {
        HttpClient_Delete(client);
    }

    if (response != NULL)
    {
        HttpMessage_Delete(response);
    }

    if (request != NULL)
    {
        HttpMessage_Delete(request);
    }

    LOG_TIME_END(TAG, DDD_Parse);
    return ret;
}

static TinyRet DDD_ParseXml(UpnpDevice *thiz, TinyXml *xml)
{
    LOG_TIME_BEGIN(TAG, DDD_ParseXml);
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(xml, TINY_RET_E_ARG_NULL);

    do
    {
        TinyXmlNode *root = TinyXml_GetRoot(xml);
        if (root == NULL)
        {
            LOG_D(TAG, "Root Not Found");
            ret = TINY_RET_E_XML_INVALID;
            break;
        }

        if (!str_equal(TinyXmlNode_GetName(root), DDD_ROOT, true))
        {
            LOG_D(TAG, "Root Tag is invalid: %s", TinyXmlNode_GetName(root));
            ret = TINY_RET_E_XML_INVALID;
            break;
        }

        ret = DDD_LoadSpecVersion(thiz, root);
        if (RET_FAILED(ret))
        {
            LOG_D(TAG, "LoadSpecVersion failed");
            break;
        }

        ret = DDD_LoadDevice(thiz, root);
        if (RET_FAILED(ret))
        {
            LOG_D(TAG, "LoadDevice failed");
            break;
        }

        /* <URLBase> is optional */
        DDD_LoadUrlBase(thiz, root);

    } while (0);

    LOG_TIME_END(TAG, DDD_ParseXml);

    return ret;
}

static TinyRet DDD_LoadSpecVersion(UpnpDevice *thiz, TinyXmlNode *root)
{
    TinyRet ret = TINY_RET_OK;

    do
    {
        TinyXmlNode *version = NULL;
        const char *major = NULL;
        const char *minor = NULL;
        int majorVersion = 0;
        int minorVersion = 0;

        version = TinyXmlNode_GetChildByName(root, DDD_SEPCVERSION);
        if (version == NULL)
        {
            ret = TINY_RET_E_XML_INVALID;
            break;
        }

        major = TinyXmlNode_GetChildContent(version, DDD_SEPCVERSION_MAJOR);
        minor = TinyXmlNode_GetChildContent(version, DDD_SEPCVERSION_MINOR);
        if (major == NULL || minor == NULL)
        {
            ret = TINY_RET_E_XML_INVALID;
            break;
        }

        majorVersion = atoi(major);
        minorVersion = atoi(minor);

        if (majorVersion != 1 || minorVersion != 0)
        {
            LOG_W(TAG, "SpecVersion: %d.%d", majorVersion, minorVersion);
        }
    } while (0);

    return ret;
}

static TinyRet DDD_LoadDevice(UpnpDevice *thiz, TinyXmlNode *root)
{
    TinyRet ret = TINY_RET_OK;

    do
    {
        TinyXmlNode *device = NULL;
        TinyXmlNode *iconList = NULL;
        TinyXmlNode *serviceList = NULL;
        const char *deviceType = NULL;
        const char *friendlyName = NULL;
        const char *modelNumber = NULL;
        const char *modelName = NULL;
        const char *modelURL = NULL;
        const char *manufacturer = NULL;
        const char *manufacturerURL = NULL;
        const char *serialNumber = NULL;
        const char *udn = NULL;

        device = TinyXmlNode_GetChildByName(root, DDD_DEVICE);
        if (device == NULL)
        {
            ret = TINY_RET_E_XML_INVALID;
            break;
        }

        deviceType = TinyXmlNode_GetChildContent(device, DDD_DEVICE_DEVICETYPE);
        friendlyName = TinyXmlNode_GetChildContent(device, DDD_DEVICE_FRIENDLYNAME);
        manufacturer = TinyXmlNode_GetChildContent(device, DDD_DEVICE_MANUFACTURER);
        manufacturerURL = TinyXmlNode_GetChildContent(device, DDD_DEVICE_MANUFACTURERURL);
        modelName = TinyXmlNode_GetChildContent(device, DDD_DEVICE_MODELNAME);
        modelNumber = TinyXmlNode_GetChildContent(device, DDD_DEVICE_MODELNUMBER);
        modelURL = TinyXmlNode_GetChildContent(device, DDD_DEVICE_MODELURL);
        udn = TinyXmlNode_GetChildContent(device, DDD_DEVICE_UDN);
        serialNumber = TinyXmlNode_GetChildContent(device, DDD_DEVICE_SERIALNUMBER);

        if (deviceType != NULL)
        {
            UpnpDevice_SetDeviceType(thiz, deviceType);
        }
        else
        {
            LOG_D(TAG, "deviceType is NULL");
            ret = TINY_RET_E_XML_INVALID;
            break;
        }

        if (friendlyName != NULL)
        {
            UpnpDevice_SetFriendlyName(thiz, friendlyName);
        }
        else
        {
            LOG_D(TAG, "friendly_name is NULL");
        }

        if (modelNumber != NULL)
        {
            UpnpDevice_SetModelNumber(thiz, modelNumber);
        }

        if (modelName != NULL)
        {
            UpnpDevice_SetModelName(thiz, modelName);
        }

        if (modelURL != NULL)
        {
            UpnpDevice_SetModelURL(thiz, modelURL);
        }

        if (udn != NULL)
        {
            UpnpDevice_SetDeviceId(thiz, udn);
        }

        if (manufacturer != NULL)
        {
            UpnpDevice_SetManufacturer(thiz, manufacturer);
        }

        if (manufacturerURL != NULL)
        {
            UpnpDevice_SetManufacturerURL(thiz, manufacturerURL);
        }

        if (serialNumber != NULL)
        {
            UpnpDevice_SetSerialNumber(thiz, serialNumber);
        }

        /* <iconList> is optional */
        iconList = TinyXmlNode_GetChildByName(device, DDD_ICONLIST);
        if (iconList != NULL)
        {
            ret = DDD_LoadIconList(thiz, iconList);
            if (RET_FAILED(ret))
            {
                LOG_D(TAG, "loadIconList failed");
                break;
            }
        }

        /* <serviceList> is required */
        serviceList = TinyXmlNode_GetChildByName(device, DDD_SERVICELIST);
        if (serviceList == NULL)
        {
            LOG_D(TAG, "serviceList is NULL");
            ret = TINY_RET_E_XML_INVALID;
            break;
        }

        ret = DDD_LoadServiceList(thiz, serviceList);
        if (RET_FAILED(ret))
        {
            LOG_D(TAG, "loadServiceList failed");
            break;
        }
    } while (0);

    return ret;
}

static TinyRet DDD_LoadUrlBase(UpnpDevice *thiz, TinyXmlNode *root)
{
    TinyRet ret = TINY_RET_OK;

    do
    {
        const char *url_base = TinyXmlNode_GetChildContent(root, DDD_URLBASE);
        if (url_base == NULL)
        {
            ret = TINY_RET_E_NOT_FOUND;
            break;
        }

        UpnpDevice_SetURLBase(thiz, url_base);
    } while (0);

    return ret;
}

static TinyRet DDD_LoadIconList(UpnpDevice *thiz, TinyXmlNode *iconList)
{
    TinyRet ret = TINY_RET_OK;

#if 0
    do
    {
        uint32_t count = 0;
        uint32_t i = 0;

        count = TinyXmlNode_GetChildren(iconList);
        if (count == 0)
        {
            ret = TINY_RET_E_XML_INVALID;
            break;
        }

        for (i = 0; i < count; i++)
        {
            TinyXmlNode *child = NULL;
            UpnpIcon *icon = NULL;
            const char *mimetype = NULL;
            const char *width = NULL;
            const char *height = NULL;
            const char *depth = NULL;
            const char *url = NULL;

            child = TinyXmlNode_GetChildAt(iconList, i);
            if (!str_equal(TinyXmlNode_GetName(child), DDD_ICON, true))
            {
                continue;
            }

            mimetype = TinyXmlNode_GetChildContent(child, DDD_ICON_MIMETYPE);
            width = TinyXmlNode_GetChildContent(child, DDD_ICON_WIDTH);
            height = TinyXmlNode_GetChildContent(child, DDD_ICON_HEIGHT);
            depth = TinyXmlNode_GetChildContent(child, DDD_ICON_DEPTH);
            url = TinyXmlNode_GetChildContent(child, DDD_ICON_URL);
            if (mimetype == NULL || width == NULL || height == NULL || depth == NULL || url == NULL)
            {
                continue;
            }

            icon = UpnpIcon_New();
            if (icon == NULL)
            {
                ret = TINY_RET_E_OUT_OF_MEMORY;
                break;
            }

            UpnpIcon_Set(icon, mimetype, atoi(width), atoi(height), atoi(depth), url);

            ScList_AddTail(&thiz->icons, icon);
        }
    } while (0);
#endif

    return ret;
}

static TinyRet DDD_LoadServiceList(UpnpDevice *thiz, TinyXmlNode *serviceList)
{
    TinyRet ret = TINY_RET_OK;

    do
    {
        UpnpServiceList * svcList = UpnpDevice_GetServiceList(thiz);
        uint32_t count = 0;
        uint32_t i = 0;

        count = TinyXmlNode_GetChildren(serviceList);
        if (count == 0)
        {
            ret = TINY_RET_E_XML_INVALID;
            break;
        }

        for (i = 0; i < count; i++)
        {
            UpnpService *service = NULL;
            TinyXmlNode *child = NULL;
            const char *service_type = NULL;
            const char *service_id = NULL;
            const char *ctrl_url = NULL;
            const char *event_sub_url = NULL;
            const char *scpd_url = NULL;

            child = TinyXmlNode_GetChildAt(serviceList, i);
            if (!str_equal(TinyXmlNode_GetName(child), DDD_SERVICE, true))
            {
                continue;
            }

            service_type = TinyXmlNode_GetChildContent(child, DDD_SERVICE_TYPE);
            service_id = TinyXmlNode_GetChildContent(child, DDD_SERVICE_ID);
            ctrl_url = TinyXmlNode_GetChildContent(child, DDD_SERVICE_CONTROLURL);
            event_sub_url = TinyXmlNode_GetChildContent(child, DDD_SERVICE_EVENTSUBURL);
            scpd_url = TinyXmlNode_GetChildContent(child, DDD_SERVICE_SCPDURL);
            if (service_type == NULL || service_id == NULL || ctrl_url == NULL || event_sub_url == NULL || scpd_url == NULL)
            {
                continue;
            }
            
            service = UpnpService_New();
            if (service == NULL)
            {
                ret = TINY_RET_E_OUT_OF_MEMORY;
                break;
            }
            else
            {
                UpnpService_SetServiceType(service, service_type);
                UpnpService_SetServiceId(service, service_id);
                UpnpService_SetControlURL(service, ctrl_url);
                UpnpService_SetEventSubURL(service, event_sub_url);
                UpnpService_SetSCPDURL(service, scpd_url);
                UpnpService_SetParentDevice(service, thiz);
            }

            UpnpServiceList_AddService(svcList, service);
        }
    } while (0);

    return ret;
}

static TinyRet UpnpDeviceParser_SetURLBase(UpnpDevice *thiz, const char *deviceUrl)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(deviceUrl, TINY_RET_E_ARG_NULL);

    do
    {
        char url_base[TINY_URL_LEN];
        char ip[TINY_IP_LEN];
        uint16_t port = 0;
        char uri[512];

        memset(ip, 0, TINY_IP_LEN);
        memset(uri, 0, 512);

        ret = url_split(deviceUrl, ip, TINY_IP_LEN, &port, uri, 512);
        if (RET_FAILED(ret))
        {
            break;
        }

        tiny_snprintf(url_base, TINY_URL_LEN, "http://%s:%d", ip, port);

        UpnpDevice_SetURLBase(thiz, url_base);
    } while (0);

    return ret;
}

uint32_t UpnpDeviceParser_ToXml(UpnpDevice *device, char *xml, uint32_t len)
{
    char *p = xml;
    uint32_t unused = len;

    RETURN_VAL_IF_FAIL(device, 0);
    RETURN_VAL_IF_FAIL(xml, 0);

    do
    {
        UpnpServiceList *list = UpnpDevice_GetServiceList(device);
        uint32_t count = UpnpServiceList_GetSize(list);
        uint32_t i = 0;

        tiny_snprintf(p, unused,
            "<?xml version=\"1.0\"?>"
            "<root xmlns=\"urn:schemas-upnp-org:device-1-0\">"
            "<specVersion>"
            "<major>1</major>"
            "<minor>0</minor>"
            "</specVersion>"
            "<device>"
            "<deviceType>%s</deviceType>"
            "<friendlyName>%s</friendlyName>"
            "<manufacturer>%s</manufacturer>"
            "<manufacturerURL>%s</manufacturerURL>"
            "<modelDescription></modelDescription>"
            "<modelName>%s</modelName>"
            "<modelNumber>%s</modelNumber>"
            "<modelURL>%s</modelURL>"
            "<UDN>%s</UDN>"
            "<iconList>"
            "</iconList>"
            "<servieList>",
            UpnpDevice_GetDeviceType(device),
            UpnpDevice_GetFriendlyName(device),
            UpnpDevice_GetManufacturer(device),
            UpnpDevice_GetManufacturerURL(device),
            UpnpDevice_GetModelName(device),
            UpnpDevice_GetModelNumber(device),
            UpnpDevice_GetModelURL(device),
            UpnpDevice_GetDeviceId(device)
            );

        p = xml + strlen(p);
        unused = len - (p - xml);

        for (i = 0; i < count; ++i)
        {
            char ss[1024 * 3];
            UpnpService *service = UpnpServiceList_GetServiceAt(list, i);
            const char *scpd = UpnpService_GetSCPDURL(service);
            const char *ctrl = UpnpService_GetControlURL(service);
            const char *event = UpnpService_GetEventSubURL(service);
            const char *id = UpnpService_GetServiceId(service);
            const char *type = UpnpService_GetServiceType(service);

            memset(ss, 0, 1024 * 3);
            tiny_snprintf(ss, 1024 * 3,
                "<service>"
                "<serviceType>%s</serviceType>"
                "<serviceId>%s</serviceId>"
                "<SCPDURL>%s</SCPDURL>"
                "<controlURL>%s</controlURL>"
                "<eventSubURL>%s</eventSubURL>"
                "</service>",
                type,
                id,
                scpd,
                ctrl,
                event);

            tiny_snprintf(p, unused, ss);

            p += strlen(ss);
            unused -= strlen(ss);
        }

        tiny_snprintf(p, unused,
            "</servieList>"
            "</device>"
            "</root>"
            );
    } while (0);

    return strlen(xml);
}