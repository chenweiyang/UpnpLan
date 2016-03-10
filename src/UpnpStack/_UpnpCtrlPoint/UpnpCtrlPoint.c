/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   UpnpCtrlPoint.c
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#include "UpnpCtrlPoint.h"
#include "ct_log.h"
#include "ct_socket.h"
#include "ct_memory.h"
#include "UpnpObject.h"
#include "UpnpUsn.h"
#include "UpnpSsdp.h"
#include "DDD.h"
#include "SDD.h"
#include "UpnpDeviceDefinition.h"
#include "UpnpServiceDefinition.h"
#include "PropertyList.h"
#include "UpnpSoapClient.h"
#include "UpnpSoapMessageDefinition.h"
#include "UpnpMonitor.h"

#define TAG             "UpnpCtrlPoint"

static CtRet UpnpCtrlPoint_Construct(UpnpCtrlPoint *thiz);
static void UpnpCtrlPoint_Dispose(UpnpCtrlPoint *thiz);

static CtRet SoapMessageToActionResult(UpnpSoapMessage *soap, UpnpAction *action, UpnpError *error);
static CtRet ActionToSoapMessage(UpnpAction *action, UpnpSoapMessage *soap);

static bool object_filter(UpnpUsn *usn, void *ctx);
static void object_listener(UpnpObject *object, bool alive, void *ctx);

void UpnpStackInit(void)
{
    ct_socket_init();
}

struct _UpnpCtrlPoint
{
    UpnpSoapClient        * soapClient;
    UpnpDeviceListener      deviceListener;
    UpnpDeviceFilter        deviceFilter;
    void                  * ctx;
    UpnpMonitor           * monitor;
};

UpnpCtrlPoint * UpnpCtrlPoint_New(void)
{
    UpnpCtrlPoint *thiz = NULL;

    do
    {
        CtRet ret = CT_RET_OK;

        thiz = (UpnpCtrlPoint *)ct_malloc(sizeof(UpnpCtrlPoint));
        if (thiz == NULL)
        {
            break;
        }

        ret = UpnpCtrlPoint_Construct(thiz);
        if (RET_FAILED(ret))
        {
            UpnpCtrlPoint_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

void UpnpCtrlPoint_Delete(UpnpCtrlPoint *thiz)
{
    RETURN_IF_FAIL(thiz);

    UpnpCtrlPoint_Dispose(thiz);
    ct_free(thiz);
}

static CtRet UpnpCtrlPoint_Construct(UpnpCtrlPoint *thiz)
{
    CtRet ret = CT_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, CT_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(UpnpCtrlPoint));

        thiz->soapClient = UpnpSoapClient_New();
        if (thiz->soapClient == NULL)
        {
            ret = CT_RET_E_NEW;
            break;
        }

        thiz->monitor = UpnpMonitor_New();
        if (thiz->monitor == NULL)
        {
            ret = CT_RET_E_NEW;
            break;
        }

    } while (0);

    return ret;
}

static void UpnpCtrlPoint_Dispose(UpnpCtrlPoint *thiz)
{
    RETURN_IF_FAIL(thiz);

    UpnpSsdp_Stop(UpnpSsdp_GetInstance());
    UpnpSoapClient_Delete(thiz->soapClient);
    UpnpMonitor_Delete(thiz->monitor);
}

CtRet UpnpCtrlPoint_SetListener(UpnpCtrlPoint *thiz, UpnpDeviceListener listener, UpnpDeviceFilter filter, void *ctx)
{
    RETURN_VAL_IF_FAIL(thiz, CT_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(listener, CT_RET_E_ARG_NULL);

    thiz->deviceListener = listener;
    thiz->deviceFilter = filter;
    thiz->ctx = ctx;

    return UpnpSsdp_SetListener(UpnpSsdp_GetInstance(), object_listener, object_filter, thiz);
}

CtRet UpnpCtrlPoint_Start(UpnpCtrlPoint *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, CT_RET_E_ARG_NULL);

    UpnpSsdp_SetUuidMode(UpnpSsdp_GetInstance(), true);

    return UpnpSsdp_Start(UpnpSsdp_GetInstance());
}

CtRet UpnpCtrlPoint_Stop(UpnpCtrlPoint *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, CT_RET_E_ARG_NULL);

    return UpnpSsdp_Stop(UpnpSsdp_GetInstance());
}

CtRet UpnpCtrlPoint_Search(UpnpCtrlPoint *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, CT_RET_E_ARG_NULL);

    return UpnpSsdp_Search(UpnpSsdp_GetInstance());
}

