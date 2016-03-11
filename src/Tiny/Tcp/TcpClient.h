/*
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   TcpClient.h
 *
 * @remark
 *		set tabstop=4
 *		set shiftwidth=4
 *		set expandtab
 */

#ifndef __TINY_TCP_CLIENT_H__
#define __TINY_TCP_CLIENT_H__

#include "tiny_base.h"

TINY_BEGIN_DECLS


#define TCP_CLIENT_BUFFER_SIZE   (1024 * 50)

typedef enum _TcpClientStatus
{
    TCP_CLIENT_DISCONNECT = 0,
    TCP_CLIENT_CONNECTING = 1,
    TCP_CLIENT_CONNECTED = 2,
} TcpClientStatus;

typedef struct _TcpClient
{
    TcpClientStatus     status;
    int                 socket_fd;
    uint32_t            recv_buf_size;

    char                self_ip[TINY_IP_LEN + 1];
    uint16_t            self_port;
    char                server_ip[TINY_IP_LEN + 1];
    uint16_t            server_port;
} TcpClient;

TcpClient * TcpClient_New(void);
TinyRet TcpClient_Construct(TcpClient *thiz);
TinyRet TcpClient_Dispose(TcpClient *thiz);
void TcpClient_Delete(TcpClient *thiz);

void TcpClient_SetBufferSize(TcpClient *thiz, uint32_t size);
uint32_t TcpClient_GetBufferSize(TcpClient *thiz);

TinyRet TcpClient_Connect(TcpClient *thiz, const char *ip, uint16_t port, uint32_t timeout);
TinyRet TcpClient_Disconnect(TcpClient *thiz);
TcpClientStatus TcpClient_GetStatus(TcpClient *thiz);

const char * TcpClient_GetSelfIp(TcpClient *thiz);
uint16_t TcpClient_GetSelfPort(TcpClient *thiz);
const char * TcpClient_GetServerIp(TcpClient *thiz);
uint16_t TcpClient_GetServerPort(TcpClient *thiz);

TinyRet TcpClient_Send(TcpClient *thiz, const char *bytes, uint32_t size, uint32_t timeout);
TinyRet TcpClient_Recv(TcpClient *thiz, char **bytes, uint32_t *size, uint32_t timeout);

typedef void(*TcpClientReceiveListener)(TcpClient *client, const char *buf, uint32_t len, void *ctx);
TinyRet TcpClient_StartRecv(TcpClient *thiz, TcpClientReceiveListener listener, void *ctx, uint32_t timeout);


TINY_END_DECLS

#endif /* __TINY_TCP_CLIENT_H__ */
