/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   SoapClient.h
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#include "SoapClient.h"
#include "HttpClient.h"
#include "SoapDefinition.h"
#include "tiny_memory.h"
#include "tiny_log.h"

#define TAG     "SoapClient"

static TinyRet SoapClient_Construct(SoapClient *thiz);
static void SoapClient_Dispose(SoapClient *thiz);

static TinyRet SoapRequest_ToHttpRequest(SoapMessage *soap, HttpMessage *request);

struct _SoapClient
{
    HttpClient  * client;
};

SoapClient * UpnpSoapClient_New(void)
{
    SoapClient *thiz = NULL;

    do
    {
        TinyRet ret = TINY_RET_OK;

        thiz = (SoapClient *)tiny_malloc(sizeof(SoapClient));
        if (thiz == NULL)
        {
            break;
        }

        ret = SoapClient_Construct(thiz);
        if (RET_FAILED(ret))
        {
            SoapClient_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

void SoapClient_Delete(SoapClient *thiz)
{
    RETURN_IF_FAIL(thiz);

    SoapClient_Dispose(thiz);
    tiny_free(thiz);
}

static TinyRet SoapClient_Construct(SoapClient *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(SoapClient));

        thiz->client = HttpClient_New();
        if (thiz->client == NULL)
        {
            ret = TINY_RET_E_NEW;
            break;
        }
    } while (0);

    return ret;
}

static void SoapClient_Dispose(SoapClient *thiz)
{
    RETURN_IF_FAIL(thiz);

    HttpClient_Delete(thiz->client);
}

TinyRet SoapClient_Invoke(SoapClient *thiz, SoapMessage *request, SoapMessage *response, int timeout)
{
    LOG_TIME_BEGIN(TAG, SoapClient_Invoke);

    TinyRet ret = TINY_RET_OK;
    HttpMessage *httpRequest = NULL;
    HttpMessage *httpResponse = NULL;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(request, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(response, TINY_RET_E_ARG_NULL);

    do
    {
        httpRequest = HttpMessage_New();
        if (httpRequest == NULL)
        {
            ret = TINY_RET_E_NEW;
            break;
        }
        
        httpResponse = HttpMessage_New();
        if (httpResponse == NULL)
        {
            ret = TINY_RET_E_NEW;
            break;
        }

        ret = SoapRequest_ToHttpRequest(request, httpRequest);
        if (RET_FAILED(ret))
        {
            LOG_D(TAG, "SoapRequest_ToHttpRequest failed: %s", TINY_RET_to_str(ret));
            break;
        }

        ret = HttpClient_Execute(thiz->client, httpRequest, httpResponse, UPNP_TIMEOUT);
        if (RET_FAILED(ret))
        {
            LOG_D(TAG, "HttpClient_Execute failed: %s", TINY_RET_to_str(ret));
            break;
        }

        if (HttpMessage_GetStatusCode(httpResponse) != HTTP_STATUS_OK)
        {
            LOG_D(TAG, "HttpClient_Execute failed: %s", TINY_RET_to_str(ret));
            ret = TINY_RET_E_UPNP_INVOKE_FAILED;
            break;
        }

        if (HttpMessage_GetContentSize(httpResponse) > 0)
        {
            ret = TINY_RET_E_UPNP_INVOKE_FAILED;
            break;
        }

        ret = SoapMessage_Parse(response,
            HttpMessage_GetContentObject(httpResponse),
            HttpMessage_GetContentSize(httpResponse));
        if (RET_FAILED(ret))
        {
            LOG_D(TAG, "SoapMessage_Parse failed: %s", TINY_RET_to_str(ret));
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

static TinyRet SoapRequest_ToHttpRequest(SoapMessage *soap, HttpMessage *request)
{
    TinyRet ret = TINY_RET_OK;

    do
    {
        char soap_action[256];
        char *data = NULL;
        uint32_t size = UPNP_SOAP_LEN;
        const char * serverUrl = SoapMessage_GetPropertyValue(soap, SOAP_ServerURL);
        const char * actionName = SoapMessage_GetPropertyValue(soap, SOAP_ActionName);
        const char * actionXmlns = SoapMessage_GetPropertyValue(soap, SOAP_ActionXmlns);

        tiny_snprintf(soap_action, 256, "\"%s#%s\"", actionXmlns, actionName);

        data = (char *)tiny_malloc(size);
        if (data == NULL)
        {
            ret = TINY_RET_E_OUT_OF_MEMORY;
            break;
        }

        memset(data, 0, size);
        ret = SoapMessage_ToString(soap, data, size);
        if (RET_FAILED(ret))
        {
            tiny_free(data);
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

        tiny_free(data);
    } while (0);

    return ret;
}