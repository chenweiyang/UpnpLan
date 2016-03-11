/*
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file  TcpConn.h
 *
 * @remark
 *
 */

#ifndef __TINY_TCP_CONN_H__
#define __TINY_TCP_CONN_H__

#include "tiny_base.h"
#include "TinyThread.h"

TINY_BEGIN_DECLS

#define TCP_CONN_BUFFER_SIZE   (1024 * 50)

typedef enum _TcpConnStatus
{
    TCP_CONN_DISCONNECT = 0,
    TCP_CONN_CONNECTED = 1,
} TcpConnStatus;

struct _TcpConn;
typedef struct _TcpConn TcpConn;
typedef void(*TcpConnListener)(TcpConn *conn, void *ctx);

struct _TcpConn
{
    uint32_t            id;
    TcpConnStatus       status;
    int                 socket_fd;
    uint32_t            recv_buf_size;

    char                self_ip[TINY_IP_LEN];
    char                client_ip[TINY_IP_LEN];
    uint16_t            client_port;

    TinyThread            thread;
    TcpConnListener   listener;
    void              * ctx;
};

TcpConn * TcpConn_New(void);
TinyRet TcpConn_Construct(TcpConn *thiz);
TinyRet TcpConn_Initialize(TcpConn *thiz, uint32_t id, int fd, const char *ip, uint16_t port);
TinyRet TcpConn_Dispose(TcpConn *thiz);
void TcpConn_Delete(TcpConn *thiz);

TinyRet TcpConn_SetListener(TcpConn *thiz, TcpConnListener listener, void *ctx);
TinyRet TcpConn_Start(TcpConn *thiz);

void TcpConn_SetBufferSize(TcpConn *thiz, uint32_t size);
uint32_t TcpConn_GetBufferSize(TcpConn *thiz);

TinyRet TcpConn_Disconnect(TcpConn *thiz);
TcpConnStatus TcpConn_GetStatus(TcpConn *thiz);
uint32_t TcpConn_GetConnectionId(TcpConn * thiz);

uint32_t TcpConn_GetSelfIpBytes(TcpConn *thiz);
TinyRet TcpConn_GetSelfIp(TcpConn *thiz, char *ip, uint32_t len);
const char * TcpConn_GetClientIp(TcpConn *thiz);
uint16_t TcpConn_GetClientPort(TcpConn *thiz);

TinyRet TcpConn_Send(TcpConn *thiz, const char *bytes, uint32_t size, uint32_t timeout);
TinyRet TcpConn_Recv(TcpConn *thiz, char **bytes, uint32_t *size, uint32_t timeout);

typedef void(*TcpConnReceiveListener)(TcpConn *client, const char *buf, uint32_t len, void *ctx);
TinyRet TcpConn_StartRecv(TcpConn *thiz, TcpConnReceiveListener listener, void *ctx, uint32_t timeout);


TINY_END_DECLS

#endif /* __TINY_TCP_CONN_H__ */
