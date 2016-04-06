/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   UpnpHost.h
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#ifndef __UPNP_HOST_H__
#define __UPNP_HOST_H__

#include "tiny_base.h"
#include "UpnpHttpManager.h"
#include "UpnpProvider.h"
#include "UpnpActionExecutor.h"
#include "UpnpDocumentGetter.h"
#include "UpnpGenaServer.h"

TINY_BEGIN_DECLS


typedef struct _UpnpHost
{
    UpnpHttpManager *http;
    UpnpProvider *provider;
    UpnpActionExecutor actionExecutor;
    UpnpDocumentGetter documentGetter;
    UpnpGenaServer genaServer;
} UpnpHost;

UpnpHost * UpnpHost_New(UpnpHttpManager *http, UpnpProvider *provider);
TinyRet UpnpHost_Construct(UpnpHost *thiz, UpnpHttpManager *http, UpnpProvider *provider);
void UpnpHost_Dispose(UpnpHost *thiz);
void UpnpHost_Delete(UpnpHost *thiz);

TinyRet UpnpHost_Start(UpnpHost *thiz);
TinyRet UpnpHost_Stop(UpnpHost *thiz);


TINY_END_DECLS

#endif /* __UPNP_HOST_H__ */