/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   UpnpRegistrar.h
*
* @remark
*
*/

#ifndef __UPNP_REGISTRAR_H__
#define __UPNP_REGISTRAR_H__

#include "tiny_base.h"
#include "upnp_api.h"
#include "UpnpDevice.h"

TINY_BEGIN_DECLS


struct _UpnpRegistrar;
typedef struct _UpnpRegistrar UpnpRegistrar;

UPNP_API UpnpRegistrar * UpnpRegistrar_New(void);
UPNP_API void UpnpRegistrar_Delete(UpnpRegistrar *thiz);

UPNP_API bool UpnpRegistrar_IsRegistered(UpnpRegistrar *thiz, UpnpDevice *device);
UPNP_API TinyRet UpnpRegistrar_Register(UpnpRegistrar *thiz, UpnpDevice *device);
UPNP_API TinyRet UpnpRegistrar_UnRegister(UpnpRegistrar *thiz, UpnpDevice *device);


TINY_END_DECLS

#endif /* __UPNP_REGISTRAR_H__ */