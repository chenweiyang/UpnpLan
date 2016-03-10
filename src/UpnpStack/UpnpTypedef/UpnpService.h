/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   UpnpService.h
*
* @remark
*
*/

#ifndef __UPNP_SERVICE_H__
#define __UPNP_SERVICE_H__

#include "tiny_base.h"
#include "upnp_api.h"
#include "UpnpActionList.h"
#include "UpnpStateList.h"

TINY_BEGIN_DECLS


struct _UpnpService;
typedef struct _UpnpService UpnpService;

UPNP_API UpnpService * UpnpService_New(void);
UPNP_API void UpnpService_Delete(UpnpService *thiz);

UPNP_API void UpnpService_SetParentDevice(UpnpService *thiz, void *device);
UPNP_API void * UpnpService_GetParentDevice(UpnpService *thiz);

UPNP_API UpnpActionList * UpnpService_GetActionList(UpnpService *thiz);
UPNP_API UpnpStateList * UpnpService_GetStateList(UpnpService *thiz);

UPNP_API PropertyList * UpnpService_GetPropertyList(UpnpService *thiz);
UPNP_API TinyRet UpnpService_SetPropertyValue(UpnpService *thiz, const char *propertyName, const char *value);
UPNP_API const char * UpnpService_GetPropertyValue(UpnpService *thiz, const char *propertyName);


TINY_END_DECLS

#endif /* __UPNP_SERVICE_H__ */
