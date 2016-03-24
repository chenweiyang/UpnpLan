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
#include "UpnpService.h"
#include "UpnpDevice.h"
#include "UpnpListener.h"
#include "TinyMutex.h"
#include "TinyList.h"

TINY_BEGIN_DECLS


typedef struct _UpnpHost
{
    UpnpHttpManager *http;
    TinyMutex mutex;
    TinyList list;
} UpnpHost;

UpnpHost * UpnpHost_New(UpnpHttpManager *http);
TinyRet UpnpHost_Construct(UpnpHost *thiz, UpnpHttpManager *http);
void UpnpHost_Dispose(UpnpHost *thiz);
void UpnpHost_Delete(UpnpHost *thiz);

TinyRet UpnpHost_Start(UpnpHost *thiz);
TinyRet UpnpHost_Stop(UpnpHost *thiz);

TinyRet UpnpHost_AddDevice(UpnpHost *thiz, UpnpDevice *device, UpnpActionHandler handler, void *ctx);
TinyRet UpnpHost_RemoveDevice(UpnpHost *thiz, UpnpDevice *device);
TinyRet UpnpHost_SendEvents(UpnpHost *thiz, UpnpService *service);


TINY_END_DECLS

#endif /* __UPNP_HOST_H__ */