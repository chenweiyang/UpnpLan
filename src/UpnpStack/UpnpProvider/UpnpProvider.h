/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   UpnpProvider.h
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#ifndef __UPNP_PROVIDER_H__
#define __UPNP_PROVIDER_H__

#include "tiny_base.h"
#include "upnp_define.h"
#include "UpnpDevice.h"
#include "TinyMutex.h"
#include "TinyMap.h"
#include "UpnpListener.h"
#include "UpnpObserver.h"

TINY_BEGIN_DECLS


typedef struct _UpnpProvider
{
    TinyMutex     mutex;
    TinyMap       devices;
    TinyMap       handlers;
    TinyMap       observers;
} UpnpProvider;

typedef struct _UpnpActionHandlerContext
{
    UpnpActionHandler handler;
    void *ctx;
} UpnpActionHandlerContext;

UpnpProvider * UpnpProvider_New(void);
TinyRet UpnpProvider_Construct(UpnpProvider *thiz);
TinyRet UpnpProvider_Dispose(UpnpProvider *thiz);
void UpnpProvider_Delete(UpnpProvider *thiz);

void UpnpProvider_Lock(UpnpProvider *thiz);
void UpnpProvider_Unlock(UpnpProvider *thiz);

TinyRet UpnpProvider_AddObserver(UpnpProvider *thiz, 
    const char *name, 
    DeviceAddedListener OnDeviceAdded,
    DeviceRemovedListener OnDeviceRemoved,
    ServiceChangedListener OnServiceChanged,
    void *ctx);
TinyRet UpnpProvider_RemoveObserver(UpnpProvider *thiz, const char *name);

typedef void(*DeviceVisitor)(UpnpDevice *device, void *ctx);
void UpnpProvider_Foreach(UpnpProvider *thiz, const char *st, DeviceVisitor OnDeviceVisit, void *ctx);

void UpnpProvider_Clear(UpnpProvider *thiz);
UpnpDevice * UpnpProvider_GetDevice(UpnpProvider *thiz, const char *deviceId);
TinyRet UpnpProvider_Add(UpnpProvider *thiz, UpnpDevice *device, UpnpActionHandler handler, void *ctx);
TinyRet UpnpProvider_Remove(UpnpProvider *thiz, const char *deviceId);

uint32_t UpnpProvider_GetDocument(UpnpProvider *thiz, const char *uri, char *content, uint32_t len);
UpnpAction * UpnpProvider_GetAction(UpnpProvider *thiz, const char *uri, const char *serviceType, const char *actionName);
UpnpActionHandlerContext * UpnpProvider_GetActionHandlerContext(UpnpProvider *thiz, UpnpAction *action);


TINY_END_DECLS

#endif /* __UPNP_PROVIDER_H__ */