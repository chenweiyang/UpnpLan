/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   UpnpSoapClient.h
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#include "UpnpSoapClient.h"
#include "HttpClient.h"
#include "UpnpSoapMessageDefinition.h"
#include "ct_memory.h"
#include "ct_log.h"

#define TAG     "UpnpActionExecutor"

static CtRet UpnpSoapClient_Construct(UpnpSoapClient *thiz);
static void UpnpSoapClient_Dispose(UpnpSoapClient *thiz);

static CtRet SoapRequest_ToHttpRequest(UpnpSoapMessage *soap, HttpMessage *request);

struct _UpnpSoapClient
{
    HttpClient  * client;
};

UpnpSoapClient * UpnpSoapClient_New(void)
{
    UpnpSoapClient *thiz = NULL;

    do
    {
        CtRet ret = CT_RET_OK;

        thiz = (UpnpSoapClient *)ct_malloc(sizeof(UpnpSoapClient));
        if (thiz == NULL)
        {
            break;
        }

        ret = UpnpSoapClient_Construct(thiz);
        if (RET_FAILED(ret))
        {
            UpnpSoapClient_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

void UpnpSoapClient_Delete(UpnpSoapClient *thiz)
{
    RETURN_IF_FAIL(thiz);

    UpnpSoapClient_Dispose(thiz);
    ct_free(thiz);
}

static CtRet UpnpSoapClient_Construct(UpnpSoapClient *thiz)
{
    CtRet ret = CT_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, CT_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(UpnpSoapClient));

        thiz->client = HttpClient_New();
        if (thiz->client == NULL)
        {
            ret = CT_RET_E_NEW;
            break;
        }
    } while (0);

    return ret;
}

static void UpnpSoapClient_Dispose(UpnpSoapClient *thiz)
{
    RETURN_IF_FAIL(thiz);

    HttpClient_Delete(thiz->client);
}

CtRet UpnpSoapClient_Invoke(UpnpSoapClient *thiz,
    UpnpSoapMessage *request,
    UpnpSoapMessage *response,
    int timeout)
{
    LOG_TIME_BEGIN(TAG, UpnpActionExecutor_Invoke);

    CtRet ret = CT_RET_OK;
    HttpMessage *httpRequest = NULL;
    HttpMessage *httpResponse = NULL;

    RETURN_VAL_IF_FAIL(thiz, CT_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(request, CT_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(response, CT_RET_E_ARG_NULL);

    do
    {
        httpRequest = HttpMessage_New();
        if (httpRequest == NULL)
        {
            ret = CT_RET_E_NEW;
            break;
        }
        
        httpResponse = HttpMessage_New();
        if (httpResponse == NULL)
        {
            ret = CT_RET_E_NEW;
            break;
        }

        ret = SoapRequest_ToHttpRequest(request, httpRequest);
        if (RET_FAILED(ret))
        {
            LOG_D(TAG, "SoapRequest_ToHttpRequest failed: %s", ct_ret_to_str(ret));
            break;
        }

        ret = HttpClient_Execute(thiz->client, httpRequest, httpResponse, UPNP_TIMEOUT);
        if (RET_FAILED(ret))
        {
            LOG_D(TAG, "HttpClient_Execute failed: %s", ct_ret_to_str(ret));
            break;
        }

        if (HttpMessage_GetStatusCode(httpResponse) != HTTP_STATUS_OK)
        {
            LOG_D(TAG, "HttpClient_Execute failed: %s", ct_ret_to_str(ret));
            ret = CT_RET_E_UPNP_INVOKE_FAILED;
            break;
        }

        if (HttpMessage_GetContentSize(httpResponse) > 0)
        {
            ret = CT_RET_E_UPNP_INVOKE_FAILED;
            break;
        }

        ret = UpnpSoapMessage_Parse(response,
            HttpMessage_GetContentObject(httpResponse),
            HttpMessage_GetContentSize(httpResponse));
        if (RET_FAILED(ret))
        {
            LOG_D(TAG, "UpnpSoapMessage_Parse failed: %s", ct_ret_to_str(ret));
            break;
        }

        HttpClient_Shutdown(thiz->client);
    } while (0);

    if (httpRequest != NULL)
    {
        HttpMessage_Delete(httpRequest);
    }

    if (httpResponse != NULL)
    {
        HttpMessage_Delete(httpResponse);
    }

    LOG_TIME_END(TAG, UpnpActionExecutor_Invoke);

    return ret;
}

static CtRet SoapRequest_ToHttpRequest(UpnpSoapMessage *soap, HttpMessage *request)
{
    CtRet ret = CT_RET_OK;

    do
    {
        char soap_action[256];
        char *data = NULL;
        uint32_t size = UPNP_SOAP_LEN;
        const char * serverUrl = UpnpSoapMessage_GetPropertyValue(soap, UPNP_SOAP_ServerURL);
        const char * actionName = UpnpSoapMessage_GetPropertyValue(soap, UPNP_SOAP_ActionName);
        const char * actionXmlns = UpnpSoapMessage_GetPropertyValue(soap, UPNP_SOAP_ActionXmlns);

        ct_snprintf(soap_action, 256, "\"%s#%s\"", actionXmlns, actionName);

        data = (char *)ct_malloc(size);
        if (data == NULL)
        {
            ret = CT_RET_E_OUT_OF_MEMORY;
            break;
        }

        memset(data, 0, size);
        ret = UpnpSoapMessage_ToString(soap, data, size);
        if (RET_FAILED(ret))
        {
            ct_free(data);
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

        ct_free(data);
    } while (0);

    return ret;
}