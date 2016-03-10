/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   HttpClient.h
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#include "HttpClient.h"
#include "TcpClient.h"
#include "tiny_memory.h"
#include "tiny_log.h"
#include "TcpClient.h"

#define TAG             "HttpClient"

struct _HttpClient
{
    TcpClient     client;
};

HttpClient * HttpClient_New(void)
{
    HttpClient *thiz = NULL;

    do
    {
        TinyRet ret = TINY_RET_OK;

        thiz = (HttpClient *)tiny_malloc(sizeof(HttpClient));
        if (thiz == NULL)
        {
            break;
        }

        ret = HttpClient_Construct(thiz);
        if (RET_FAILED(ret))
        {
            HttpClient_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

TinyRet HttpClient_Construct(HttpClient *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(HttpClient));

        ret = TcpClient_Construct(&thiz->client);
        if (RET_FAILED(ret))
        {
            break;
        }
    } while (0);

    return ret;
}

TinyRet HttpClient_Dispose(HttpClient *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    TcpClient_Dispose(&thiz->client);

    return TINY_RET_OK;
}

void HttpClient_Delete(HttpClient *thiz)
{
    RETURN_IF_FAIL(thiz);

    HttpClient_Dispose(thiz);
    tiny_free(thiz);
}

TinyRet HttpClient_Execute(HttpClient *thiz, HttpMessage *request, HttpMessage *response, uint32_t timeout)
{
    LOG_TIME_BEGIN(TAG, HttpClient_Execute);
    TinyRet ret = TINY_RET_OK;
    char *bytes = NULL;
    uint32_t size = 0;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(request, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(response, TINY_RET_E_ARG_NULL);

    do
    {
        if (HttpMessage_GetType(request) != HTTP_REQUEST)
        {
            break;
        }

        ret = TcpClient_Connect(&thiz->client, 
            HttpMessage_GetIp(request),
            HttpMessage_GetPort(request),
            timeout);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = HttpMessage_ToBytes(request, &bytes, &size);
        if (RET_FAILED(ret))
        {
            break;
        }

    #if (HTTP_DEBUG)
        printf("%s", bytes);
    #endif

        ret = TcpClient_Send(&thiz->client, bytes, size, timeout);
        if (RET_FAILED(ret))
        {
            break;
        }

        tiny_free(bytes);
        bytes = NULL;
        size = 0;

        ret = TcpClient_Recv(&thiz->client, &bytes, &size, timeout);
        if (RET_FAILED(ret))
        {
            break;
        }

    #if (HTTP_DEBUG)
        printf("%s", bytes);
    #endif

        ret = HttpMessage_Parse(response, bytes, size);
        if (RET_FAILED(ret))
        {
            break;
        }

        if (HttpMessage_GetContentSize(response) == 0)
        {
            break;
        }

        if (HttpMessage_IsContentFull(response))
        {
            break;
        }

        while (1)
        {
            tiny_free(bytes);
            bytes = NULL;
            size = 0;

            ret = TcpClient_Recv(&thiz->client, &bytes, &size, timeout);
            if (RET_FAILED(ret))
            {
                break;
            }

        #if (HTTP_DEBUG)
            printf("%s", bytes);
        #endif

            ret = HttpMessage_AddContentObject(response, bytes, size);
            if (RET_FAILED(ret))
            {
                break;
            }

            if (HttpMessage_IsContentFull(response))
            {
                break;
            }
        }
    } while (0);

    if (bytes != NULL)
    {
        tiny_free(bytes);
    }

    LOG_TIME_END(TAG, HttpClient_Execute);

    return ret;
}

bool HttpClient_IsConnected(HttpClient *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, false);

    return (TcpClient_GetStatus(&thiz->client) == TCP_CLIENT_CONNECTED ? true : false);
}

TinyRet HttpClient_Shutdown(HttpClient *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, false);
    
    do
    {
        if (TcpClient_GetStatus(&thiz->client) == TCP_CLIENT_DISCONNECT)
        {
            ret = TINY_RET_E_SOCKET_DISCONNECTED;
            break;
        }

        ret = TcpClient_Disconnect(&thiz->client);
        if (RET_FAILED(ret))
        {
            break;
        }
    } while (0);

    return ret;
}
