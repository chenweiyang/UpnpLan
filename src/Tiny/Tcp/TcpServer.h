/*
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   TcpServer.h
 *
 * @remark
 *		set tabstop=4
 *		set shiftwidth=4
 *		set expandtab
 */

#ifndef __TINY_TCP_SERVER_H__
#define __TINY_TCP_SERVER_H__

#include "tiny_base.h"
#include "TcpConn.h"
#include "TcpConnPool.h"
#include "TinyThread.h"

TINY_BEGIN_DECLS


typedef struct _TcpServer
{
    bool                        running;
    int                         socket_fd;
    uint16_t                    listen_port;
    TinyThread                    thread;

    TcpConnPool               conn_pool;
    TcpConnListener           conn_listener;
    void                      * conn_listener_ctx;
    uint32_t                    conn_id;
} TcpServer;

TcpServer * TcpServer_New(void);
TinyRet TcpServer_Construct(TcpServer *thiz);
TinyRet TcpServer_Dispose(TcpServer *thiz);
void TcpServer_Delete(TcpServer *thiz);

TinyRet TcpServer_Start(TcpServer *thiz, uint16_t port, TcpConnListener listener, void *ctx);
TinyRet TcpServer_Stop(TcpServer *thiz);
uint16_t TcpServer_GetListenPort(TcpServer *thiz);


TINY_END_DECLS

#endif /* __TINY_TCP_SERVER_H__ */
