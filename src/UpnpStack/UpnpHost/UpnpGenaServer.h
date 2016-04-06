/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   UpnpGenaServer.h
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#ifndef __UPNP_GENA_SERVER_H__
#define __UPNP_GENA_SERVER_H__

#include "tiny_base.h"
#include "UpnpHttpManager.h"
#include "UpnpProvider.h"
#include "TinyWorker.h"

TINY_BEGIN_DECLS


typedef struct _UpnpGenaServer
{
    UpnpHttpManager *http;
    UpnpProvider *provider;
    TinyWorker notifyWorker;
} UpnpGenaServer;

UpnpGenaServer * UpnpGenaServer_New(UpnpHttpManager *http, UpnpProvider *provider);
TinyRet UpnpGenaServer_Construct(UpnpGenaServer *thiz, UpnpHttpManager *http, UpnpProvider *provider);
void UpnpGenaServer_Dispose(UpnpGenaServer *thiz);
void UpnpGenaServer_Delete(UpnpGenaServer *thiz);

TinyRet UpnpGenaServer_Start(UpnpGenaServer *thiz);
TinyRet UpnpGenaServer_Stop(UpnpGenaServer *thiz);


TINY_END_DECLS

#endif /* __UPNP_GENA_SERVER_H__ */