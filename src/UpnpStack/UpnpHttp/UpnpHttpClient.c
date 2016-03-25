/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   UpnpHttpClient.c
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#include "UpnpHttpClient.h"
#include "upnp_define.h"
#include "HttpClient.h"
#include "tiny_log.h"
#include "tiny_memory.h"
#include "message/ActionRequest.h"
#include "message/ActionResponse.h"
#include "message/ServiceSubRequest.h"
#include "message/ServiceSubResponse.h"
#include "message/ServiceUnsubRequest.h"
#include "message/ServiceUnsubResponse.h"

#define TAG     "UpnpHttpClient"


UpnpHttpClient * UpnpHttpClient_New(void)
{
    UpnpHttpClient *thiz = NULL;

    do
    {
        TinyRet ret = TINY_RET_OK;

        thiz = (UpnpHttpClient *)tiny_malloc(sizeof(UpnpHttpClient));
        if (thiz == NULL)
        {
            break;
        }

        ret = UpnpHttpClient_Construct(thiz);
        if (RET_FAILED(ret))
        {
            UpnpHttpClient_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

TinyRet UpnpHttpClient_Construct(UpnpHttpClient *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(UpnpHttpClient));

        thiz->client = HttpClient_New();
        if (thiz->client == NULL)
        {
            ret = TINY_RET_E_NEW;
            break;
        }
    } while (0);

    return ret;
}

void UpnpHttpClient_Dispose(UpnpHttpClient *thiz)
{
    RETURN_IF_FAIL(thiz);

    HttpClient_Delete(thiz->client);
}

void UpnpHttpClient_Delete(UpnpHttpClient *thiz)
{
    RETURN_IF_FAIL(thiz);

    UpnpHttpClient_Dispose(thiz);
    tiny_free(thiz);
}

TinyRet UpnpHttpClient_Post(UpnpHttpClient *thiz, UpnpAction *action, UpnpError *error, uint32_t timeout)
{
    LOG_TIME_BEGIN(TAG, UpnpHttpClient_Post);

    TinyRet ret = TINY_RET_OK;
    HttpMessage *request = NULL;
    HttpMessage *response = NULL;
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
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

        /**
         * UpnpAction -> HttpReqeust
         */
        ret = ActionToRequest(action, request);
        if (RET_FAILED(ret))
        {
            break;
        }

        //
        // 2016.3.24
        // 每次HTTP请求都重新开始连接，暂时先这么做。
        // 理想的做法是建立一个连接池，能复用长连接的尽量复用，加速方法调用的速度。
        //
        HttpClient_Shutdown(thiz->client);

        ret = HttpClient_Execute(thiz->client, request, response, UPNP_TIMEOUT);
        if (RET_FAILED(ret))
        {
            LOG_D(TAG, "HttpClient_Execute failed: %s", tiny_ret_to_str(ret));
            break;
        }

        /**
         * HttpResponse -> UpnpAction
         */
        ret = ActionFromResponse(action, error, response);
    } while (0);

    if (request != NULL)
    {
        HttpMessage_Delete(request);
    }

    if (response != NULL)
    {
        HttpMessage_Delete(response);
    }

    LOG_TIME_END(TAG, UpnpHttpClient_Post);

    return ret;
}

TinyRet UpnpHttpClient_Notify(UpnpHttpClient *thiz, HttpMessage *request, HttpMessage *response, uint32_t timeout)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    return TINY_RET_OK;
}

TinyRet UpnpHttpClient_Subscribe(UpnpHttpClient *thiz, UpnpSubscription *subscription, UpnpError *error, uint32_t timeout)
{
    LOG_TIME_BEGIN(TAG, UpnpSubscriber_Subscribe);

    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(subscription, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(error, TINY_RET_E_ARG_NULL);

    do
    {
        HttpMessage request;
        HttpMessage response;

        ret = HttpMessage_Construct(&request);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = HttpMessage_Construct(&response);
        if (RET_FAILED(ret))
        {
            HttpMessage_Dispose(&request);
            break;
        }

        do
        {
            /**
            * UpnpSubscription -> HttpReqeust
            */
            ret = ServiceSubToRequest(subscription, &request);
            if (RET_FAILED(ret))
            {
                break;
            }

            //
            // 2016.3.24
            // 每次HTTP请求都重新开始连接，暂时先这么做。
            // 理想的做法是建立一个连接池，能复用长连接的尽量复用，加速方法调用的速度。
            //
            HttpClient_Shutdown(thiz->client);

            ret = HttpClient_Execute(thiz->client, &request, &response, UPNP_TIMEOUT);
            if (RET_FAILED(ret))
            {
                LOG_D(TAG, "HttpClient_Execute failed: %s", tiny_ret_to_str(ret));
                break;
            }

            if (HttpMessage_GetStatusCode(&response) != HTTP_STATUS_OK)
            {
                LOG_D(TAG, "HttpClient_Execute failed: %d %s",
                    HttpMessage_GetStatusCode(&response),
                    HttpMessage_GetStatus(&response));
                ret = TINY_RET_E_UPNP_INVOKE_FAILED;
                break;
            }

            /**
            * HttpResponse -> UpnpSubscription
            */
            ret = ServiceSubFromResponse(subscription, error, &response);
        } while (0);

        HttpMessage_Dispose(&request);
        HttpMessage_Dispose(&response);
    } while (0);

    LOG_TIME_END(TAG, UpnpSubscriber_Subscribe);

    return ret;
}

TinyRet UpnpHttpClient_Unsubscribe(UpnpHttpClient *thiz, UpnpSubscription *subscription, UpnpError *error, uint32_t timeout)
{
    LOG_TIME_BEGIN(TAG, UpnpHttpClient_Unsubscribe);

    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(subscription, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(error, TINY_RET_E_ARG_NULL);

    do
    {
        HttpMessage request;
        HttpMessage response;

        ret = HttpMessage_Construct(&request);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = HttpMessage_Construct(&response);
        if (RET_FAILED(ret))
        {
            HttpMessage_Dispose(&request);
            break;
        }

        do
        {
            /**
            * UpnpSubscription -> HttpReqeust
            */
            ret = ServiceUnsubToRequest(subscription, &request);
            if (RET_FAILED(ret))
            {
                break;
            }

            //
            // 2016.3.24
            // 每次HTTP请求都重新开始连接，暂时先这么做。
            // 理想的做法是建立一个连接池，能复用长连接的尽量复用，加速方法调用的速度。
            //
            HttpClient_Shutdown(thiz->client);

            ret = HttpClient_Execute(thiz->client, &request, &response, UPNP_TIMEOUT);
            if (RET_FAILED(ret))
            {
                LOG_D(TAG, "HttpClient_Execute failed: %s", tiny_ret_to_str(ret));
                break;
            }

            if (HttpMessage_GetStatusCode(&response) != HTTP_STATUS_OK)
            {
                LOG_D(TAG, "HttpClient_Execute failed: %d %s",
                    HttpMessage_GetStatusCode(&response),
                    HttpMessage_GetStatus(&response));
                ret = TINY_RET_E_UPNP_INVOKE_FAILED;
                break;
            }

            /**
            * HttpResponse -> UpnpSubscription
            */
            ret = ServiceUnsubFromResponse(subscription, error, &response);
        } while (0);

        HttpMessage_Dispose(&request);
        HttpMessage_Dispose(&response);
    } while (0);

    LOG_TIME_END(TAG, UpnpHttpClient_Unsubscribe);

    return ret;
}