/*
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   TcpServer.c
 *
 * @remark
 *		set tabstop=4
 *		set shiftwidth=4
 *		set expandtab
 */

#include "TcpServer.h"
#include "tiny_memory.h"
#include "tiny_socket.h"
#include "tiny_log.h"

#define TAG         "TcpServer"
#define MAX_CONNS   128

static void accept_loop(void *param);

TcpServer * TcpServer_New(void)
{
    TcpServer *thiz = NULL;

    do
    {
        TinyRet ret = TINY_RET_OK;

        thiz = (TcpServer *)tiny_malloc(sizeof(TcpServer));
        if (thiz == NULL)
        {
            break;
        }

        ret = TcpServer_Construct(thiz);
        if (RET_FAILED(ret))
        {
            TcpServer_Delete(thiz);
            thiz = NULL;
            break;
        }
    }
    while (0);

    return thiz;
}

TinyRet TcpServer_Construct(TcpServer *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(TcpServer));
        thiz->running = false;
        thiz->listen_port = 0;
        thiz->conn_id = 0;

        ret = TinyThread_Construct(&thiz->thread);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = TcpConnPool_Construct(&thiz->conn_pool);
        if (RET_FAILED(ret))
        {
            break;
        }
    }
    while (0);

    return ret;
}

TinyRet TcpServer_Dispose(TcpServer *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    TcpServer_Stop(thiz);
    TinyThread_Dispose(&thiz->thread);
    TcpConnPool_Dispose(&thiz->conn_pool);

    return TINY_RET_OK;
}

void TcpServer_Delete(TcpServer *thiz)
{
    RETURN_IF_FAIL(thiz);

    TcpServer_Dispose(thiz);
    tiny_free(thiz);
}

TinyRet TcpServer_Start(TcpServer *thiz, uint16_t port, TcpConnListener listener, void *ctx)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        if (thiz->running)
        {
            ret = TINY_RET_E_STARTED;
            break;
        }

        thiz->conn_listener = listener;
        thiz->conn_listener_ctx = ctx;

        ret = tiny_tcp_open(&thiz->socket_fd, true);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = tiny_tcp_listen(thiz->socket_fd, port, MAX_CONNS);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = TinyThread_Initialize(&thiz->thread, accept_loop, thiz, "HttpServer");
        if (RET_FAILED(ret))
        {
            break;
        }

        TinyThread_Start(&thiz->thread);
        thiz->running = true;
    }
    while (0);

    return ret;
}

TinyRet TcpServer_Stop(TcpServer *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        if (!thiz->running)
        {
            ret = TINY_RET_E_STOPPED;
            break;
        }

        tiny_tcp_close(thiz->socket_fd);
        thiz->running = false;
    }
    while (0);

    return ret;
}

uint16_t TcpServer_GetListenPort(TcpServer *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, 0);

    if (thiz->listen_port == 0)
    {
        thiz->listen_port = tiny_socket_get_port(thiz->socket_fd);
    }

    return thiz->listen_port;
}

static void accept_loop(void *param)
{
    TcpServer *thiz = (TcpServer *)param;

    while (true)
    {
        TinyRet ret = TINY_RET_OK;
        int fd = 0;
        char ip[TINY_IP_LEN];
        uint16_t port = 0;

        memset(ip, 0, TINY_IP_LEN);
        fd = tiny_tcp_accept(thiz->socket_fd, ip, TINY_IP_LEN, &port);
        if (fd > 0)
        {
            TcpConn *conn = TcpConn_New();
            if (conn == NULL)
            {
                LOG_E(TAG, "TcpConn_New failed");
                break;
            }

            ret = TcpConn_Initialize(conn, thiz->conn_id++, fd, ip, port);
            if (RET_FAILED(ret))
            {
                LOG_E(TAG, "TcpConn_Initialize failed");
                TcpConn_Delete(conn);
                break;
            }

            ret = TcpConn_SetListener(conn, thiz->conn_listener, thiz->conn_listener_ctx);
            if (RET_FAILED(ret))
            {
                LOG_E(TAG, "TcpConn_SetListener failed");
                TcpConn_Delete(conn);
                break;
            }

            ret = TcpConn_Start(conn);
            if (RET_FAILED(ret))
            {
                LOG_E(TAG, "TcpConn_Start failed");
                TcpConn_Delete(conn);
                break;
            }

            TcpConnPool_Add(&thiz->conn_pool, conn);
            TcpConnPool_RemoveClosed(&thiz->conn_pool);
        }
        else
        {
            LOG_D(TAG, "accept failed");
            break;
        }
    }

    TcpConnPool_Clean(&thiz->conn_pool);
    thiz->running = false;
}