UpnpDevice * UpnpCtrlPoint_LoadDeviceDescription(UpnpDeviceSummary *summary)
{
    UpnpDevice * device = NULL;

    do
    {
        uint32_t i = 0;
        uint32_t size = 0;
        CtRet ret = CT_RET_OK;
        UpnpServiceList *services = NULL;
        const char *urlbase = NULL;

        device = UpnpDevice_New();
        if (device == NULL)
        {
            LOG_D(TAG, "UpnpDevice_New failed");
            break;
        }

        ret = DDD_Parse(summary->deviceUrl, device, UPNP_TIMEOUT);
        if (RET_FAILED(ret))
        {
            LOG_D(TAG, "DDD_Load failed");
            UpnpDevice_Delete(device);
            device = NULL;
            break;
        }

        urlbase = UpnpDevice_GetPropertyValue(device, UPNP_DEVICE_URLBase);
        if (urlbase == NULL)
        {
            LOG_D(TAG, "<%s> not found", UPNP_DEVICE_URLBase);
            UpnpDevice_Delete(device);
            device = NULL;
            break;
        }

        services = UpnpDevice_GetServiceList(device);
        size = UpnpServiceList_GetSize(services);
        for (i = 0; i < size; ++i)
        {
            char url[CT_URL_LEN];
            UpnpService *service = UpnpServiceList_GetServiceAt(services, i);
            const char *scpdUrl = UpnpService_GetPropertyValue(service, UPNP_SERVICE_SCPDURL);
            if (scpdUrl == NULL)
            {
                LOG_D(TAG, "Get <%s> failed", UPNP_SERVICE_SCPDURL);
                ret = CT_RET_E_NOT_FOUND;
                break;
            }

            ct_snprintf(url, CT_URL_LEN, "%s%s", urlbase, scpdUrl);

            ret = SDD_Parse(url, service, UPNP_TIMEOUT);
            if (RET_FAILED(ret))
            {
                break;
            }
        }

        if (RET_FAILED(ret))
        {
            UpnpDevice_Delete(device);
            device = NULL;
            break;
        }
    } while (0);

    return device;
}

