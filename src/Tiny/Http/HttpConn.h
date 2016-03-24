/*
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   HttpConn.h
 *
 * @remark
 *
 */

#ifndef __HTTP_CONN_H__
#define __HTTP_CONN_H__

#include "tiny_base.h"
#include "HttpMessage.h"
#include "HttpContent.h"

TINY_BEGIN_DECLS


struct _HttpConn;
typedef struct _HttpConn HttpConn;

typedef void (* HttpConnListener)(HttpConn *conn, void *ctx);

HttpConn * HttpConn_New(int fd, const char *ip, uint16_t port, HttpConnListener listener,void *ctx);
void HttpConn_Delete(HttpConn * thiz);
int16_t HttpConn_GetSocketFd(HttpConn * thiz);

void HttpConn_SetBufferSize(HttpConn *thiz, uint32_t size);
uint32_t HttpConn_GetBufferSize(HttpConn *thiz);
TinyRet HttpConn_Disconnect(HttpConn *thiz);

typedef enum _HttpConnStatus
{
    HTTP_CONN_DISCONNECT = 0,
    HTTP_CONN_CONNECTED = 1,
} HttpConnStatus;
HttpConnStatus HttpConn_GetStatus(HttpConn *thiz);

uint32_t HttpConn_GetIntAddress(HttpConn *thiz);
const char * HttpConn_GetAddress(HttpConn *thiz);

// client ip & port
const char * HttpConn_GetClientAddress(HttpConn *thiz);
uint16_t HttpConn_GetClientPort(HttpConn *thiz);

// Connection ID
void HttpConn_SetId(HttpConn *thiz, uint32_t id);
uint32_t HttpConn_GetId(HttpConn * thiz);

// send
TinyRet HttpConn_SendMessage(HttpConn *thiz, HttpMessage *msg, uint32_t timeout);
TinyRet HttpConn_SendContent(HttpConn *thiz, HttpContent *content, uint32_t timeout);

// recv
TinyRet HttpConn_RecvMessage(HttpConn *thiz, HttpMessage *msg, uint32_t timeout);
TinyRet HttpConn_RecvMessage(HttpConn *thiz, HttpContent *content, uint32_t timeout);

// recv
typedef void (* HttpConn_RecvHandler)(HttpConn *client, const char *buf, uint32_t len, void *ctx);
TinyRet HttpConn_Recv(HttpConn *thiz, HttpConn_RecvHandler handler, void *ctx, uint32_t timeout);


TINY_END_DECLS


#endif /* __HTTP_CONN_H__ */