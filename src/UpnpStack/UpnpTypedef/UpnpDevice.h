/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   UpnpDevice.h
*
* @remark
*
*/

#ifndef __UPNP_DEVICE_H__
#define __UPNP_DEVICE_H__

#include "tiny_base.h"
#include "upnp_api.h"
#include "UpnpService.h"

TINY_BEGIN_DECLS


struct _UpnpDevice;
typedef struct _UpnpDevice UpnpDevice;

UPNP_API UpnpDevice * UpnpDevice_New(void);
UPNP_API void UpnpDevice_Delete(UpnpDevice *thiz);

UPNP_API TinyRet UpnpDevice_SetHttpPort(UpnpDevice *thiz, uint16_t port);
UPNP_API TinyRet UpnpDevice_SetAddress(UpnpDevice *thiz, const char *Address);
UPNP_API TinyRet UpnpDevice_SetURI(UpnpDevice *thiz, const char *URI);
UPNP_API TinyRet UpnpDevice_SetDeviceId(UpnpDevice *thiz, const char *deviceId);
UPNP_API TinyRet UpnpDevice_SetFriendlyName(UpnpDevice *thiz, const char *friendlyName);
UPNP_API TinyRet UpnpDevice_SetDeviceType(UpnpDevice *thiz, const char *deviceType);
UPNP_API TinyRet UpnpDevice_SetManufacturer(UpnpDevice *thiz, const char *manufacturer);
UPNP_API TinyRet UpnpDevice_SetManufacturerURL(UpnpDevice *thiz, const char *manufacturerURL);
UPNP_API TinyRet UpnpDevice_SetModelName(UpnpDevice *thiz, const char *modelName);
UPNP_API TinyRet UpnpDevice_SetModelNumber(UpnpDevice *thiz, const char *modelNumber);
UPNP_API TinyRet UpnpDevice_SetModelURL(UpnpDevice *thiz, const char *modelURL);
UPNP_API TinyRet UpnpDevice_SetSerialNumber(UpnpDevice *thiz, const char *serialNumber);
UPNP_API TinyRet UpnpDevice_SetURLBase(UpnpDevice *thiz, const char *URLBase);

UPNP_API uint16_t UpnpDevice_GetHttpPort(UpnpDevice *thiz);
UPNP_API const char * UpnpDevice_GetAddress(UpnpDevice *thiz);
UPNP_API const char * UpnpDevice_GetURI(UpnpDevice *thiz);
UPNP_API const char * UpnpDevice_GetDeviceId(UpnpDevice *thiz);
UPNP_API const char * UpnpDevice_GetFriendlyName(UpnpDevice *thiz);
UPNP_API const char * UpnpDevice_GetDeviceType(UpnpDevice *thiz);
UPNP_API const char * UpnpDevice_GetManufacturer(UpnpDevice *thiz);
UPNP_API const char * UpnpDevice_GetManufacturerURL(UpnpDevice *thiz);
UPNP_API const char * UpnpDevice_GetModelName(UpnpDevice *thiz);
UPNP_API const char * UpnpDevice_GetModelNumber(UpnpDevice *thiz);
UPNP_API const char * UpnpDevice_GetModelURL(UpnpDevice *thiz);
UPNP_API const char * UpnpDevice_GetSerialNumber(UpnpDevice *thiz);
UPNP_API const char * UpnpDevice_GetURLBase(UpnpDevice *thiz);

UPNP_API TinyRet UpnpDevice_AddService(UpnpDevice *thiz, UpnpService *service);
UPNP_API uint32_t UpnpDevice_GetServiceCount(UpnpDevice *thiz);
UPNP_API UpnpService * UpnpDevice_GetServiceAt(UpnpDevice *thiz, uint32_t index);
UPNP_API UpnpService * UpnpDevice_GetService(UpnpDevice *thiz, const char *serviceId);
UPNP_API UpnpService * UpnpDevice_GetServiceByControlURL(UpnpDevice *thiz, const char *controlURL);
UPNP_API UpnpService * UpnpDevice_GetServiceByEventSubURL(UpnpDevice *thiz, const char *eventSubURL);


TINY_END_DECLS

#endif /* __UPNP_DEVICE_H__ */
