/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   UpnpHttpManager.h
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#ifndef __UPNP_HTTP_MANAGER_H__
#define __UPNP_HTTP_MANAGER_H__

#include "tiny_base.h"
#include "UpnpHttpServer.h"
#include "UpnpHttpClient.h"

TINY_BEGIN_DECLS


typedef struct _UpnpHttpManager
{
    UpnpHttpClient client;
    UpnpHttpServer server;
} UpnpHttpManager;

UpnpHttpManager * UpnpHttpManager_New(void);
TinyRet UpnpHttpManager_Construct(UpnpHttpManager *thiz);
void UpnpHttpManager_Dispose(UpnpHttpManager *thiz);
void UpnpHttpManager_Delete(UpnpHttpManager *thiz);


TINY_END_DECLS

#endif /* __UPNP_HTTP_MANAGER_H__ */