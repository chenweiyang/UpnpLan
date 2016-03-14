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
#include "UpnpObject.h"

TINY_BEGIN_DECLS


struct _UpnpRegistry;
typedef struct _UpnpRegistry UpnpRegistry;

UPNP_API UpnpRegistry * UpnpRegistry_New(void);
UPNP_API void UpnpRegistry_Delete(UpnpRegistry *thiz);

UPNP_API TinyRet UpnpRegistry_Start(UpnpRegistry *thiz);
UPNP_API TinyRet UpnpRegistry_Stop(UpnpRegistry *thiz);

typedef bool(*UpnpObjectFilter)(UpnpUsn *usn, void *ctx);
typedef void(*UpnpObjectListener)(UpnpObject *object, bool alive, void *ctx);
UPNP_API TinyRet UpnpRegistry_Discover(UpnpRegistry *thiz, bool strictedUuid, UpnpObjectListener listener, UpnpObjectFilter filter, void *ctx);
UPNP_API TinyRet UpnpRegistry_StopDiscovery(UpnpRegistry *thiz);

UPNP_API TinyRet UpnpRegistry_Register(UpnpRegistry *thiz, UpnpObject *object);
UPNP_API TinyRet UpnpRegistry_UnRegister(UpnpRegistry *thiz, UpnpObject *object);


TINY_END_DECLS

#endif /* __UPNP_REGISTRY_H__ */