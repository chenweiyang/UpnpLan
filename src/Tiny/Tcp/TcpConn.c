/*
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   TcpConn.c
 *
 * @remark
 *		set tabstop=4
 *		set shiftwidth=4
 *		set expandtab
 */

#include "TcpConn.h"
#include "tiny_memory.h"
#include "tiny_socket.h"
#include "tiny_log.h"


#define TAG     "TcpConn"

static void conn_loop(void *param);

TcpConn * TcpConn_New(void)
{
    TcpConn *thiz = NULL;

    do
    {
        TinyRet ret = TINY_RET_OK;

        thiz = (TcpConn *)tiny_malloc(sizeof(TcpConn));
        if (thiz == NULL)
        {
            break;
        }

        ret = TcpConn_Construct(thiz);
        if (RET_FAILED(ret))
        {
            TcpConn_Delete(thiz);
            thiz = NULL;
            break;
        }
    }
    while (0);

    return thiz;
}

TinyRet TcpConn_Construct(TcpConn *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(TcpConn));

        thiz->id = 0;
        thiz->status = TCP_CONN_DISCONNECT;
        thiz->socket_fd = 0;
        thiz->recv_buf_size = TCP_CONN_BUFFER_SIZE;

        ret = TinyThread_Construct(&thiz->thread);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = TinyThread_Initialize(&thiz->thread, conn_loop, thiz, "http_conn");
        if (RET_FAILED(ret))
        {
            break;
        }
    }
    while (0);

    return ret;
}

TinyRet TcpConn_Initialize(TcpConn *thiz, uint32_t id, int fd, const char *ip, uint16_t port)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    thiz->id = id;
    thiz->status = TCP_CONN_CONNECTED;
    thiz->socket_fd = fd;

    tiny_socket_set_nonblock(thiz->socket_fd);

    strncpy(thiz->client_ip, ip, CT_IP_LEN);
    thiz->client_port = port;

    return TINY_RET_OK;
}

TinyRet TcpConn_Dispose(TcpConn *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    thiz->id = 0;
    thiz->socket_fd = 0;

    memset(thiz->client_ip, 0, CT_IP_LEN);
    thiz->client_port = 0;

    return TINY_RET_OK;
}

void TcpConn_Delete(TcpConn *thiz)
{
    RETURN_IF_FAIL(thiz);

    TcpConn_Dispose(thiz);
    tiny_free(thiz);
}

TinyRet TcpConn_SetListener(TcpConn *thiz, TcpConnListener listener, void *ctx)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(listener, TINY_RET_E_ARG_NULL);

    thiz->listener = listener;
    thiz->ctx = ctx;

    return TINY_RET_OK;
}

TinyRet TcpConn_Start(TcpConn *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    TinyThread_Start(&thiz->thread);

    return TINY_RET_OK;
}

void TcpConn_SetBufferSize(TcpConn *thiz, uint32_t size)
{
    RETURN_IF_FAIL(thiz);

    thiz->recv_buf_size = size;
}

uint32_t TcpConn_GetBufferSize(TcpConn *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, 0);

    return thiz->recv_buf_size;
}

TinyRet TcpConn_Disconnect(TcpConn *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        if (thiz->status == TCP_CONN_DISCONNECT)
        {
            ret = TINY_RET_E_SOCKET_DISCONNECTED;
            break;
        }

        tiny_tcp_close(thiz->socket_fd);
    }
    while (0);

    return ret;
}

TcpConnStatus TcpConn_GetStatus(TcpConn *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TCP_CONN_DISCONNECT);

    return thiz->status;
}

uint32_t TcpConn_GetConnectionId(TcpConn * thiz)
{
    RETURN_VAL_IF_FAIL(thiz, 0);

    return thiz->id;
}

uint32_t TcpConn_GetSelfIpBytes(TcpConn *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, 0);

    return tiny_socket_get_ipv4_bytes(thiz->socket_fd);
}

TinyRet TcpConn_GetSelfIp(TcpConn *thiz, char *ip, uint32_t len)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    return tiny_socket_get_ip(thiz->socket_fd, ip, len);
}

const char * TcpConn_GetClientIp(TcpConn *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->client_ip;
}

uint16_t TcpConn_GetClientPort(TcpConn *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, 0);

    return thiz->client_port;
}

TinyRet TcpConn_Send(TcpConn *thiz, const char *bytes, uint32_t size, uint32_t timeout)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(bytes, TINY_RET_E_ARG_NULL);

    do
    {
        int sent = 0;

        if (thiz->status != TCP_CONN_CONNECTED)
        {
            ret = TINY_RET_E_SOCKET_DISCONNECTED;
            break;
        }

        sent = tiny_tcp_write(thiz->socket_fd, bytes, size);
        ret = (sent > 0) ? TINY_RET_OK : TINY_RET_E_SOCKET_WRITE;
    } while (0);

    return ret;
}

TinyRet TcpConn_Recv(TcpConn *thiz, char **bytes, uint32_t *size, uint32_t timeout)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        if (thiz->status != TCP_CONN_CONNECTED)
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
            tiny_free(*bytes);
            ret = TINY_RET_E_SOCKET_READ;
            break;
        }

    } while (false);

    return ret;
}

TinyRet TcpConn_StartRecv(TcpConn *thiz, TcpConnReceiveListener listener, void *ctx, uint32_t timeout)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(listener, TINY_RET_E_ARG_NULL);

    do
    {
        char * buf = NULL;
        int size = 0;

        if (thiz->status != TCP_CONN_CONNECTED)
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
            if (size > 0)
            {
                listener(thiz, buf, size, ctx);
            }
            else
            {
                ret = TINY_RET_E_SOCKET_READ;
            }
        }

        tiny_free(buf);
    } while (0);

    return ret;
}

static void conn_loop(void *param)
{
    TcpConn *thiz = (TcpConn *)param;
    
    LOG_V(TAG, "conn_loop");

    thiz->listener(thiz, thiz->ctx);
    tiny_tcp_close(thiz->socket_fd);
}