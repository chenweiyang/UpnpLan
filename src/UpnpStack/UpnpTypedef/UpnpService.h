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

typedef void(*UpnpServiceChangedListener)(UpnpService *service, void *ctx);
UPNP_API void UpnpService_SetChangedListener(UpnpService *thiz, UpnpServiceChangedListener listener, void *ctx);
UPNP_API TinyRet UpnpService_SendEvents(UpnpService *thiz);

UPNP_API UpnpActionList * UpnpService_GetActionList(UpnpService *thiz);
UPNP_API UpnpStateList * UpnpService_GetStateList(UpnpService *thiz);

UPNP_API TinyRet UpnpService_SetServiceType(UpnpService *thiz, const char *serviceType);
UPNP_API TinyRet UpnpService_SetServiceId(UpnpService *thiz, const char *serviceId);
UPNP_API TinyRet UpnpService_SetControlURL(UpnpService *thiz, const char *controlURL);
UPNP_API TinyRet UpnpService_SetEventSubURL(UpnpService *thiz, const char *eventSubURL);
UPNP_API TinyRet UpnpService_SetSCPDURL(UpnpService *thiz, const char *SCPDURL);
UPNP_API TinyRet UpnpService_SetCallbackURI(UpnpService *thiz, const char *callbackURI);

UPNP_API const char * UpnpService_GetServiceType(UpnpService *thiz);
UPNP_API const char * UpnpService_GetServiceId(UpnpService *thiz);
UPNP_API const char * UpnpService_GetControlURL(UpnpService *thiz);
UPNP_API const char * UpnpService_GetEventSubURL(UpnpService *thiz);
UPNP_API const char * UpnpService_GetSCPDURL(UpnpService *thiz);
UPNP_API const char * UpnpService_GetCallbackURI(UpnpService *thiz);


TINY_END_DECLS

#endif /* __UPNP_SERVICE_H__ */
