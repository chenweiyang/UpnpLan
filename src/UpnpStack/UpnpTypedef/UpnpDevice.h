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
#include "PropertyList.h"
#include "UpnpServiceList.h"

TINY_BEGIN_DECLS


struct _UpnpDevice;
typedef struct _UpnpDevice UpnpDevice;

UPNP_API UpnpDevice * UpnpDevice_New(void);
UPNP_API void UpnpDevice_Delete(UpnpDevice *thiz);

UPNP_API UpnpServiceList * UpnpDevice_GetServiceList(UpnpDevice *thiz);

UPNP_API PropertyList * UpnpDevice_GetPropertyList(UpnpDevice *thiz);
UPNP_API TinyRet UpnpDevice_SetPropertyValue(UpnpDevice *thiz, const char *propertyName, const char *value);
UPNP_API const char * UpnpDevice_GetPropertyValue(UpnpDevice *thiz, const char *propertyName);


TINY_END_DECLS

#endif /* __UPNP_DEVICE_H__ */
