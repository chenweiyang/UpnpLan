/*
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-7-9
 *
 * @file   AsyncHttpClient.h
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#include "AsyncHttpClient.h"
#include "HttpClient.h"
#include "TinyThread.h"
#include "tiny_memory.h"

#define TAG         "AsyncHttpClient"

static void thread_loop(void *param);

struct _AsyncHttpClient
{
    bool                    is_running;
    HttpClient          * client;
    TinyThread              * thread;
    HttpMessage         * request;
    uint32_t                timeout;
    HttpClientListener    listener;
    void                  * ctx;
};

AsyncHttpClient * AsyncHttpClient_New(void)
{
    AsyncHttpClient *thiz = NULL;

    do
    {
        TinyRet ret = TINY_RET_OK;

        thiz = (AsyncHttpClient *)tiny_malloc(sizeof(AsyncHttpClient));
        if (thiz == NULL)
        {
            break;
        }

        ret = AsyncHttpClient_Construct(thiz);
        if (RET_FAILED(ret))
        {
            AsyncHttpClient_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

TinyRet AsyncHttpClient_Construct(AsyncHttpClient *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(AsyncHttpClient));

        thiz->client = HttpClient_New();
        if (thiz->client == NULL)
        {
            ret = TINY_RET_E_NEW;
            break;
        }

        thiz->thread = TinyThread_New();
        if (thiz->thread == NULL)
        {
            ret = TINY_RET_E_NEW;
            break;
        }

        ret = TinyThread_Initialize(thiz->thread, thread_loop, thiz, "AsyncHttpClient");
        if (RET_FAILED(ret))
        {
            break;
        }

        thiz->request = HttpMessage_New();
        if (thiz->request == NULL)
        {
            ret = TINY_RET_E_NEW;
            break;
        }
    } while (0);

    return ret;
}

TinyRet AsyncHttpClient_Dispose(AsyncHttpClient *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    TinyThread_Delete(thiz->thread);
    HttpClient_Delete(thiz->client);
    HttpMessage_Delete(thiz->request);

    return TINY_RET_OK;
}

void AsyncHttpClient_Delete(AsyncHttpClient *thiz)
{
    RETURN_IF_FAIL(thiz);

    AsyncHttpClient_Dispose(thiz);
    tiny_free(thiz);
}

TinyRet AsyncHttpClient_Execute(AsyncHttpClient *thiz, 
    HttpMessage *request,
    uint32_t timeout,
    HttpClientListener listener,
    void *ctx)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(request, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(listener, TINY_RET_E_ARG_NULL);

    do
    {
        if (thiz->is_running)
        {
            ret = TINY_RET_E_STARTED;
            break;
        }

        HttpMessage_Copy(thiz->request, request); 
        thiz->timeout = timeout;
        thiz->listener = listener;
        thiz->ctx = ctx;
        thiz->is_running = true;
        TinyThread_Start(thiz->thread);
    } while (0);

    return ret;
}

bool AsyncHttpClient_IsConnected(AsyncHttpClient *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, false);

    return HttpClient_IsConnected(thiz->client);
}

TinyRet AsyncHttpClient_Shutdown(AsyncHttpClient *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    return HttpClient_Shutdown(thiz->client);
}

static void thread_loop(void *param)
{
    TinyRet ret = TINY_RET_OK;
    AsyncHttpClient *thiz = (AsyncHttpClient *)param;
    HttpMessage *response = NULL;

    do
    {
        response = HttpMessage_New();
        if (response == NULL)
        {
            ret = TINY_RET_E_NEW;    
            break;
        }

        ret = HttpClient_Execute(thiz->client, thiz->request, response, thiz->timeout);
    } while (0);
 
    if (thiz->listener != NULL)
    {
        thiz->listener(thiz, ret, response, thiz->ctx);
    }

    if (response != NULL)
    {
        HttpMessage_Delete(response);
    }

    thiz->is_running = false;
}
