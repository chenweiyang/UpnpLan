/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   UpnpHttpConnection.c
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#include "UpnpHttpConnection.h"
#include "tiny_memory.h"
#include "tiny_log.h"
#include "HttpMessage.h"
#include "upnp_define.h"

#define TAG     "UpnpHttpConnection"

UpnpHttpConnection * UpnpHttpConnection_New(TcpConn *conn)
{
    UpnpHttpConnection *thiz = NULL;

    do
    {
        TinyRet ret = TINY_RET_OK;

        thiz = (UpnpHttpConnection *)tiny_malloc(sizeof(UpnpHttpConnection));
        if (thiz == NULL)
        {
            break;
        }

        ret = UpnpHttpConnection_Construct(thiz, conn);
        if (RET_FAILED(ret))
        {
            UpnpHttpConnection_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

TinyRet UpnpHttpConnection_Construct(UpnpHttpConnection *thiz, TcpConn *conn)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(UpnpHttpConnection));
        thiz->conn = conn;
    } while (0);

    return ret;
}

void UpnpHttpConnection_Dispose(UpnpHttpConnection *thiz)
{
    RETURN_IF_FAIL(thiz);

    thiz->conn = NULL;
}

void UpnpHttpConnection_Delete(UpnpHttpConnection *thiz)
{
    RETURN_IF_FAIL(thiz);

    UpnpHttpConnection_Dispose(thiz);
    tiny_free(thiz);
}

TinyRet UpnpHttpConnection_SendOk(UpnpHttpConnection *thiz)
{
    return UpnpHttpConnection_SendError(thiz, 200, "OK");
}

TinyRet UpnpHttpConnection_SendError(UpnpHttpConnection *thiz, int code, const char *status)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        HttpMessage response;
        char string[1024];
        uint32_t len;

        ret = HttpMessage_Construct(&response);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "HttpMessage_Construct failed");
            ret = TINY_RET_E_NEW;
            break;
        }

        do
        {
            HttpMessage_SetType(&response, HTTP_RESPONSE);
            HttpMessage_SetVersion(&response, 1, 1);
            HttpMessage_SetResponse(&response, code, status);

            len = HttpMessage_ToString(&response, string, 1024);
            if (len == 0)
            {
                LOG_E(TAG, "HttpMessage_ToString failed");
                break;
            }

            ret = TcpConn_Send(thiz->conn, string, len, UPNP_TIMEOUT);
        } while (0);

        HttpMessage_Dispose(&response);
    } while (0);

    return ret;
}

TinyRet UpnpHttpConnection_SendFile(UpnpHttpConnection *thiz, const char *file)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    return TINY_RET_E_NOT_IMPLEMENTED;
}

TinyRet UpnpHttpConnection_SendFileContent(UpnpHttpConnection *thiz, const char *content, uint32_t contentLength)
{
    TinyRet ret = TINY_RET_OK;
    HttpMessage *response = NULL;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        char *bytes = NULL;
        uint32_t size = 0;

        response = HttpMessage_New();
        if (response == NULL)
        {
            LOG_E(TAG, "HttpMessage_New failed");
            ret = TINY_RET_E_NEW;
            break;
        }

        HttpMessage_SetType(response, HTTP_RESPONSE);
        HttpMessage_SetResponse(response, 200, "OK");
        HttpMessage_SetHeaderInteger(response, "Content-Length", contentLength);

        ret = HttpMessage_ToBytes(response, &bytes, &size);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "HttpMessage_ToBytes failed");
            break;
        }

        ret = TcpConn_Send(thiz->conn, bytes, size, UPNP_TIMEOUT);

        tiny_free(bytes);
        bytes = NULL;
        size = 0;

        ret = TcpConn_Send(thiz->conn, content, contentLength, UPNP_TIMEOUT);
    } while (0);

    if (response != NULL)
    {
        HttpMessage_Delete(response);
    }

    return ret;
}

TinyRet UpnpHttpConnection_SendSubscribeResponse(UpnpHttpConnection *thiz, const char *sid, const char *timeout)
{
    TinyRet ret = TINY_RET_OK;
    HttpMessage *response = NULL;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        char *bytes = NULL;
        uint32_t size = 0;

        response = HttpMessage_New();
        if (response == NULL)
        {
            LOG_E(TAG, "HttpMessage_New failed");
            ret = TINY_RET_E_NEW;
            break;
        }

        HttpMessage_SetType(response, HTTP_RESPONSE);
        HttpMessage_SetResponse(response, 200, "OK");
        HttpMessage_SetHeader(response, "SID", sid);
        HttpMessage_SetHeader(response, "TIMEOUT", timeout);

        ret = HttpMessage_ToBytes(response, &bytes, &size);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "HttpMessage_ToBytes failed");
            break;
        }

        ret = TcpConn_Send(thiz->conn, bytes, size, UPNP_TIMEOUT);

        tiny_free(bytes);
        bytes = NULL;
        size = 0;
    } while (0);

    if (response != NULL)
    {
        HttpMessage_Delete(response);
    }

    return ret;
}