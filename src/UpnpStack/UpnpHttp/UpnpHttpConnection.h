/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   UpnpHttpConnection.h
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#ifndef __UPNP_HTTP_CONNECTION_H__
#define __UPNP_HTTP_CONNECTION_H__

#include "tiny_base.h"
#include "TcpConn.h"
#include "UpnpAction.h"

TINY_BEGIN_DECLS


typedef struct _UpnpHttpConn
{
    TcpConn *conn;
} UpnpHttpConnection;

UpnpHttpConnection * UpnpHttpConnection_New(TcpConn *conn);
TinyRet UpnpHttpConnection_Construct(UpnpHttpConnection *thiz, TcpConn *conn);
void UpnpHttpConnection_Dispose(UpnpHttpConnection *thiz);
void UpnpHttpConnection_Delete(UpnpHttpConnection *thiz);

TinyRet UpnpHttpConnection_SendOk(UpnpHttpConnection *thiz);
TinyRet UpnpHttpConnection_SendError(UpnpHttpConnection *thiz, int code, const char *status);
TinyRet UpnpHttpConnection_SendFile(UpnpHttpConnection *thiz, const char *file);
TinyRet UpnpHttpConnection_SendFileContent(UpnpHttpConnection *thiz, const char *content, uint32_t contentLength);
TinyRet UpnpHttpConnection_SendActionResponse(UpnpHttpConnection *thiz, UpnpAction *action);
TinyRet UpnpHttpConnection_SendSubscribeResponse(UpnpHttpConnection *thiz, const char *sid, const char *timeout);


TINY_END_DECLS

#endif /* __UPNP_HTTP_CONNECTION_H__ */