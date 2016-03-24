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
#include "Ssdp.h"
#include "UpnpUsn.h"
#include "UpnpObject.h"
#include "UpnpObjectList.h"
#include "UpnpDevice.h"
#include "UpnpValidator.h"

TINY_BEGIN_DECLS

typedef void(*UpnpObjectListener)(UpnpObject *object, bool alive, void *ctx);

typedef struct _UpnpRegistry
{
    Ssdp                        ssdp;
    UpnpObjectListener          listener;
    void                      * ctx;
    UpnpValidator               validator;
    UpnpObjectList              foundObjects;
} UpnpRegistry;

UpnpRegistry * UpnpRegistry_New(void);
TinyRet UpnpRegistry_Construct(UpnpRegistry *thiz);
void UpnpRegistry_Dispose(UpnpRegistry *thiz);
void UpnpRegistry_Delete(UpnpRegistry *thiz);

TinyRet UpnpRegistry_Start(UpnpRegistry *thiz);
TinyRet UpnpRegistry_Stop(UpnpRegistry *thiz);
TinyRet UpnpRegistry_Discover(UpnpRegistry *thiz, bool strictedUuid, UpnpObjectListener listener, UpnpObjectFilter filter, void *ctx);
TinyRet UpnpRegistry_StopDiscovery(UpnpRegistry *thiz);
TinyRet UpnpRegistry_Register(UpnpRegistry *thiz, UpnpDevice *device);
TinyRet UpnpRegistry_UnRegister(UpnpRegistry *thiz, UpnpDevice *device);


TINY_END_DECLS

#endif /* __UPNP_REGISTRY_H__ */