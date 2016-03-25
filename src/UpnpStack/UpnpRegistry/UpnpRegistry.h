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
#include "UpnpValidator.h"
#include "UpnpProvider.h"

TINY_BEGIN_DECLS

typedef void(*UpnpObjectListener)(UpnpObject *object, bool alive, void *ctx);

typedef struct _UpnpRegistry
{
    UpnpProvider              * provider;
    Ssdp                        ssdp;
    UpnpObjectListener          listener;
    void                      * ctx;
    UpnpValidator               validator;
    UpnpObjectList              foundObjects;
} UpnpRegistry;

UpnpRegistry * UpnpRegistry_New(UpnpProvider *provider);
TinyRet UpnpRegistry_Construct(UpnpRegistry *thiz, UpnpProvider *provider);
void UpnpRegistry_Dispose(UpnpRegistry *thiz);
void UpnpRegistry_Delete(UpnpRegistry *thiz);
TinyRet UpnpRegistry_Start(UpnpRegistry *thiz);
TinyRet UpnpRegistry_Stop(UpnpRegistry *thiz);
TinyRet UpnpRegistry_Discover(UpnpRegistry *thiz, bool strictedUuid, UpnpObjectListener listener, UpnpObjectFilter filter, void *ctx);
TinyRet UpnpRegistry_StopDiscovery(UpnpRegistry *thiz);


TINY_END_DECLS

#endif /* __UPNP_REGISTRY_H__ */