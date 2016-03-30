/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   UpnpDeviceConfig.h
*
* @remark
*
*/

#ifndef __UPNP_DEVICE_CONFIG_H__
#define __UPNP_DEVICE_CONFIG_H__

#include "tiny_base.h"
#include "upnp_api.h"
#include "UpnpDevice.h"

TINY_BEGIN_DECLS


struct _UpnpDeviceConfig;
typedef struct _UpnpDeviceConfig UpnpDeviceConfig;

UPNP_API UpnpDeviceConfig * UpnpDeviceConfig_New(void);
UPNP_API void UpnpDeviceConfig_Delete(UpnpDeviceConfig *thiz);

UPNP_API void UpnpDeviceConfig_SetDeviceName(UpnpDeviceConfig *thiz, const char *name);
UPNP_API void UpnpDeviceConfig_SetDeviceId(UpnpDeviceConfig *thiz, const char *deviceId);
UPNP_API void UpnpDeviceConfig_SetModelName(UpnpDeviceConfig *thiz, const char *name);
UPNP_API void UpnpDeviceConfig_SetModelNumber(UpnpDeviceConfig *thiz, const char *number);
UPNP_API void UpnpDeviceConfig_SetModelDescription(UpnpDeviceConfig *thiz, const char *description);
UPNP_API void UpnpDeviceConfig_SetModelUrl(UpnpDeviceConfig *thiz, const char *url);
UPNP_API void UpnpDeviceConfig_SetManufacturer(UpnpDeviceConfig *thiz, const char *manufacturer);
UPNP_API void UpnpDeviceConfig_SetManufacturerUrl(UpnpDeviceConfig *thiz, const char *url);

UPNP_API UpnpDevice * UpnpDeviceConfig_CreateDevice(UpnpDeviceConfig *thiz, const char *deviceType);


TINY_END_DECLS

#endif /* __UPNP_DEVICE_CONFIG_H__ */