/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   UpnpRegistryCore.h
*
* @remark
*
*/

#ifndef __UPNP_REGISTRY_CORE_H__
#define __UPNP_REGISTRY_CORE_H__

#include "tiny_base.h"
#include "TinyThread.h"
#include "TinySelector.h"
#include "TinySocketIpc.h"
#include "HttpMessage.h"

TINY_BEGIN_DECLS

typedef void(*HttpRequestHandler)(HttpMessage *request, void *ctx);
typedef void(*HttpResponseHandler)(HttpMessage *response, void *ctx);

typedef struct _UpnpRegistryCore
{
    TinyThread                  thread;
    TinySelector                selector;
    TinySocketIpc               ipc;
    bool                        running;
    int                         group_fd;
    int                         search_fd;
    HttpRequestHandler          requestHandler;
    HttpResponseHandler         responseHandler;
    void                      * ctx;
} UpnpRegistryCore;

UpnpRegistryCore * UpnpRegistryCore_New(void);
TinyRet UpnpRegistryCore_Construct(UpnpRegistryCore *thiz);
TinyRet UpnpRegistryCore_Dispose(UpnpRegistryCore *thiz);
void UpnpRegistryCore_Delete(UpnpRegistryCore *thiz);
TinyRet UpnpRegistryCore_Start(UpnpRegistryCore *thiz, HttpRequestHandler reqHandler, HttpResponseHandler respHandler, void *ctx);
TinyRet UpnpRegistryCore_Stop(UpnpRegistryCore *thiz);
TinyRet UpnpRegistryCore_Notify(UpnpRegistryCore *thiz, HttpMessage *message);
TinyRet UpnpRegistryCore_SendResponseTo(UpnpRegistryCore *thiz, HttpMessage *message);
TinyRet UpnpRegistryCore_SendRequest(UpnpRegistryCore *thiz, HttpMessage *message);
TinyRet UpnpRegistryCore_SendRequestTo(UpnpRegistryCore *thiz, HttpMessage *message);


TINY_END_DECLS

#endif /* __UPNP_REGISTRY_CORE_H__ */