CtRet UpnpCtrlPoint_Invoke(UpnpCtrlPoint *thiz, UpnpAction *action, UpnpError *error)
{
    CtRet ret = CT_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, CT_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(action, CT_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(error, CT_RET_E_ARG_NULL);

    do
    {
        UpnpSoapMessage * request = UpnpSoapMessage_New();
        if (request == NULL)
        {
            ret = CT_RET_E_NEW;
            break;
        }

        do
        {
            ret = ActionToSoapMessage(action, request);
            if (RET_FAILED(ret))
            {
                break;
            }

            UpnpSoapMessage *response = UpnpSoapMessage_New();
            if (response == NULL)
            {
                ret = CT_RET_E_NEW;
                break;
            }

            ret = UpnpSoapClient_Invoke(thiz->soapClient, request, response, UPNP_TIMEOUT);
            if (RET_SUCCEEDED(ret))
            {
                ret = SoapMessageToActionResult(response, action, error);
            }

            UpnpSoapMessage_Delete(response);
        } while (0);

        UpnpSoapMessage_Delete(request);
    } while (0);

    return ret;
}

CtRet UpnpCtrlPoint_Subscribe(UpnpCtrlPoint *thiz, UpnpSubscription *subscription, UpnpError *error)
{
    RETURN_VAL_IF_FAIL(thiz, CT_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(subscription, CT_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(error, CT_RET_E_ARG_NULL);

    return UpnpMonitor_Subscribe(thiz->monitor, subscription, error);
}

CtRet UpnpCtrlPoint_Unsubscribe(UpnpCtrlPoint *thiz, UpnpError *error)
{
    RETURN_VAL_IF_FAIL(thiz, CT_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(error, CT_RET_E_ARG_NULL);

    return UpnpMonitor_Unsubscribe(thiz->monitor, error);
}

static CtRet ActionToSoapMessage(UpnpAction *action, UpnpSoapMessage *soap)
{
    CtRet ret = CT_RET_OK;

    RETURN_VAL_IF_FAIL(action, CT_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(soap, CT_RET_E_ARG_NULL);

    do
    {
        PropertyList *actionArguments = NULL;
        PropertyList *soapArguments = NULL;
        UpnpDevice *device = NULL;
        UpnpService *service = NULL;
        const char *ctrlUrl = NULL;
        const char *urlBase = NULL;
        char url[CT_URL_LEN];

        memset(url, 0, CT_URL_LEN);

        service = (UpnpService *)UpnpAction_GetParentService(action);
        if (service == NULL)
        {
            ret = CT_RET_E_UPNP_SERVICE_NOT_FOUND;
            break;
        }

        device = (UpnpDevice *)UpnpService_GetParentDevice(service);
        if (service == NULL)
        {
            ret = CT_RET_E_UPNP_DEVICE_NOT_FOUND;
            break;
        }

        ctrlUrl = UpnpService_GetPropertyValue(service, UPNP_SERVICE_ControlURL);
        urlBase = UpnpDevice_GetPropertyValue(device, UPNP_DEVICE_URLBase);

        ct_snprintf(url, CT_URL_LEN, "%s%s", urlBase, ctrlUrl);

        ret = UpnpSoapMessage_SetPropertyValue(soap, UPNP_SOAP_ServerURL, url);
        if (RET_FAILED(ret))
        {
            break;
        }

        soapArguments = UpnpSoapMessage_GetArgumentList(soap);
        actionArguments = UpnpAction_GetArgumentList(action);
        PropertyList_Copy(soapArguments, actionArguments);
    } while (0);

    return ret;
}

static CtRet SoapMessageToActionResult(UpnpSoapMessage *soap, UpnpAction *action, UpnpError *error)
{
    CtRet ret = CT_RET_OK;

    PropertyList *fault = UpnpSoapMessage_GetFault(soap);
    if (fault != NULL)
    {
        const char * errorCode = UpnpSoapMessage_GetFaultValue(soap, UPNP_SOAP_FAULT_ErrorCode);
        const char * errorDesc = UpnpSoapMessage_GetFaultValue(soap, UPNP_SOAP_FAULT_ErrorDescription);
        error->code = atoi(errorCode);
        strncpy(error->description, errorDesc, UPNP_ERR_DESCRIPTION_LEN);
        ret = CT_RET_E_UPNP_INVOKE_FAILED;
    }
    else
    {
        uint32_t i = 0;
        uint32_t count = 0;

        PropertyList *actionResults = UpnpAction_GetResultList(action);
        PropertyList *soapResults = UpnpSoapMessage_GetArgumentList(soap);

        count = PropertyList_GetSize(actionResults);
        for (i = 0; i < count; ++i)
        {
            Property *p = PropertyList_GetPropertyAt(actionResults, i);
            const char *name = p->definition.name;
            ObjectType *type = &p->definition.type;

            Property *result = PropertyList_GetProperty(soapResults, name);
            if (result->definition.type.clazzType != type->clazzType)
            {
                ret = CT_RET_E_UPNP_INVOKE_FAILED;
                break;
            }
        }
    }

    return ret;
}

static bool object_filter(UpnpUsn *usn, void *ctx)
{
    bool result = false;

    //LOG_D(TAG, "object_filter");

    do
    {
        UpnpCtrlPoint *thiz = (UpnpCtrlPoint *)ctx;
        UpnpUriType type = UPNP_URI_UNDEFINED;

        type = usn->uri.type;
        if (type != UPNP_URI_UPNP_DEVICE && type != UPNP_URI_NON_UPNP_DEVICE)
        {
            //LOG_D(TAG, "%s is not DEVICE", usn->string);
            break;
        }

        result = (thiz->deviceFilter == NULL) ? true : thiz->deviceFilter(usn->uri.domain_name, usn->uri.device_type, thiz->ctx);
    } while (0);

    return result;
}

static void object_listener(UpnpObject *object, bool alive, void *ctx)
{
    do
    {
        UpnpCtrlPoint *thiz = (UpnpCtrlPoint *)ctx;
        UpnpUri *nt = NULL;

        if (thiz->deviceListener == NULL)
        {
            break;
        }

        nt = UpnpObject_GetNt(object);
        if (nt->type == UPNP_URI_UPNP_DEVICE || nt->type == UPNP_URI_NON_UPNP_DEVICE)
        {
            UpnpDeviceSummary device;
            memset(&device, 0, sizeof(UpnpDeviceSummary));
            strncpy(device.deviceIp, UpnpObject_GetIp(object), CT_IP_LEN);
            strncpy(device.deviceId, UpnpObject_GetUsn(object), UPNP_USN_LEN);
            strncpy(device.deviceUrl, UpnpObject_GetLocation(object), CT_URL_LEN);
            strncpy(device.domainName, nt->domain_name, UPNP_DOMAIN_NAME_LEN);
            strncpy(device.deviceType, nt->device_type, UPNP_TYPE_LEN);
            device.deviceVersion = atoi(nt->version);
            strncpy(device.upnpStackInfo, UpnpObject_GetStackInfo(object), UPNP_STACK_INFO_LEN);

            thiz->deviceListener(&device, alive, thiz->ctx);
        }
    } while (0);
}
