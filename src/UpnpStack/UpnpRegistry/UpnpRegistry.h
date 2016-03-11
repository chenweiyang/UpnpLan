/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   UpnpRegistry.h
*
* @remark
*
*/

#ifndef __UPNP_REGISTRY_H__
#define __UPNP_REGISTRY_H__

#include "tiny_base.h"
#include "upnp_api.h"
#include "UpnpUsn.h"
#include "UpnpDevice.h"

TINY_BEGIN_DECLS


struct _UpnpRegistry;
typedef struct _UpnpRegistry UpnpRegistry;

UPNP_API UpnpRegistry * UpnpRegistry_New(void);
UPNP_API void UpnpRegistry_Delete(UpnpRegistry *thiz);

UPNP_API TinyRet UpnpRegistry_Start(UpnpRegistry *thiz);
UPNP_API TinyRet UpnpRegistry_Stop(UpnpRegistry *thiz);

typedef void(*UpnpDeviceListener)(UpnpDevice *device, bool alive, void *ctx);
UPNP_API TinyRet UpnpRegistry_Discover(UpnpRegistry *thiz, UpnpDeviceListener listener, void *ctx);
UPNP_API TinyRet UpnpRegistry_StopDiscovery(UpnpRegistry *thiz);

UPNP_API TinyRet UpnpRegistry_Register(UpnpRegistry *thiz, UpnpDevice *device);
UPNP_API TinyRet UpnpRegistry_UnRegister(UpnpRegistry *thiz, UpnpDevice *device);


TINY_END_DECLS

#endif /* __UPNP_REGISTRY_H__ */