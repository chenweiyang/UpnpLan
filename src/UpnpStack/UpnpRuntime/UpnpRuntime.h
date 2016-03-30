/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   UpnpRuntime.h
*
* @remark
*
*/

#ifndef __UPNP_RUNTIME_H__
#define __UPNP_RUNTIME_H__

#include "tiny_base.h"
#include "upnp_api.h"
#include "UpnpError.h"
#include "UpnpAction.h"
#include "UpnpDevice.h"
#include "UpnpListener.h"

TINY_BEGIN_DECLS


struct _UpnpRuntime;
typedef struct _UpnpRuntime UpnpRuntime;

UPNP_API UpnpRuntime * UpnpRuntime_New(void);
UPNP_API void UpnpRuntime_Delete(UpnpRuntime *thiz);

UPNP_API TinyRet UpnpRuntime_Start(UpnpRuntime *thiz);
UPNP_API TinyRet UpnpRuntime_Stop(UpnpRuntime *thiz);

/**
 * for UpnpControlPoint
 */
UPNP_API TinyRet UpnpRuntime_StartScan(UpnpRuntime *thiz, UpnpDeviceListener listener, UpnpDeviceFilter filter, void *ctx);
UPNP_API TinyRet UpnpRuntime_StopScan(UpnpRuntime *thiz);
UPNP_API TinyRet UpnpRuntime_Invoke(UpnpRuntime *thiz, UpnpAction *action, UpnpError *error);
UPNP_API TinyRet UpnpRuntime_Subscribe(UpnpRuntime *thiz, UpnpService *service, uint32_t timeout, UpnpEventListener listener, void *ctx, UpnpError *error);
UPNP_API TinyRet UpnpRuntime_Unsubscribe(UpnpRuntime *thiz, UpnpService *service, UpnpError *error);

/**
 * for UpnpDeviceHost
 */
UPNP_API TinyRet UpnpRuntime_Register(UpnpRuntime *thiz, UpnpDevice *device, UpnpActionHandler handler, void *ctx);
UPNP_API TinyRet UpnpRuntime_Unregister(UpnpRuntime *thiz, const char *deviceId);


TINY_END_DECLS

#endif /* __UPNP_RUNTIME_H__ */
