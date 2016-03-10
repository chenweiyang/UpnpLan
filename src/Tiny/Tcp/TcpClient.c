/*
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   TcpClient.c
 *
 * @remark
 *		set tabstop=4
 *		set shiftwidth=4
 *		set expandtab
 */

#include "TcpClient.h"
#include "tiny_socket.h"
#include "tiny_memory.h"
#include "tiny_log.h"

#define TAG                 "TcpClient"

TcpClient * TcpClient_New(void)
{
    TcpClient *thiz = NULL;

    do
    {
        TinyRet ret = TINY_RET_OK;

        thiz = (TcpClient *)tiny_malloc(sizeof(TcpClient));
        if (thiz == NULL)
        {
            break;
        }

        ret = TcpClient_Construct(thiz);
        if (RET_FAILED(ret))
        {
            TcpClient_Delete(thiz);
            thiz = NULL;
            break;
        }
    }
    while (0);

    return thiz;
}

TinyRet TcpClient_Construct(TcpClient *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    memset(thiz, 0, sizeof(TcpClient));
    thiz->status = TCP_CLIENT_DISCONNECT;
    thiz->socket_fd = 0;
    thiz->recv_buf_size = TCP_CLIENT_BUFFER_SIZE;

    return TINY_RET_OK;
}

TinyRet TcpClient_Dispose(TcpClient *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    TcpClient_Disconnect(thiz);

    return TINY_RET_OK;
}

void TcpClient_Delete(TcpClient *thiz)
{
    RETURN_IF_FAIL(thiz);

    TcpClient_Dispose(thiz);
    tiny_free(thiz);
}

void TcpClient_SetBufferSize(TcpClient *thiz, uint32_t size)
{
    RETURN_IF_FAIL(thiz);

    thiz->recv_buf_size = size;
}

uint32_t TcpClient_GetBufferSize(TcpClient *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, 0);

    return thiz->recv_buf_size;
}

TinyRet TcpClient_Connect(TcpClient *thiz, const char *ip, uint16_t port, uint32_t timeout)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(ip, TINY_RET_E_ARG_NULL);

    LOG_D(TAG, "TcpClient_Connect: %s:%d timeout:%d", ip, port, timeout);

    do
    {
        if (thiz->status != TCP_CLIENT_DISCONNECT)
        {
            ret = TINY_RET_E_SOCKET_CONNECTED;
            break;
        }

        ret = tiny_tcp_open(&thiz->socket_fd, false);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = tiny_tcp_async_connect(thiz->socket_fd, ip, port);
        if (RET_FAILED(ret))
        {
            break;
        }

        thiz->status = TCP_CLIENT_CONNECTING;

        if (ret == TINY_RET_PENDING)
        {
            ret = tiny_tcp_waiting_for_connected(thiz->socket_fd, timeout);
            if (RET_FAILED(ret))
            {
                tiny_tcp_close(thiz->socket_fd);
                break;
            }
        }

        thiz->status = TCP_CLIENT_CONNECTED;

        strncpy(thiz->server_ip, ip, CT_IP_LEN);
        thiz->server_port = port;

        tiny_socket_get_ip(thiz->socket_fd, thiz->self_ip, CT_IP_LEN);
        thiz->self_port = tiny_socket_get_port(thiz->socket_fd);
    }
    while (false);

    thiz->status = RET_SUCCEEDED(ret) ? TCP_CLIENT_CONNECTED : TCP_CLIENT_DISCONNECT;

    return ret;
}

TinyRet TcpClient_Disconnect(TcpClient *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        if (thiz->status == TCP_CLIENT_DISCONNECT)
        {
            ret = TINY_RET_E_SOCKET_DISCONNECTED;
            break;
        }

        tiny_tcp_close(thiz->socket_fd);
        thiz->status = TCP_CLIENT_DISCONNECT;
        ret = TINY_RET_OK;
    }
    while (0);

    return ret;
}

TcpClientStatus TcpClient_GetStatus(TcpClient *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TCP_CLIENT_DISCONNECT);

    return thiz->status;
}

const char * TcpClient_GetSelfIp(TcpClient *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->self_ip;
}

uint16_t TcpClient_GetSelfPort(TcpClient *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, 0);

    return thiz->self_port;
}

const char * TcpClient_GetServerIp(TcpClient *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->server_ip;
}

uint16_t TcpClient_GetServerPort(TcpClient *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, 0);

    return thiz->server_port;
}

TinyRet TcpClient_Send(TcpClient *thiz, const char *bytes, uint32_t size, uint32_t timeout)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(bytes, TINY_RET_E_ARG_NULL);

    do
    {
        int sent = 0;

        if (thiz->status != TCP_CLIENT_CONNECTED)
        {
            ret = TINY_RET_E_SOCKET_DISCONNECTED;
            break;
        }

        ret = tiny_tcp_waiting_for_write(thiz->socket_fd, timeout);
        if (RET_FAILED(ret))
        {
            break;
        }

        sent = tiny_tcp_write(thiz->socket_fd, bytes, size);
        ret = (sent > 0) ? TINY_RET_OK : TINY_RET_E_SOCKET_WRITE;
    } while (false);

    return ret;
}

TinyRet TcpClient_Recv(TcpClient *thiz, char **bytes, uint32_t *size, uint32_t timeout)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        if (thiz->status != TCP_CLIENT_CONNECTED)
        {
            ret = TINY_RET_E_SOCKET_DISCONNECTED;
            break;
        }

        ret = tiny_tcp_waiting_for_read(thiz->socket_fd, timeout);
        if (RET_FAILED(ret))
        {
            break;
        }

        *bytes = (char *)tiny_malloc(thiz->recv_buf_size);
        if (*bytes == NULL)
        {
            ret = TINY_RET_E_OUT_OF_MEMORY;
            break;
        }

        memset(*bytes, 0, thiz->recv_buf_size);

        *size = tiny_tcp_read(thiz->socket_fd, *bytes, thiz->recv_buf_size);
        if (*size <= 0)
        {
            ret = TINY_RET_E_SOCKET_READ;
            break;
        }
    } while (false);

    return ret;
}

TinyRet TcpClient_StartRecv(TcpClient *thiz, TcpClientReceiveListener listener, void *ctx, uint32_t timeout)
{
    TinyRet ret = TINY_RET_OK;
    char * buf = NULL;
    int size = 0;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(listener, TINY_RET_E_ARG_NULL);

    do
    {
        if (thiz->status != TCP_CLIENT_CONNECTED)
        {
            ret = TINY_RET_E_SOCKET_DISCONNECTED;
            break;
        }

        ret = tiny_tcp_waiting_for_read(thiz->socket_fd, timeout);
        if (RET_FAILED(ret))
        {
            break;
        }

        buf = (char *)tiny_malloc(thiz->recv_buf_size);
        if (buf == NULL)
        {
            ret = TINY_RET_E_OUT_OF_MEMORY;
            break;
        }

        while (true)
        {
            memset(buf, 0, thiz->recv_buf_size);

            size = tiny_tcp_read(thiz->socket_fd, buf, thiz->recv_buf_size);
            if (size <= 0)
            {
                ret = TINY_RET_E_SOCKET_READ;
                break;
            }

            listener(thiz, buf, size, ctx);
        }
    }
    while (false);

    if (buf != NULL)
    {
        tiny_free(buf);
    }

    return ret;
}
