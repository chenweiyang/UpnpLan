/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   UpnpHttpServer.h
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#ifndef __UPNP_HTTP_SERVER_H__
#define __UPNP_HTTP_SERVER_H__

#include "tiny_base.h"
#include "UpnpHttpConnection.h"
#include "TcpServer.h"

TINY_BEGIN_DECLS


typedef void(*UpnpGetHandler)(UpnpHttpConnection *conn,
const char *uri,
void *ctx);

typedef void(*UpnpPostHandler)(UpnpHttpConnection *conn,
    const char *uri,
    const char *soapAction,
    const char *content,
    void *ctx);

typedef void(*UpnpNotifyHandler)(UpnpHttpConnection *conn,
    const char *uri,
    const char *nt,
    const char *nts,
    const char *sid,
    const char *seq,
    const char *content,
    void *ctx);

typedef void(*UpnpSubscribeHandler)(UpnpHttpConnection *conn,
    const char *uri,
    const char *callback,
    const char *nt,
    const char *timeout,
    void *ctx);

typedef void(*UpnpUnsubscribeHandler)(UpnpHttpConnection *conn,
    const char *uri,
    const char *sid,
    void *ctx);

typedef struct _UpnpHttpServer
{
    TcpServer server;

    UpnpGetHandler OnGet;
    void * OnGetCtx;

    UpnpPostHandler OnPost;
    void * OnPostCtx;

    UpnpNotifyHandler OnNotify;
    void * OnNotifyCtx;

    UpnpSubscribeHandler OnSubscribe;
    void * OnSubscribeCtx;

    UpnpUnsubscribeHandler OnUnsubscribe;
    void * OnUnsubscribeCtx;
} UpnpHttpServer;

UpnpHttpServer * UpnpHttpServer_New(void);
TinyRet UpnpHttpServer_Construct(UpnpHttpServer *thiz);
void UpnpHttpServer_Dispose(UpnpHttpServer *thiz);
void UpnpHttpServer_Delete(UpnpHttpServer *thiz);

TinyRet UpnpHttpServer_RegisterGetHandler(UpnpHttpServer *thiz, UpnpGetHandler handler, void *ctx);
TinyRet UpnpHttpServer_RegisterPostHandler(UpnpHttpServer *thiz, UpnpPostHandler handler, void *ctx);
TinyRet UpnpHttpServer_RegisterNotifyHandler(UpnpHttpServer *thiz, UpnpNotifyHandler handler, void *ctx);
TinyRet UpnpHttpServer_RegisterSubscribeHandler(UpnpHttpServer *thiz, UpnpSubscribeHandler handler, void *ctx);
TinyRet UpnpHttpServer_RegisterUnsubscribeHandler(UpnpHttpServer *thiz, UpnpUnsubscribeHandler handler, void *ctx);
TinyRet UpnpHttpServer_UnregisterGetHandler(UpnpHttpServer *thiz);
TinyRet UpnpHttpServer_UnregisterPostHandler(UpnpHttpServer *thiz);
TinyRet UpnpHttpServer_UnregisterNotifyHandler(UpnpHttpServer *thiz);
TinyRet UpnpHttpServer_UnregisterSubscribeHandler(UpnpHttpServer *thiz);
TinyRet UpnpHttpServer_UnregisterUnsubscribeHandler(UpnpHttpServer *thiz);

TinyRet UpnpHttpServer_Start(UpnpHttpServer *thiz);
TinyRet UpnpHttpServer_Stop(UpnpHttpServer *thiz);
bool UpnpHttpServer_IsRunning(UpnpHttpServer *thiz);
uint16_t UpnpHttpServer_GetListeningPort(UpnpHttpServer *thiz);


TINY_END_DECLS

#endif /* __UPNP_HTTP_SERVER_H__ */