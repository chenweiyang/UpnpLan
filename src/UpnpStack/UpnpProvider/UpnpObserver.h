/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   UpnpObserver.h
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#ifndef __UPNP_OBSERVER_H__
#define __UPNP_OBSERVER_H__

#include "tiny_base.h"
#include "UpnpDevice.h"

TINY_BEGIN_DECLS

typedef void(*DeviceAddedListener)(UpnpDevice *device, void *ctx);
typedef void(*DeviceRemovedListener)(UpnpDevice *device, void *ctx);
typedef void(*ServiceChangedListener)(UpnpService *service, void *ctx);

typedef struct _UpnpObserver
{
    DeviceAddedListener      OnDeviceAdded;
    DeviceRemovedListener    OnDeviceRemoved;
    ServiceChangedListener   OnServiceChanged;
    void                   * ctx;
} UpnpObserver;

UpnpObserver * UpnpObserver_New(void);
TinyRet UpnpObserver_Construct(UpnpObserver *thiz);
TinyRet UpnpObserver_Dispose(UpnpObserver *thiz);
void UpnpObserver_Delete(UpnpObserver *thiz);


TINY_END_DECLS

#endif /* __UPNP_OBSERVER_H__ */